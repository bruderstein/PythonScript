#include "stdafx.h"
#include "PythonHandler.h"

#include "Scintilla.h"
#include "ScintillaWrapper.h"
#include "ScintillaPython.h"
#include "NotepadPlusWrapper.h"
#include "NotepadPython.h"
#include "PythonConsole.h"
#include "MenuManager.h"


using namespace std;
using namespace NppPythonScript;

PythonHandler::PythonHandler(char *pluginsDir, char *configDir, HINSTANCE hInst, HWND nppHandle, HWND scintilla1Handle, HWND scintilla2Handle, PythonConsole *pythonConsole)
	: PyProducerConsumer<RunScriptArgs*>(),
	  m_nppHandle(nppHandle),
      m_scintilla1Handle(scintilla1Handle),
	  m_scintilla2Handle(scintilla2Handle),
	  m_hInst(hInst),
	  m_machineBaseDir(pluginsDir),
	  m_userBaseDir(configDir),
	  mp_console(pythonConsole),
	  m_currentView(0),
	  mp_mainThreadState(NULL),
	  mp_python(NULL),
	  m_consumerStarted(false)
{
	m_machineBaseDir.append("\\PythonScript\\");
	m_userBaseDir.append("\\PythonScript\\");
	
	mp_notepad = createNotepadPlusWrapper();
	mp_scintilla = createScintillaWrapper();
	mp_scintilla1 = new ScintillaWrapper(scintilla1Handle);
	mp_scintilla2 = new ScintillaWrapper(scintilla2Handle);
	
}


PythonHandler::~PythonHandler(void)
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

}

ScintillaWrapper* PythonHandler::createScintillaWrapper()
{
	m_currentView = mp_notepad->getCurrentView();
	return new ScintillaWrapper(m_currentView ? m_scintilla2Handle : m_scintilla1Handle);
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
	

	// Init paths 
	char initBuffer[1024];
	_snprintf_s(initBuffer, 1024, 1024, 
		"import sys\n"
		"sys.path.append(r'%slib')\n"
		"sys.path.append(r'%slib')\n"
		"sys.path.append(r'%sscripts')\n"
		"sys.path.append(r'%sscripts')\n", 
		m_machineBaseDir.c_str(), 
		m_userBaseDir.c_str(),
		m_machineBaseDir.c_str(), 
		m_userBaseDir.c_str());

	PyRun_SimpleString(initBuffer);
	
	// Init Notepad++/Scintilla modules
	initModules();

	// Initialise threading and create & acquire Global Interpreter Lock
	PyEval_InitThreads();
	
	mp_mainThreadState = PyThreadState_Get();
	PyThreadState_Swap(NULL);

	PyEval_ReleaseLock();
	
}

void PythonHandler::initModules()
{
	importScintilla(mp_scintilla, mp_scintilla1, mp_scintilla2);
	importNotepad(mp_notepad);
	importConsole(mp_console);
}


void PythonHandler::runStartupScripts()
{
	
	// Machine scripts (N++\Plugins\PythonScript\scripts dir)
	string startupPath(m_machineBaseDir);
	startupPath.append("scripts\\startup.py");
	if (::PathFileExistsA(startupPath.c_str()))
	{
		
		runScript(startupPath, true);
	}

	// User scripts ($CONFIGDIR$\PythonScript\scripts dir)
	startupPath = m_userBaseDir;
	startupPath.append("scripts\\startup.py");
	if (::PathFileExistsA(startupPath.c_str()))
	{
		runScript(startupPath, true);
	}

}



bool PythonHandler::runScript(const string& scriptFile, 
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
		int length = strlen(filename) + 1;
		char *filenameCopy = new char[length];
		strcpy_s(filenameCopy, length, filename);
		RunScriptArgs *args = new RunScriptArgs();
		args->filename = filenameCopy;
		args->synchronous = synchronous;
		args->threadState = mp_mainThreadState;
		args->completedEvent = completedEvent;
		args->isStatement = isStatement;

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

void PythonHandler::consume(RunScriptArgs *args)
{
	runScriptWorker(args);
}

void PythonHandler::runScriptWorker(RunScriptArgs *args)
{

	PyGILState_STATE gstate = PyGILState_Ensure();
	
	if (args->isStatement)
	{
		PyRun_SimpleString(args->filename);
	}
	else
	{
		PyObject* pyFile = PyFile_FromString(args->filename, "r");

		if (pyFile)
		{
			PyRun_SimpleFile(PyFile_AsFile(pyFile), args->filename);
			Py_DECREF(pyFile);			
		}
	}
	PyGILState_Release(gstate);
	
	if (NULL != args->completedEvent)
	{
		SetEvent(args->completedEvent);
	}

	delete args->filename;
	delete args;
	
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
	PyGILState_STATE gstate = PyGILState_Ensure();
	
	PyThreadState_SetAsyncExc(handler->getExecutingThreadID(), PyExc_KeyboardInterrupt);
	
	PyGILState_Release(gstate);
}
