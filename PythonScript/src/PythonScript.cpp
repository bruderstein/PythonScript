// Plugin Template
//

#include "stdafx.h"
#include "PluginInterface.h"
#include "AboutDialog.h"
#include "MenuManager.h"

#include "Scintilla.h"
#include "WcharMbcsConverter.h"
#include "PythonHandler.h"
#include "PythonConsole.h"
#include "ShortcutDlg.h"
#include "Notepad_plus_msgs.h"
#include "HelpController.h"
#include "PythonScript/NppPythonScript.h"

#define CHECK_INITIALISED()  if (!g_initialised) initialisePython()

/* Info for Notepad++ */
CONST TCHAR PLUGIN_NAME[]	= _T("Python Script");

FuncItem	*funcItem = NULL;

/* Global data */
NppData				nppData;
HANDLE				g_hModule			= NULL;
TCHAR				iniFilePath[MAX_PATH];

/* Dialogs */
AboutDialog		aboutDlg;
ShortcutDlg     *g_shortcutDlg = NULL;

PythonConsole   *g_console = 0;
// Paths
char g_pluginDir[MAX_PATH];
char g_configDir[MAX_PATH];
std::string g_previousScript;

bool g_infoSet = false;
int g_scriptsMenuIndex = 0;
int g_stopScriptIndex = 0;
bool g_initialised = false;
MenuManager *g_menuManager;

// Scripts on the menu
std::vector<std::string*> g_menuScripts;

// Scripts on the toolbar
std::vector< std::pair<std::string*, HICON>* > g_toolbarScripts;



void doAbout();

void newScript();
void showConsole();
void showShortcutDlg();
void stopScript();
void runScript(int);
void runScript(const char *script, bool synchronous, HANDLE completedEvent = NULL, bool allowQueuing = false);
void runStatement(const char *statement, bool synchronous, HANDLE completedEvent = NULL, bool allowQueuing = false);
void shutdown(void *);
void doHelp();
void previousScript();

FuncItem* getGeneratedFuncItemArray(int *nbF);



// Run script functions




HWND getCurrentHScintilla(int which);

// Main python handler/wrapper
PythonHandler *pythonHandler;

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID /* lpReserved */
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

	NO_DEFAULT_CASE;
	}
	return TRUE;
}




extern "C" __declspec(dllexport) void setInfo(NppData notepadPlusData)
{
	nppData = notepadPlusData;
#ifdef DEBUG_STARTUP
	MessageBox(NULL, _T("setInfo"), _T("Python Script"), 0);
#endif


	// Get the two key directories (plugins config and the Npp dir)
	TCHAR pluginConfig[MAX_PATH];
	::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(pluginConfig));
	strcpy_s(g_configDir, MAX_PATH, WcharMbcsConverter::tchar2char(pluginConfig).get());

	TCHAR pluginDir[MAX_PATH];
	::SendMessage(nppData._nppHandle, NPPM_GETNPPDIRECTORY, MAX_PATH, reinterpret_cast<LPARAM>(pluginDir));
	_tcscat_s(pluginDir, MAX_PATH, _T("\\plugins"));
	strcpy_s(g_pluginDir, MAX_PATH, WcharMbcsConverter::tchar2char(pluginDir).get());
	

	ConfigFile::create(pluginConfig, pluginDir, reinterpret_cast<HINSTANCE>(g_hModule));
	MenuManager::create(nppData._nppHandle, reinterpret_cast<HINSTANCE>(g_hModule), runScript);
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
#ifdef DEBUG_STARTUP
		MessageBox(NULL, _T("Python GetFuncsArray"), _T("Python Script"), 0);
#endif

		funcItem = getGeneratedFuncItemArray(nbF);
	}
	else
	{
		MessageBox(NULL, _T("A fatal error has occurred. Notepad++ has incorrectly called getFuncsArray() before setInfo().  No menu items will be available for PythonScript."), _T("Python Script"), 0);
		funcItem = (FuncItem*) malloc(sizeof(FuncItem));
		memset(funcItem, 0, sizeof(FuncItem));
		_tcscpy_s(funcItem[0]._itemName, 64, _T("About - Python Script Disabled"));
		funcItem[0]._pFunc = doAbout;
		*nbF = 1;
	}

	return funcItem;
}

