// Plugin Template
//

#include "stdafx.h"
#include "PluginInterface.h"
#include "PythonScript.h"
#include "AboutDialog.h"
#include "ConsoleDialog.h"
#include "MenuManager.h"

#include "WcharMbcsConverter.h"
#include "PythonHandler.h"
#include "PythonConsole.h"
#include "NotepadPlusWrapper.h"
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

PythonConsole   g_console;
// Paths
char g_pluginDir[MAX_PATH];
char g_configDir[MAX_PATH];

bool g_infoSet = false;
int g_aboutFuncIndex = 0;

// Scripts on the menu
vector<string*> g_menuScripts;

// Scripts on the toolbar
vector< pair<string*, HICON>* > g_toolbarScripts;



void doAbout();
void loadSettings();
void saveSettings();
void newScript();
void showConsole();
void runScript(int);
void runScript(const char*);
FuncItem* getGeneratedFuncItemArray(int *nbF);



// Run script functions
void runScript0() { runScript(0); }
void runScript1() { runScript(1); }
void runScript2() { runScript(2); }
void runScript3() { runScript(3); }
void runScript4() { runScript(4); }
void runScript5() { runScript(5); }
void runScript6() { runScript(6); }
void runScript7() { runScript(7); }
void runScript8() { runScript(8); }
void runScript9() { runScript(9); }
void runScript10() { runScript(10); }
void runScript11() { runScript(11); }
void runScript12() { runScript(12); }
void runScript13() { runScript(13); }
void runScript14() { runScript(14); }
void runScript15() { runScript(15); }
void runScript16() { runScript(16); }
void runScript17() { runScript(17); }
void runScript18() { runScript(18); }
void runScript19() { runScript(19); }
void runScript20() { runScript(20); }
void runScript21() { runScript(21); }
void runScript22() { runScript(22); }
void runScript23() { runScript(23); }
void runScript24() { runScript(24); }
void runScript25() { runScript(25); }
void runScript26() { runScript(26); }
void runScript27() { runScript(27); }
void runScript28() { runScript(28); }
void runScript29() { runScript(29); }
void runScript30() { runScript(30); }
void runScript31() { runScript(31); }
void runScript32() { runScript(32); }
void runScript33() { runScript(33); }
void runScript34() { runScript(34); }
void runScript35() { runScript(35); }
void runScript36() { runScript(36); }
void runScript37() { runScript(37); }
void runScript38() { runScript(38); }
void runScript39() { runScript(39); }
void runScript40() { runScript(40); }
void runScript41() { runScript(41); }
void runScript42() { runScript(42); }
void runScript43() { runScript(43); }
void runScript44() { runScript(44); }
void runScript45() { runScript(45); }
void runScript46() { runScript(46); }
void runScript47() { runScript(47); }
void runScript48() { runScript(48); }
void runScript49() { runScript(49); }

void (*g_runScriptFuncs[MAX_MENU_SCRIPTS])(void) = {
  runScript0,
  runScript1,
  runScript2,
  runScript3,
  runScript4,
  runScript5,
  runScript6,
  runScript7,
  runScript8,
  runScript9,
  runScript10,
  runScript11,
  runScript12,
  runScript13,
  runScript14,
  runScript15,
  runScript16,
  runScript17,
  runScript18,
  runScript19,
  runScript20,
  runScript21,
  runScript22,
  runScript23,
  runScript24,
  runScript25,
  runScript26,
  runScript27,
  runScript28,
  runScript29,
  runScript30,
  runScript31,
  runScript32,
  runScript33,
  runScript34,
  runScript35,
  runScript36,
  runScript37,
  runScript38,
  runScript39,
  runScript40,
  runScript41,
  runScript42,
  runScript43,
  runScript44,
  runScript45,
  runScript46,
  runScript47,
  runScript48,
  runScript49
};




HWND getCurrentHScintilla(int which);

// Main python handler/wrapper
PythonHandler *pythonHandler;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	//MessageBox(NULL, _T("DllMain"), _T("Python"), 0);
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




