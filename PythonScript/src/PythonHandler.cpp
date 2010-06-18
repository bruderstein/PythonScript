#include "stdafx.h"
#include "PythonHandler.h"

#include "Python.h"

#include "ScintillaWrapper.h"
#include "ScintillaPython.h"
#include "NotepadPlusWrapper.h"
#include "NotepadPython.h"
#include "PythonConsole.h"
#include "PluginInterface.h"

using namespace std;

PythonHandler::PythonHandler(char *pluginsDir, char *configDir, HWND nppHandle, HWND scintilla1Handle, HWND scintilla2Handle, PythonConsole *pythonConsole)
	: m_machineBaseDir(pluginsDir),
	  m_userBaseDir(configDir),
	  m_nppHandle(nppHandle),
	  m_currentView(0),
	  m_scintilla1Handle(scintilla1Handle),
	  m_scintilla2Handle(scintilla2Handle),
	  mp_console(pythonConsole),
	  m_hThread(NULL)
{
	m_machineBaseDir.append("\\PythonScript\\");
	m_userBaseDir.append("\\PythonScript\\");
	
	mp_notepad = createNotepadPlusWrapper();
	mp_scintilla = createScintillaWrapper();
	
	m_scriptRunning = CreateEvent(NULL, FALSE, TRUE, NULL);
}


PythonHandler::~PythonHandler(void)
{
	if (Py_IsInitialized())
	{
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
	return new NotepadPlusWrapper(m_nppHandle);
}


void PythonHandler::initPython()
{
	if (Py_IsInitialized())
		return;

	
	preinitScintillaModule();
	
	Py_Initialize();
	

	// Init paths 
	char initBuffer[1024];
	_snprintf_s(initBuffer, 1024, 1024, 
		"import sys\n"
		"sys.path.append('%slib')\n"
		"sys.path.append('%slib')\n"
		"sys.path.append('%sscripts')\n"
		"sys.path.append('%sscripts')\n", 
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
	importScintilla(mp_scintilla);
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



bool PythonHandler::runScript(const string& scriptFile, bool synchronous /* = false */)
{
	return runScript(scriptFile.c_str(), synchronous);
}


bool PythonHandler::runScript(const char *filename, bool synchronous /* = false */)
{
	DWORD result;
	/*if (synchronous)
		result = WaitForSingleObject(m_scriptRunning, INFINITE);
	else
		result = WaitForSingleObject(m_scriptRunning, 0);
	*/
	bool retVal = false;
	//if (result == WAIT_OBJECT_0)
	//{
		// Close the old thread
		//if (m_hThread != NULL)
		//	CloseHandle(m_hThread);

		int length = strlen(filename) + 1;
		char *filenameCopy = new char[length];
		strcpy_s(filenameCopy, length, filename);
		RunScriptArgs *args = new RunScriptArgs();
		args->instance = this;
		args->filename = filenameCopy;
		args->waitHandle = m_scriptRunning;
		args->synchronous = synchronous;
		args->threadState = mp_mainThreadState;

		if (!synchronous)
		{
			
			
			m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)runScriptWorker, args, 0, &m_dwThreadId);

			if (m_hThread != NULL)
				retVal = true;
		}
		else
		{
			args->threadState = mp_mainThreadState;
			runScriptWorker(args);
			retVal = true;
		}

		
	

	return retVal;

}



void PythonHandler::runScriptWorker(RunScriptArgs *args)
{

	/*
	PyThreadState *threadState = PyThreadState_New(args->threadState->interp);

	PyEval_AcquireLock();

	
		
	

	PyThreadState_Swap(threadState);
	*/
	PyGILState_STATE gstate = PyGILState_Ensure();
	
	PyObject* pyFile = PyFile_FromString(args->filename, "r");

	if (pyFile)
	{
		PyRun_SimpleFile(PyFile_AsFile(pyFile), args->filename);
		Py_DECREF(pyFile);			
	}
	
	/*
	PyThreadState_Swap(NULL);
	PyEval_ReleaseLock();
	*/

	PyGILState_Release(gstate);

	delete args->filename;
	delete args;
	
}

void PythonHandler::notify(SCNotification *notifyCode)
{
	if (notifyCode->nmhdr.hwndFrom == m_scintilla1Handle || notifyCode->nmhdr.hwndFrom == m_scintilla2Handle)
	{
		mp_scintilla->notify(notifyCode);
	}
	else if (notifyCode->nmhdr.hwndFrom != mp_console->getScintillaHwnd())
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