HWND getCurrentHScintilla(int which)
{
	return (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;
};


FuncItem* getGeneratedFuncItemArray(int *nbF)
{
	
	MenuManager* menuManager = MenuManager::getInstance();
	
	MenuManager::ItemVectorTD items;
	items.reserve(8);
	int stopScriptIndex;
	int dynamicStartIndex;
	int scriptsMenuIndex;
	int runPreviousIndex;

	items.push_back(std::pair<tstring, void(*)()>(_T("New Script"), newScript));
	items.push_back(std::pair<tstring, void(*)()>(_T("Show Console"), showConsole));
	
	items.push_back(std::pair<tstring, void(*)()>(_T("--"), reinterpret_cast<void(*)()>(NULL)));
	
	items.push_back(std::pair<tstring, void(*)()>(_T("Stop Script"), stopScript));
	stopScriptIndex = items.size() - 1;

	items.push_back(std::pair<tstring, void(*)()>(_T("--"), reinterpret_cast<void(*)()>(NULL)));
	

	
	items.push_back(std::pair<tstring, void(*)()>(_T("Run Previous Script"), previousScript));
	runPreviousIndex = items.size() - 1;

	items.push_back(std::pair<tstring, void(*)()>(_T("--"), reinterpret_cast<void(*)()>(NULL)));
	scriptsMenuIndex = items.size() - 1;



	items.push_back(std::pair<tstring, void(*)()>(_T("Configuration"), showShortcutDlg));
	// Add dynamic scripts above the Configuration option - an extra separator will automatically
	// be added to the end of the list, if there are items in the dynamic menu
	dynamicStartIndex = items.size() - 1;

	items.push_back(std::pair<tstring, void(*)()>(_T("--"), reinterpret_cast<void(*)()>(NULL)));
	items.push_back(std::pair<tstring, void(*)()>(_T("Context-Help"), doHelp));
	items.push_back(std::pair<tstring, void(*)()>(_T("About"), doAbout));
	


	FuncItem* funcItems = menuManager->getFuncItemArray(nbF, items, runScript, dynamicStartIndex, scriptsMenuIndex, stopScriptIndex, runPreviousIndex);
	

	return funcItems;

}
	

void initialise()
{
	g_console = new PythonConsole(nppData._nppHandle);

	pythonHandler = new PythonHandler(g_pluginDir, g_configDir, (HINSTANCE)g_hModule, nppData._nppHandle, nppData._scintillaMainHandle, nppData._scintillaSecondHandle, g_console);
	
	aboutDlg.initDialog((HINSTANCE)g_hModule, nppData);
	
	g_shortcutDlg = new ShortcutDlg((HINSTANCE)g_hModule, nppData, _T("\\PythonScript\\scripts"));


	g_console->init((HINSTANCE)g_hModule, nppData);
	

	
	MenuManager* menuManager = MenuManager::getInstance();
	//menuManager->idsInitialised();
	menuManager->populateScriptsMenu();
	menuManager->stopScriptEnabled(false);
	menuManager->initPreviousScript();

	
	
}

void initialisePython()
{
	g_initialised = true;
	DWORD startTicks = GetTickCount();
	
	
	pythonHandler->initPython();
	
	g_console->initPython(pythonHandler);
	
	pythonHandler->runStartupScripts();

	
	DWORD endTicks = GetTickCount();
	g_console->message("Python ");
	g_console->message(Py_GetVersion());
	
	char result[200];
	
	sprintf_s(result, 200, "\nInitialisation took %ldms\nReady.\n", endTicks-startTicks);
	g_console->message(result);
	
}

void registerToolbarIcons()
{
#ifdef DEBUG_STARTUP
	MessageBox(NULL, _T("Register toolbar icons"), _T("Python Script"), 0); 
#endif
	MenuManager* menuManager = MenuManager::getInstance();
	menuManager->idsInitialised();		
	menuManager->configureToolbarIcons();
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
#ifdef DEBUG_STARTUP
				MessageBox(NULL, _T("NPPN_READY"), _T("Python Script"), 0);
#endif
				initialise();
				ConfigFile *config = ConfigFile::getInstance();
				if (config->getSetting(_T("STARTUP")) == _T("ATSTARTUP"))
				{
					initialisePython();
				}
			}
			break;

		case NPPN_FILESAVED:
			{
				TCHAR filename[MAX_PATH];
				::SendMessage(nppData._nppHandle, NPPM_GETFULLPATHFROMBUFFERID, notifyCode->nmhdr.idFrom, reinterpret_cast<LPARAM>(filename));
				ConfigFile *configFile = ConfigFile::getInstance();
				const tstring machineScripts = configFile->getMachineScriptsDir().c_str();
				const tstring userScripts = configFile->getUserScriptsDir().c_str();

				if (_tcsnicmp(filename, machineScripts.c_str(), machineScripts.size()) == 0
					|| _tcsnicmp(filename, userScripts.c_str(), userScripts.size()) == 0)
				{
					MenuManager::getInstance()->refreshScriptsMenu();
				}
			}
			break;

		case NPPN_TBMODIFICATION:
			registerToolbarIcons();
			break;

		case NPPN_SHORTCUTREMAPPED:
			{
				MenuManager *menuManager = MenuManager::getInstance();
				menuManager->updateShortcut(notifyCode->nmhdr.idFrom, reinterpret_cast<ShortcutKey*>(notifyCode->nmhdr.hwndFrom));
			}
			break;

		default:
			// Other messages can just be ignored
			break;

	}
	
	// Notify the scripts
	if (pythonHandler)
		pythonHandler->notify(notifyCode);

	// Post notifications handlers
	switch(notifyCode->nmhdr.code)
	{
		case NPPN_SHUTDOWN:
			{
				DWORD shutdownThreadID;
				CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)shutdown, NULL, NULL, &shutdownThreadID);
			}
			break;	
		default:
			// Ignore all other messages
			break;
	}

	
}



