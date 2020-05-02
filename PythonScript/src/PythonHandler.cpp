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
#include "ConfigFile.h"

namespace NppPythonScript
{

PythonHandler::PythonHandler(TCHAR *pluginsDir, TCHAR *configDir, HINSTANCE hInst, HWND nppHandle, HWND scintilla1Handle, HWND scintilla2Handle, boost::shared_ptr<PythonConsole> pythonConsole)
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
	mp_scintilla1.reset(new ScintillaWrapper(scintilla1Handle, m_nppHandle));
	mp_scintilla2.reset(new ScintillaWrapper(scintilla2Handle, m_nppHandle));
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
			GILLock lock;  // It's actually pointless, as we don't need it anymore,
			               // but we'll grab it anyway, just in case we need to wait for something to finish

			// Can't call finalize with boost::python.
			// Py_Finalize();

		}


		// To please Lint, let's NULL these handles
		m_hInst = NULL;
		m_nppHandle = NULL;
		mp_mainThreadState = NULL;
	}
	catch (...)
	{
		// I don't know what to do with that, but a destructor should never throw, so...
	}
}


boost::shared_ptr<ScintillaWrapper> PythonHandler::createScintillaWrapper()
{
	m_currentView = mp_notepad->getCurrentView();
	return boost::shared_ptr<ScintillaWrapper>(new ScintillaWrapper(m_currentView ? m_scintilla2Handle : m_scintilla1Handle, m_nppHandle));
}

boost::shared_ptr<NotepadPlusWrapper> PythonHandler::createNotepadPlusWrapper()
{
	return boost::shared_ptr<NotepadPlusWrapper>(new NotepadPlusWrapper(m_hInst, m_nppHandle));
}

void PythonHandler::initPython()
{
	if (Py_IsInitialized())
		return;


	preinitScintillaModule();

	// Don't import site - if Python 2.7 doesn't find it as part of Py_Initialize,
	// it does an exit(1) - AGH!
	Py_NoSiteFlag = 1;
	Py_IgnoreEnvironmentFlag = 1;
	Py_NoUserSiteDirectory = 1;

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
    char pathCommands[500];

    // If the user wants to use their installed python version, append the paths.
    // If not (and they want to use the bundled python install), the default, then prepend the paths
    if (ConfigFile::getInstance()->getSetting(_T("PREFERINSTALLEDPYTHON")) == _T("1")) {
        strcpy_s<500>(pathCommands, "import sys\n"
            "sys.path.append(r'%slib'%s)\n"
            "sys.path.append(r'%slib'%s)\n"
            "sys.path.append(r'%sscripts'%s)\n"
            "sys.path.append(r'%sscripts'%s)\n"
			"sys.path.append(r'%slib\\lib-tk'%s)\n" );
	} else {
        strcpy_s<500>(pathCommands, "import sys\n"
            "sys.path.insert(0,r'%slib'%s)\n"
            "sys.path.insert(1,r'%slib'%s)\n"
            "sys.path.insert(2,r'%sscripts'%s)\n"
            "sys.path.insert(3,r'%sscripts'%s)\n"
            "sys.path.insert(4,r'%slib\\lib-tk'%s)\n"
			);
	}

	_snprintf_s(initBuffer, 1024, 1024,
        pathCommands,
		smachineDir.c_str(),
		machineIsUnicode ? ".decode('utf8')" : "",

		suserDir.c_str(),
		userIsUnicode ? ".decode('utf8')" : "",

		smachineDir.c_str(),
		machineIsUnicode ? ".decode('utf8')" : "",

		suserDir.c_str(),
		userIsUnicode ? ".decode('utf8')" : "",

		smachineDir.c_str(),
		machineIsUnicode ? ".decode('utf8')" : ""
		);

	PyRun_SimpleString(initBuffer);

    initSysArgv();


	// Init Notepad++/Scintilla modules
	initModules();

    mp_mainThreadState = PyEval_SaveThread();

}

void PythonHandler::initSysArgv()
{
    LPWSTR commandLine = ::GetCommandLineW();
    int argc;
    LPWSTR* argv = ::CommandLineToArgvW(commandLine, &argc);


    boost::python::list argvList;
    for(int currentArg = 0; currentArg != argc; ++currentArg)
	{
        std::shared_ptr<char> argInUtf8 = WcharMbcsConverter::wchar2char(argv[currentArg]);
        PyObject* unicodeArg = PyUnicode_FromString(argInUtf8.get());

		argvList.append(boost::python::handle<>(unicodeArg));
    }

    boost::python::object sysModule(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("sys"))));
    sysModule.attr("argv") = argvList;


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
	tstring startupPath(m_machineBaseDir);
	startupPath.append(_T("scripts\\startup.py"));
	if (::PathFileExists(startupPath.c_str()))
	{

		runScript(startupPath, true);
	}

	// User scripts ($CONFIGDIR$\PythonScript\scripts dir)
	startupPath = m_userBaseDir;
	startupPath.append(_T("scripts\\startup.py"));
	if (::PathFileExists(startupPath.c_str()))
	{
		runScript(startupPath, true);
	}

}

bool PythonHandler::runScript(const tstring& scriptFile,
							  bool synchronous /* = false */,
							  bool allowQueuing /* = false */,
							  HANDLE completedEvent /* = NULL */,
							  bool isStatement /* = false */)
{
	return runScript(scriptFile.c_str(), synchronous, allowQueuing, completedEvent, isStatement);
}

bool PythonHandler::runScript(const TCHAR *filename,
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

    GILLock gilLock;

	if (args->m_isStatement)
	{
		if (PyRun_SimpleString(WcharMbcsConverter::tchar2char(args->m_filename.c_str()).get()) == -1)
		{
			if (ConfigFile::getInstance()->getSetting(_T("ADDEXTRALINETOOUTPUT")) == _T("1"))
			{
				mp_console->writeText(boost::python::str("\n"));
			}
			
			if (ConfigFile::getInstance()->getSetting(_T("OPENCONSOLEONERROR")) == _T("1"))
			{
				mp_console->pythonShowDialog();
			}
		}
	}
	else
	{
		FILE* pyFile = _Py_wfopen(args->m_filename.c_str(), _T("rb"));
		if (pyFile)
		{
			if (PyRun_SimpleFileEx(pyFile, WcharMbcsConverter::tchar2char(args->m_filename.c_str()).get(), 1) == -1)
			{
				if (ConfigFile::getInstance()->getSetting(_T("ADDEXTRALINETOOUTPUT")) == _T("1"))
				{
					mp_console->writeText(boost::python::str("\n"));
				}
				
				if (ConfigFile::getInstance()->getSetting(_T("OPENCONSOLEONERROR")) == _T("1"))
				{
					mp_console->pythonShowDialog();
				}
			}
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
    GILLock gilLock;

	PyThreadState_SetAsyncExc((long)handler->getExecutingThreadID(), PyExc_KeyboardInterrupt);

}


}