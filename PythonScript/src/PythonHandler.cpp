#include "stdafx.h"
#include "PythonHandler.h"

#include "Python.h"

#include "ScintillaWrapper.h"
#include "ScintillaPython.h"
#include "NotepadPlusWrapper.h"
#include "NotepadPython.h"
#include "PluginInterface.h"

using namespace std;

PythonHandler::PythonHandler(char *pluginsDir, char *configDir, HWND nppHandle, HWND scintilla1Handle, HWND scintilla2Handle)
	: m_machineBaseDir(pluginsDir),
	  m_userBaseDir(configDir),
	  m_nppHandle(nppHandle),
	  m_scintilla1Handle(scintilla1Handle),
	  m_scintilla2Handle(scintilla2Handle)
{
	m_machineBaseDir.append("\\PythonScript\\");
	m_userBaseDir.append("\\PythonScript\\");

	mp_scintilla = createScintillaWrapper();
	mp_notepad = createNotepadPlusWrapper();
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
	// Default to 1st scintilla handle initially
	return new ScintillaWrapper(m_scintilla1Handle);
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


}

void PythonHandler::initModules()
{
	importScintilla(mp_scintilla);
	importNotepad(mp_notepad);
}


void PythonHandler::runStartupScripts()
{
	// Machine scripts (N++\Plugins\PythonScript\scripts dir)
	string startupPath(m_machineBaseDir);
	startupPath.append("scripts\\startup.py");
	if (::PathFileExistsA(startupPath.c_str()))
	{
		
		runScript(startupPath);
	}

	// User scripts ($CONFIGDIR$\PythonScript\scripts dir)
	startupPath = m_userBaseDir;
	startupPath.append("scripts\\startup.py");
	if (::PathFileExistsA(startupPath.c_str()))
	{
		runScript(startupPath);
	}

}



bool PythonHandler::runScript(const string& scriptFile)
{
	return runScript(scriptFile.c_str());
}

bool PythonHandler::runScript(const char *filename)
{
	bool retVal = false;
	
	// Why doesn't PyFile_FromString take a const?
	// It doesn't modify the string, so const_cast is safe
	PyObject* pyFile = PyFile_FromString(const_cast<char*>(filename), "r");
	if (pyFile)
	{
		PyRun_SimpleFile(PyFile_AsFile(pyFile), filename);
		Py_DECREF(pyFile);
		retVal = true;
	}

	return retVal;
}

void PythonHandler::notify(SCNotification *notifyCode)
{
	if (notifyCode->nmhdr.hwndFrom == m_scintilla1Handle || notifyCode->nmhdr.hwndFrom == m_scintilla2Handle)
	{
		mp_scintilla->notify(notifyCode);
	}
	else
	{
		mp_notepad->notify(notifyCode);
	}
}