extern "C" __declspec(dllexport) LRESULT messageProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case NPPM_MSGTOPLUGIN:
			{

				CommunicationInfo* ci = reinterpret_cast<CommunicationInfo*>(lParam);
				switch(ci->internalMsg)
				{
					case PYSCR_EXECSCRIPT:
					case PYSCR_EXECSTATEMENT:
					{
						CHECK_INITIALISED();
						PythonScript_Exec* pse = reinterpret_cast<PythonScript_Exec*>(ci->info);
						if (pse->structVersion != 1)
						{
							return TRUE;
						}

						std::shared_ptr<char> script = WcharMbcsConverter::tchar2char(pse->script);

						bool synchronous = (pse->flags & PYSCRF_SYNC) == PYSCRF_SYNC;

						if (PYSCR_EXECSCRIPT == ci->internalMsg)
						{
							runScript(script.get(), synchronous, pse->completedEvent, true);
						}
						else
						{
							runStatement(script.get(), synchronous, pse->completedEvent, true);
						}

						pse->deliverySuccess = TRUE;

						return FALSE;
					}

					case PYSCR_SHOWCONSOLE:
						if (g_console)
						{
							g_console->showDialog();
						}
						return FALSE;

					default:
						// Other messages are unknown, hence ignore
						break;
				}
			}
			break;

		case WM_COMMAND:
			MenuManager::getInstance()->processWmCommand(wParam, lParam);
			break;

		default:
			// Other messages can just be ignored
			break;
		
	}
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





void stopScript()
{
	if (pythonHandler)
	{
		pythonHandler->stopScript();
	}
}


bool shortcutKeyHasCtrl(int number)
{
	bool retVal = false;
	int cmdID = MenuManager::getInstance()->getOriginalCommandID(number);
	if (cmdID)
	{
		ShortcutKey key;
		BOOL hasShortcut = ::SendMessage(nppData._nppHandle, NPPM_GETSHORTCUTBYCMDID, cmdID, reinterpret_cast<LPARAM>(&key));
		if (hasShortcut && 0 != key._key && key._isCtrl && !key._isAlt && !key._isShift)
		{
			retVal = true;
		}
	}
	return retVal;
}


void runScript(int number)
{
	/*  If the shortcut for the given script number does not have a control in it,
	 *  (or no shortcut key is assigned), then we can pretend the user clicked the menu option.
	 *  runScript() will then check if Ctrl is held down, if it is, it will edit the script.
	 *  Obviously if this menu item's key DOES have Ctrl in it, (and we're on N++ 5.8 or upwards) 
	 *  then we can't tell the difference (as we haven't subclassed N++).  
	 *  If that's the case, then ctrl-click won't work.  That's just tough, I think.
	 */
	if (!shortcutKeyHasCtrl(number))
	{
		MenuManager::s_menuItemClicked = true;
	}

	runScript(ConfigFile::getInstance()->getMenuScript(number).c_str(), false);
}




void runStatement(const char *statement, bool synchronous, HANDLE completedEvent /* = NULL */, bool allowQueuing /* = false */)
{
	CHECK_INITIALISED();
	MenuManager::getInstance()->stopScriptEnabled(true);
	if (!pythonHandler->runScript(statement, synchronous, allowQueuing, completedEvent, true))
	{
		MessageBox(NULL, _T("Another script is currently running.  Running two scripts at the same time could produce unpredicable results, and is therefore disabled."), _T("Python Script"), 0);
	}

}

void updatePreviousScript(const char *filename)
{
	if (g_previousScript == filename)
		return;

	g_previousScript = filename;

	MenuManager *menuManager = MenuManager::getInstance();
	menuManager->updatePreviousScript(filename);
}