extern "C" __declspec(dllexport) void setInfo(NppData notepadPlusData)
{
	nppData = notepadPlusData;
	// Initialise the dialogs
	aboutDlg.init((HINSTANCE)g_hModule, nppData);
	g_console.init((HINSTANCE)g_hModule, nppData);
	
	// Get the two key directories (plugins config and the Npp dir)
	TCHAR temp[MAX_PATH];
	::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(temp));
	strcpy_s(g_configDir, MAX_PATH, WcharMbcsConverter::tchar2char(temp).get());

	::SendMessage(nppData._nppHandle, NPPM_GETNPPDIRECTORY, MAX_PATH, reinterpret_cast<LPARAM>(temp));
	strcpy_s(g_pluginDir, MAX_PATH, WcharMbcsConverter::tchar2char(temp).get());
	strcat_s(g_pluginDir, MAX_PATH, "\\plugins");

	loadSettings();

	g_infoSet = true;
}

extern "C" __declspec(dllexport) CONST TCHAR * getName()
{
	return PLUGIN_NAME;
}

extern "C" __declspec(dllexport) FuncItem * getFuncsArray(int *nbF)
{

	if (g_infoSet)
	{
		MessageBox(NULL, _T("Python GetFuncsArray"), _T("Python Script"), 0);
		funcItem = getGeneratedFuncItemArray(nbF);
	}
	else
	{
		MessageBox(NULL, _T("A fatal error has occurred. Notepad++ has incorrectly called getFuncsArray() before setInfo().  No menu items will be available for PythonScript."), _T("Python Script"), 0);
		funcItem = (FuncItem*) malloc(sizeof(FuncItem));
		memset(funcItem, 0, sizeof(FuncItem));
		_tcscpy_s(funcItem[0]._itemName, 64, _T("About"));
		funcItem[0]._pFunc = doAbout;
	}

	return funcItem;
}

