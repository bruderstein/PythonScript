#include "stdafx.h"
#include "PythonHandler.h"

#include "Scintilla.h"
#include "ScintillaWrapper.h"
#include "ScintillaPython.h"
#include "NotepadPlusWrapper.h"
#include "NotepadPython.h"
#include "PythonConsole.h"
#include "MenuManager.h"
#include "WcharMbcsConverter.h"
#include "GILManager.h"

namespace NppPythonScript
{

PythonHandler::PythonHandler(TCHAR *pluginsDir, TCHAR *configDir, HINSTANCE hInst, HWND nppHandle, HWND scintilla1Handle, HWND scintilla2Handle, PythonConsole *pythonConsole)
	: PyProducerConsumer<RunScriptArgs>(),
	  m_nppHandle(nppHandle),
      m_scintilla1Handle(scintilla1Handle),
	  m_scintilla2Handle(scintilla2Handle),
	  m_hInst(hInst),
	  m_machineBaseDir(pluginsDir),
	  m_userBaseDir(configDir),
	  mp_console(pythonConsole),
	  m_currentView(0),
	  mp_mainThreadState(NULL),
	  m_consumerStarted(false)
{
	m_machineBaseDir.append(_T("\\PythonScript\\"));
	m_userBaseDir.append(_T("\\PythonScript\\"));
	
	mp_notepad = createNotepadPlusWrapper();
	mp_scintilla = createScintillaWrapper();
	mp_scintilla1 = new ScintillaWrapper(scintilla1Handle, m_nppHandle);
	mp_scintilla2 = new ScintillaWrapper(scintilla2Handle, m_nppHandle);
}

PythonHandler::~PythonHandler(void)
{
	try
	{
		if (Py_IsInitialized())
		{
			if (consumerBusy())
			{
				stopScript();	
			}

			// We need to swap back to the main thread
			PyEval_AcquireLock();
			PyThreadState_Swap(mp_mainThreadState);

			// Can't call finalize with boost::python.
			// Py_Finalize();

		}

		delete mp_scintilla2;
		delete mp_scintilla1;
		delete mp_scintilla;
		delete mp_notepad;

		// To please Lint, let's NULL these handles
		m_hInst = NULL;
		m_nppHandle = NULL;
		m_scintilla1Handle = NULL;
		m_scintilla2Handle = NULL;
		mp_console = NULL;
		mp_mainThreadState = NULL;
	}
	catch (...)
	{
		// I don't know what to do with that, but a destructor should never throw, so...
	}
}


NppPythonScript::ScintillaWrapper* PythonHandler::createScintillaWrapper()
{
	m_currentView = mp_notepad->getCurrentView();
	return new NppPythonScript::ScintillaWrapper(m_currentView ? m_scintilla2Handle : m_scintilla1Handle, m_nppHandle);
}

NotepadPlusWrapper* PythonHandler::createNotepadPlusWrapper()
{
	return new NotepadPlusWrapper(m_hInst, m_nppHandle);
}

void PythonHandler::initPython()
{
	if (Py_IsInitialized())
		return;

	
	preinitScintillaModule();

	// Don't import site - if Python 2.7 doesn't find it as part of Py_Initialize,
	// it does an exit(1) - AGH! 
	Py_NoSiteFlag = 1;

	Py_Initialize();
    // Initialise threading and create & acquire Global Interpreter Lock
	PyEval_InitThreads();


	std::shared_ptr<char> machineBaseDir = WcharMbcsConverter::tchar2char(m_machineBaseDir.c_str());
	std::shared_ptr<char> configDir = WcharMbcsConverter::tchar2char(m_userBaseDir.c_str());
	
	bool machineIsUnicode = containsExtendedChars(machineBaseDir.get());
	bool userIsUnicode    = containsExtendedChars(configDir.get());
	
	
	std::string smachineDir(machineBaseDir.get());
	std::string suserDir(configDir.get());
	
	

	// Init paths 
	char initBuffer[1024];

	_snprintf_s(initBuffer, 1024, 1024, 
		"import sys\n"
		"sys.path.append(r'%slib'%s)\n"
		"sys.path.append(r'%slib'%s)\n"
		"sys.path.append(r'%sscripts'%s)\n"
		"sys.path.append(r'%sscripts'%s)\n", 
		
		smachineDir.c_str(), 
		machineIsUnicode ? ".decode('utf8')" : "",
		
		suserDir.c_str(),
		userIsUnicode ? ".decode('utf8')" : "",
		
		smachineDir.c_str(), 
		machineIsUnicode ? ".decode('utf8')" : "",
		
		suserDir.c_str(),
		userIsUnicode ? ".decode('utf8')" : "");

	PyRun_SimpleString(initBuffer);
	
	// Init Notepad++/Scintilla modules
	initModules();

    /* Old manual version of PyEval_SaveThread() 
	mp_mainThreadState = PyThreadState_Get();
	PyThreadState_Swap(NULL);

	PyEval_ReleaseLock();
    */
    mp_mainThreadState = PyEval_SaveThread();
	
}

void PythonHandler::initModules()
{
	importScintilla(mp_scintilla, mp_scintilla1, mp_scintilla2);
	importNotepad(mp_notepad);
	importConsole(mp_console);
}


bool PythonHandler::containsExtendedChars(char *s)
{
	bool retVal = false;
	for(int pos = 0; s[pos]; ++pos)
	{
		if (s[pos] & 0x80)
		{
			retVal = true;
			break;
		}
	}
	return retVal;
}

void PythonHandler::runStartupScripts()
{
	
	// Machine scripts (N++\Plugins\PythonScript\scripts dir)
	std::string startupPath(WcharMbcsConverter::tchar2char(m_machineBaseDir.c_str()).get());
	startupPath.append("scripts\\startup.py");
	if (::PathFileExistsA(startupPath.c_str()))
	{
		
		runScript(startupPath, true);
	}

	// User scripts ($CONFIGDIR$\PythonScript\scripts dir)
	startupPath = WcharMbcsConverter::tchar2char(m_userBaseDir.c_str()).get();
	startupPath.append("scripts\\startup.py");
	if (::PathFileExistsA(startupPath.c_str()))
	{
		runScript(startupPath, true);
	}

}

bool PythonHandler::runScript(const std::string& scriptFile, 
							  bool synchronous /* = false */, 
							  bool allowQueuing /* = false */,
							  HANDLE completedEvent /* = NULL */,
							  bool isStatement /* = false */)
{
	return runScript(scriptFile.c_str(), synchronous, allowQueuing, completedEvent, isStatement);
}

bool PythonHandler::runScript(const char *filename, 
							  bool synchronous /* = false */, 
							  bool allowQueuing /* = false */,
							  HANDLE completedEvent /* = NULL */,
							  bool isStatement /* = false */)
{
	bool retVal;

	if (!allowQueuing && consumerBusy())
	{
		retVal = false;
	}
	else
	{
		std::shared_ptr<RunScriptArgs> args(
			new RunScriptArgs(
				filename,
				mp_mainThreadState,
				synchronous,
				completedEvent,
				isStatement));

		if (!synchronous)
		{
			retVal = produce(args);
			if (!m_consumerStarted)
			{
				startConsumer();
			}
		}
		else
		{
			runScriptWorker(args);
			retVal = true;
		}
	}
	return retVal;
}

void PythonHandler::consume(std::shared_ptr<RunScriptArgs> args)
{
	runScriptWorker(args);
}

void PythonHandler::runScriptWorker(const std::shared_ptr<RunScriptArgs>& args)
{

    NppPythonScript::GILLock gilLock = NppPythonScript::GILManager::getGIL();
	
	if (args->m_isStatement)
	{
		PyRun_SimpleString(args->m_filename.c_str());
	}
	else
	{
		// We assume PyFile_FromString won't modify the file name passed in param
		// (that would be quite troubling) and that the missing 'const' is simply an oversight
		// from the Python API developers. 
		// We also assume the second parameter, "r" won't be modified by the function call.
		//lint -e{1776}  Converting a string literal to char * is not const safe (arg. no. 2)
		PyObject* pyFile = PyFile_FromString(const_cast<char *>(args->m_filename.c_str()), "r");

		if (pyFile)
		{
			PyRun_SimpleFile(PyFile_AsFile(pyFile), args->m_filename.c_str());
			Py_DECREF(pyFile);			
		}
	}
	
	if (NULL != args->m_completedEvent)
	{
		SetEvent(args->m_completedEvent);
	}
}

void PythonHandler::notify(SCNotification *notifyCode)
{
	if (notifyCode->nmhdr.hwndFrom == m_scintilla1Handle || notifyCode->nmhdr.hwndFrom == m_scintilla2Handle)
	{
		mp_scintilla->notify(notifyCode);
	}
	else if (notifyCode->nmhdr.hwndFrom != mp_console->getScintillaHwnd()) // ignore console notifications
	{
		// Change the active scintilla handle for the "buffer" variable if the active buffer has changed
		if (notifyCode->nmhdr.code == NPPN_BUFFERACTIVATED)
		{
			int newView = mp_notepad->getCurrentView();
			if (newView != m_currentView)
			{
				m_currentView = newView;
				mp_scintilla->setHandle(newView ? m_scintilla2Handle : m_scintilla1Handle);
			}
		}

		mp_notepad->notify(notifyCode);
	}
}

void PythonHandler::queueComplete()
{
	MenuManager::getInstance()->stopScriptEnabled(false);
}


void PythonHandler::stopScript()
{
	DWORD threadID;
	CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(stopScriptWorker), this, 0, &threadID);
}


void PythonHandler::stopScriptWorker(PythonHandler *handler)
{
    NppPythonScript::GILLock gilLock = NppPythonScript::GILManager::getGIL();
	
	PyThreadState_SetAsyncExc((long)handler->getExecutingThreadID(), PyExc_KeyboardInterrupt);
	
}


}