void runScript(const char *filename, bool synchronous, HANDLE completedEvent /* = NULL */, bool allowQueuing /* = false */)
{
	
	BYTE keyState[256];
	
	// If the filename is empty, then just ignore it
	if (!filename || (*filename) == '\0')
	{
		// Reset the menuItemClicked, just in case (highly unlikely, but still)
		MenuManager::s_menuItemClicked = false;
		return;
	}

	::GetKeyboardState(keyState);
	
	// If a menu item was clicked (or assumed to be, see runScript(int))
	// and either control held down, and shift + alt are not, then edit the file
	if (MenuManager::s_menuItemClicked 
		&& (keyState[VK_CONTROL] & 0x80)
		&& ((keyState[VK_SHIFT] & 0x80) == 0)
		&& ((keyState[VK_MENU] & 0x80) == 0))
	{
		if (!SendMessage(nppData._nppHandle, NPPM_SWITCHTOFILE, 0, reinterpret_cast<LPARAM>(WcharMbcsConverter::char2tchar(filename).get())))
		{
			SendMessage(nppData._nppHandle, NPPM_DOOPEN, 0, reinterpret_cast<LPARAM>(WcharMbcsConverter::char2tchar(filename).get()));
		}
	}
	else
	{
		CHECK_INITIALISED();
		MenuManager::getInstance()->stopScriptEnabled(true);
		
		// TODO: Really need to not change this if it's a MSGTOPLUGIN run
		updatePreviousScript(filename);

		if (!pythonHandler->runScript(filename, synchronous, allowQueuing, completedEvent))
		{
			MessageBox(NULL, _T("Another script is currently running.  Running two scripts at the same time could produce unpredicable results, and is therefore disabled."), _T("Python Script"), 0);
		}
	}

	MenuManager::s_menuItemClicked = false;

}

void showShortcutDlg()
{
	if (g_shortcutDlg)
	{
		g_shortcutDlg->doDialog();
	}
}

void showConsole()
{
	if (g_console)
	{
		CHECK_INITIALISED();
		g_console->showDialog();
	}
}

void newScript()
{
	
	OPENFILENAMEA ofn;
	memset(&ofn, 0, sizeof(OPENFILENAMEA));

	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = nppData._nppHandle;
	std::shared_ptr<char> userScriptsDir = WcharMbcsConverter::tchar2char(ConfigFile::getInstance()->getUserScriptsDir().c_str());
	ofn.lpstrInitialDir = userScriptsDir.get();
	//ofn.lpstrFileTitle = "Choose filename for new script";
	ofn.lpstrFile = new char[MAX_PATH];
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "py";
	//lint -e840 Use of nul character in a string literal
	// This is how it's meant to be used.
	ofn.lpstrFilter = "Python Source Files (*.py)\0*.py\0All Files (*.*)\0*.*\0";
	//lint +e840
	ofn.nFilterIndex = 1;

	ofn.Flags = OFN_OVERWRITEPROMPT;
	

	if (GetSaveFileNameA(&ofn))
	{
		
		HANDLE hFile = CreateFileA(ofn.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		CloseHandle(hFile);
		SendMessage(nppData._nppHandle, NPPM_DOOPEN, 0, reinterpret_cast<LPARAM>(WcharMbcsConverter::char2tchar(ofn.lpstrFile).get()));
		int bufferID = SendMessage(nppData._nppHandle, NPPM_GETCURRENTBUFFERID, 0, 0);
		SendMessage(nppData._nppHandle, NPPM_SETBUFFERLANGTYPE, L_PYTHON, bufferID);
	}
	

	delete [] ofn.lpstrFile;


}



void shutdown(void* /* dummy */)
{
	if (pythonHandler)
	{
		delete pythonHandler;
		pythonHandler = NULL;
	}

	if (g_console)
	{
		delete g_console;
		g_console = NULL;
	}

	for(std::vector<std::string*>::iterator it = g_menuScripts.begin(); it != g_menuScripts.end(); ++it)
	{
		delete *it;
	}

	for(std::vector< std::pair<std::string*, HICON>* >::iterator it = g_toolbarScripts.begin(); it != g_toolbarScripts.end(); ++it)
	{
		// Delete the string
		delete (*(*it)).first;

		// Destroy the HICON
		DestroyIcon((*(*it)).second);

		// Delete the pair
		delete *it;
	}

	MenuManager::deleteInstance();
	
}


void doHelp()
{
	int which;
	
	SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, reinterpret_cast<LPARAM>(&which));
	
	HelpController help(nppData._nppHandle, which ? nppData._scintillaSecondHandle : nppData._scintillaMainHandle);
	help.callHelp();
}

void previousScript()
{
	runScript(g_previousScript.c_str(), false);
}