HWND getCurrentHScintilla(int which)
{
	return (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;
};


FuncItem* getGeneratedFuncItemArray(int *nbF)
{
	string startupFilename(g_configDir);
	startupFilename.append("\\PythonScriptStartup.cnf");
	
	basic_ifstream<TCHAR> startupFile(startupFilename.c_str());
	
	TCHAR buffer[500];
	
	
	HICON defaultIcon = NULL;
	HICON hIcon;

	while (startupFile.good())
	{
		startupFile.getline(buffer, 500);
		TCHAR *context;
		TCHAR *element = _tcstok_s(buffer, _T("/"), &context);

		// Menu item
		if (0 == _tcscmp(element, _T("ITEM")))
		{
			element = _tcstok_s(NULL, _T("/"), &context);
			shared_ptr<char> charElement = (WcharMbcsConverter::tchar2char(element));
			g_menuScripts.push_back(new string(charElement.get()));
		}
		
		// Toolbar item
		else if (0 == _tcscmp(element, _T("TOOLBAR")))
		{
			element = _tcstok_s(NULL, _T("/"), &context);
			TCHAR *iconPath = _tcstok_s(NULL, _T("/"), &context);
			if (!iconPath)
			{
				// No icon specified, so use the default
				if (NULL == defaultIcon)
				{
					// First time we get no icon specified, load the default icon
					shared_ptr<TCHAR> tcharPluginDir = WcharMbcsConverter::char2tchar(g_pluginDir);
					tstring defaultIconPath(tcharPluginDir.get());
					defaultIconPath.append(_T("\\python.ico"));
					defaultIcon = static_cast<HICON>(LoadImage(NULL, iconPath, IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
				}
				hIcon = defaultIcon;
			}
			else 
			{
				hIcon = static_cast<HICON>(LoadImage(NULL, iconPath, IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
			}

			shared_ptr<char> charElement = (WcharMbcsConverter::tchar2char(element));
			g_toolbarScripts.push_back(new pair<string*, HICON>(new string(charElement.get()), hIcon));

		}

	}
	startupFile.close();

	
	

	// This is cached as it's used in the loop below
	int menuScriptsSize = g_menuScripts.size();

	// Remove one from the count of menu items if the list is empty
	// as we'll only have one separator
	*nbF = menuScriptsSize + FIXED_MENU_ITEMS - (g_menuScripts.empty() ? 1 : 0);

	FuncItem *items = (FuncItem*)malloc(sizeof(FuncItem) * (*nbF));

	_tcscpy_s(items[0]._itemName, 64, _T("New Script"));
	items[0]._init2Check = FALSE;
	items[0]._pShKey = NULL;
	items[0]._pFunc = newScript;

	_tcscpy_s(items[1]._itemName, 64, _T("Show Console"));
	items[1]._init2Check = FALSE;
	items[1]._pShKey = NULL;
	items[1]._pFunc = showConsole;
	
	
	_tcscpy_s(items[2]._itemName, 64, _T("--"));
	items[2]._init2Check = FALSE;
	items[2]._pShKey = NULL;
	items[2]._pFunc = NULL;

	const int startPos = 3;
	int currentPos;

	menuScriptsSize += startPos;

	for (currentPos = startPos; currentPos < menuScriptsSize; ++currentPos)
	{
		shared_ptr<TCHAR> tFilename = WcharMbcsConverter::char2tchar(g_menuScripts[currentPos - startPos]->c_str());
		::PathRemoveExtension(tFilename.get());
		_tcscpy_s(items[currentPos]._itemName, 64, PathFindFileName(tFilename.get()));
		items[currentPos]._init2Check = FALSE;
		items[currentPos]._pShKey = NULL;
		items[currentPos]._pFunc = g_runScriptFuncs[currentPos - startPos];
	}

	if (currentPos > startPos)
	{
		_tcscpy_s(items[currentPos]._itemName, 64, _T("--"));
		items[currentPos]._init2Check = FALSE;
		items[currentPos]._pShKey = NULL;
		items[currentPos]._pFunc = NULL;
		++currentPos;
	}

	_tcscpy_s(items[currentPos]._itemName, 64, _T("--"));
	items[currentPos]._init2Check = FALSE;
	items[currentPos]._pShKey = NULL;
	items[currentPos]._pFunc = NULL;
	++currentPos;

	_tcscpy_s(items[currentPos]._itemName, 64, _T("About"));
	items[currentPos]._init2Check = FALSE;
	items[currentPos]._pShKey = NULL;
	items[currentPos]._pFunc = doAbout;
	g_aboutFuncIndex = currentPos;

	return items;

}
	



void initialise()
{
	DWORD startTicks = GetTickCount();
	pythonHandler = new PythonHandler(g_pluginDir, g_configDir, nppData._nppHandle, nppData._scintillaMainHandle, nppData._scintillaSecondHandle, &g_console);
	
	pythonHandler->initPython();
	g_console.initPython(pythonHandler);

	pythonHandler->runStartupScripts();

	
	MenuManager* menuManager = MenuManager::create(nppData._nppHandle, funcItem[g_aboutFuncIndex]._cmdID, g_aboutFuncIndex, runScript);
	menuManager->populateScriptsMenu();

	DWORD endTicks = GetTickCount();
	char result[200];
	sprintf_s(result, 200, "Python initialisation took %ldms\nReady.\n", endTicks-startTicks);
	g_console.message(result);
}

extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode)
{
	/* This switch is split into two
	 * 1. Notifications that must be run BEFORE any registered Python callbacks, and
	 * 2. Notifications that must be run AFTER any registered Python callbacks
	 */

	switch(notifyCode->nmhdr.code)
	{
		case NPPN_READY:
			{
				MessageBox(NULL, _T("NPPN_READY"), _T("Python Script"), 0);
				
			
				initialise();
			}
			break;
	}

	// Notify the scripts
	if (pythonHandler)
		pythonHandler->notify(notifyCode);

	// Post notifications handlers
	switch(notifyCode->nmhdr.code)
	{
		case NPPN_SHUTDOWN:
			saveSettings();
			delete pythonHandler;
			pythonHandler = NULL;
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






void runScript(int number)
{
	runScript(g_menuScripts[number]->c_str());
}


void runScript(const char *filename)
{
	BYTE keyState[256];
	::GetKeyboardState(keyState);

	// If either control held down, then edit the file
	if ((keyState[VK_LCONTROL] & 0x80) || (keyState[VK_RCONTROL] & 0x80))
	{
		NotepadPlusWrapper wrapper(nppData._nppHandle);
		if (!wrapper.activateFile(filename))
		{
			wrapper.open(filename);
		}
	}
	else
	{
		
		if (!pythonHandler->runScript(filename, false))
		{
			MessageBox(NULL, _T("Cannot run a script when a script is already running"), _T("Python Script"), 0);
		}
	}
}

void showConsole()
{
	g_console.showDialog();
}

void newScript()
{
	NotepadPlusWrapper wrapper(nppData._nppHandle);

	wrapper.newDocument();
	wrapper.setLangType(L_PYTHON);

}





