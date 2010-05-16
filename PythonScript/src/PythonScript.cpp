// Plugin Template
//

#include "stdafx.h"
#include "PluginInterface.h"
#include "PythonScript.h"
#include "AboutDialog.h"


#include "WcharMbcsConverter.h"
#include "PythonHandler.h"

#include "Python.h"
#include <boost/python.hpp>

using namespace boost::python;

using namespace std;



/* Info for Notepad++ */
CONST TCHAR PLUGIN_NAME[]	= _T("Python Script");

FuncItem	*funcItem = NULL;

/* Global data */
NppData				nppData;
HANDLE				g_hModule			= NULL;
TCHAR				iniFilePath[MAX_PATH];

/* Dialogs */
AboutDialog		aboutDlg;

// Paths
char g_pluginDir[MAX_PATH];
char g_configDir[MAX_PATH];


void doAbout();
void loadSettings();
void saveSettings();
void initialisePython();
void runScript1();
void runScript2();
//void initgreettest();


HWND getCurrentHScintilla(int which);

// Main python handler/wrapper
PythonHandler *pythonHandler;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	g_hModule = hModule;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		  break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;

	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

void insertScintillaText(char *text, int length)
{

	SendMessage(getCurrentHScintilla(0), SCI_ADDTEXT, length, reinterpret_cast<LPARAM>(text));
}

PyObject* outputRedirect(PyObject* pSelf, PyObject* pArgs)
{
	char* output = NULL;
	int length;
	if (!PyArg_ParseTuple(pArgs, "s#", &output, &length)) 
	{
		return NULL;
	}

	insertScintillaText(output, length);
	
	Py_INCREF(Py_None);
    return Py_None;
}



static PyMethodDef outputRedirectionMethods[] = {

    // "Python name", C Ffunction Code, Argument Flags, __doc__ description
    {"nppoutput", outputRedirect, METH_VARARGS, "Output redirector for Notepad++."},
    {NULL, NULL, 0, NULL}
};


const char *greet()
{
	return "hello from boost";
}

const char *doubles(const char* s)
{
	return s;
}


BOOST_PYTHON_MODULE(greettest)
{
	def("greet", greet);
	def("doubles", doubles);
}


extern "C" __declspec(dllexport) void setInfo(NppData notepadPlusData)
{
	nppData = notepadPlusData;
	aboutDlg.init((HINSTANCE)g_hModule, nppData);


	loadSettings();
	
	

}

extern "C" __declspec(dllexport) CONST TCHAR * getName()
{
	return PLUGIN_NAME;
}

extern "C" __declspec(dllexport) FuncItem * getFuncsArray(int *nbF)
{
	int nbFunc = 3;
	funcItem = (FuncItem*) malloc(sizeof(FuncItem) * nbFunc);
	memset(funcItem, 0, sizeof(FuncItem) * nbFunc);
	_tcscpy_s(funcItem[0]._itemName, 64, _T("Script 1"));
	funcItem[0]._pFunc = runScript1;
	_tcscpy_s(funcItem[1]._itemName, 64, _T("Script 1"));
	funcItem[1]._pFunc = runScript2;
	_tcscpy_s(funcItem[2]._itemName, 64, _T("About"));
	funcItem[2]._pFunc = doAbout;
	
	*nbF = nbFunc;
	return funcItem;
}

HWND getCurrentHScintilla(int which)
{
	return (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;
};

void initialise()
{
	pythonHandler = new PythonHandler(g_pluginDir, g_configDir);
	PyImport_AppendInittab( "greettest", &initgreettest);
	pythonHandler->initPython();

	Py_InitModule("nppoutput", outputRedirectionMethods);
	pythonHandler->runStartupScripts();
	/* PyRun_SimpleString(
		"import sys\n"
		"import nppoutput\n"
		"class StdoutRedirect:\n"
		"\tdef write(self, str):\n"
		"\t\tnppoutput.nppoutput(str)\n"
		"sys.stdout = StdoutRedirect()\n"
		"sys.stderr = StdoutRedirect()\n");
		*/
	// PyImport_ImportModule("greettest");	
	object main_module((
      handle<>(borrowed(PyImport_AddModule("__main__")))));

    object main_namespace = main_module.attr("__dict__");
	object greettest_module( (handle<>(PyImport_ImportModule("greettest"))) );
	main_namespace["greettest"] = greettest_module;
}

extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode)
{
	switch(notifyCode->nmhdr.code)
	{
		case NPPN_READY:
			{
				MessageBox(NULL, _T("NPPN_READY"), _T("Python Script"), 0);
				TCHAR temp[MAX_PATH];
				::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(temp));
				strcpy_s(g_configDir, MAX_PATH, WcharMbcsConverter::tchar2char(temp).get());

				::SendMessage(nppData._nppHandle, NPPM_GETNPPDIRECTORY, MAX_PATH, reinterpret_cast<LPARAM>(temp));
				strcpy_s(g_pluginDir, MAX_PATH, WcharMbcsConverter::tchar2char(temp).get());
				strcat_s(g_pluginDir, MAX_PATH, "\\plugins");
			
				initialise();
			}
			break;

		case NPPN_SHUTDOWN:
			saveSettings();
			delete pythonHandler;

			if (funcItem != NULL)
				free(funcItem);
			break;
	}

}

extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}


#ifdef _UNICODE
extern "C" __declspec(dllexport) BOOL isUnicode()
{
	return TRUE;
}
#endif


void doAbout()
{
	aboutDlg.doDialog();
}



void loadSettings(void)
{
	TCHAR configPath[MAX_PATH];
	/* initialize the config directory */
	::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)configPath);

	/* Test if config path exist */
	if (::PathFileExists(configPath) == FALSE) {
		::CreateDirectory(configPath, NULL);
	}

	
	_tcscpy_s(iniFilePath, MAX_PATH, configPath);
	_tcscat_s(iniFilePath, MAX_PATH, PLUGINTEMPLATE_INI);
	if (PathFileExists(iniFilePath) == FALSE)
	{
		::CloseHandle(::CreateFile(iniFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));
	}
	
	
}


void saveSettings(void)
{
	
}






void runScript1()
{
	pythonHandler->runScript("E:\\script1.py");
}

void runScript2()
{
	pythonHandler->runScript("E:\\script2.py");
}








