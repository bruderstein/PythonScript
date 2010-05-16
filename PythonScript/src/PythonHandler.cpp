#include "stdafx.h"
#include "PythonHandler.h"

#include "Python.h"

using namespace std;

PythonHandler::PythonHandler(char *pluginsDir, char *configDir)
	: m_machineBaseDir(pluginsDir),
	  m_userBaseDir(configDir)
{
	m_machineBaseDir.append("\\PythonScript\\");
	m_userBaseDir.append("\\PythonScript\\");
}


PythonHandler::~PythonHandler(void)
{
	if (Py_IsInitialized())
	{
		// Can't call finalize with boost::python.
		// Py_Finalize();
	}
}


void PythonHandler::initPython()
{
	if (Py_IsInitialized())
		return;

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
	//Py_InitModule("notepad", notepadMethods);
	//Py_InitModule("buffer", bufferMethods);

	//PyRun_SimpleString("import notepad\n"
    //  				   "import buffer\n");

}


void PythonHandler::runStartupScripts()
{
	// Machine scripts (N++\Plugins\PythonScript\scripts dir)
	string startupPath(m_machineBaseDir);
	startupPath.append("scripts\\startup.py");
	if (fileExists(startupPath.c_str()))
	{
		
		runScript(startupPath);
	}

	// User scripts ($CONFIGDIR$\PythonScript\scripts dir)
	startupPath = m_userBaseDir;
	startupPath.append("scripts\\startup.py");
	if (fileExists(startupPath.c_str()))
	{
		runScript(startupPath);
	}

}


bool PythonHandler::fileExists(const char *fileName)
{
    DWORD       fileAttr;

    fileAttr = GetFileAttributesA(fileName);
    if (0xFFFFFFFF == fileAttr)
        return false;
    return true;
}

void PythonHandler::reinitPython()
{
	if (Py_IsInitialized())
		Py_Finalize();

	initPython();
}


bool PythonHandler::runScript(const string& scriptFile)
{
	return runScript(scriptFile.c_str());
}

bool PythonHandler::runScript(const char *filename)
{
	bool retVal = false;
	int filenameLength = strlen(filename) + 1;
	char *copyFilename = (char*) malloc(filenameLength);
	strcpy_s(copyFilename, filenameLength, filename);
	
	// Why doesn't PyFile_FromString take a const?
	PyObject* pyFile = PyFile_FromString(copyFilename, "r");
	if (pyFile)
	{
		PyRun_SimpleFile(PyFile_AsFile(pyFile), filename);
		Py_DECREF(pyFile);
		retVal = true;
	}

	return retVal;
}

