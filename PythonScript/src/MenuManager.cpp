#include "stdafx.h"
#include "keys.h"
#include "MenuManager.h"
#include "Notepad_Plus_Msgs.h"
#include "WcharMbcsConverter.h"
#include "ConfigFile.h"
#include <PluginInterface.h>

using namespace std;

// Static instance
MenuManager* MenuManager::s_menuManager = NULL;

WNDPROC MenuManager::s_origWndProc;

int MenuManager::s_startCommandID;
int MenuManager::s_endCommandID;
int MenuManager::s_startFixedID;
int MenuManager::s_endFixedID;
int MenuManager::s_startDynamicEntryID;
int MenuManager::s_endDynamicEntryID;
int MenuManager::s_startToolbarID;
int MenuManager::s_endToolbarID;

bool MenuManager::s_menuItemClicked;

void (*MenuManager::s_runScript)(int);


MenuManager* MenuManager::create(HWND hNotepad, HINSTANCE hInst, void(*runScript)(const char *, bool, HANDLE, bool))
{
	if (NULL == s_menuManager)
	{
		s_menuManager = new MenuManager(hNotepad, hInst, runScript);
	}
	return s_menuManager;
}


MenuManager::~MenuManager()
{
	// Free the Scripts menu HMENUs
	for(map<string, HMENU>::iterator iter = m_submenus.begin(); iter != m_submenus.end(); ++iter)
	{
		DestroyMenu((*iter).second);
	}
}


MenuManager* MenuManager::getInstance()
{
	return s_menuManager;
}


MenuManager::MenuManager(HWND hNotepad, HINSTANCE hInst, void(*runScript)(const char *, bool, HANDLE, bool))
	:
	m_hInst (hInst),
	m_hNotepad (hNotepad),
	m_runScript (runScript),
	m_pythonPluginMenu (NULL),
	m_funcItems(NULL)
{
	s_startDynamicEntryID = 1;
	s_endDynamicEntryID = 0;
	s_startToolbarID = 1;
	s_endToolbarID = 0;

	m_runScriptFuncs[0] = runScript0;
	m_runScriptFuncs[1] = runScript1;
	m_runScriptFuncs[2] = runScript2;
	m_runScriptFuncs[3] = runScript3;
	m_runScriptFuncs[4] = runScript4;
	m_runScriptFuncs[5] = runScript5;
	m_runScriptFuncs[6] = runScript6;
	m_runScriptFuncs[7] = runScript7;
	m_runScriptFuncs[8] = runScript8;
	m_runScriptFuncs[9] = runScript9;
	m_runScriptFuncs[10] = runScript10;
	m_runScriptFuncs[11] = runScript11;
	m_runScriptFuncs[12] = runScript12;
	m_runScriptFuncs[13] = runScript13;
	m_runScriptFuncs[14] = runScript14;
	m_runScriptFuncs[15] = runScript15;
	m_runScriptFuncs[16] = runScript16;
	m_runScriptFuncs[17] = runScript17;
	m_runScriptFuncs[18] = runScript18;
	m_runScriptFuncs[19] = runScript19;
	m_runScriptFuncs[20] = runScript20;
	m_runScriptFuncs[21] = runScript21;
	m_runScriptFuncs[22] = runScript22;
	m_runScriptFuncs[23] = runScript23;
	m_runScriptFuncs[24] = runScript24;
	m_runScriptFuncs[25] = runScript25;
	m_runScriptFuncs[26] = runScript26;
	m_runScriptFuncs[27] = runScript27;
	m_runScriptFuncs[28] = runScript28;
	m_runScriptFuncs[29] = runScript29;
	m_runScriptFuncs[30] = runScript30;
	m_runScriptFuncs[31] = runScript31;
	m_runScriptFuncs[32] = runScript32;
	m_runScriptFuncs[33] = runScript33;
	m_runScriptFuncs[34] = runScript34;
	m_runScriptFuncs[35] = runScript35;
	m_runScriptFuncs[36] = runScript36;
	m_runScriptFuncs[37] = runScript37;
	m_runScriptFuncs[38] = runScript38;
	m_runScriptFuncs[39] = runScript39;
	m_runScriptFuncs[40] = runScript40;
	m_runScriptFuncs[41] = runScript41;
	m_runScriptFuncs[42] = runScript42;
	m_runScriptFuncs[43] = runScript43;
	m_runScriptFuncs[44] = runScript44;
	m_runScriptFuncs[45] = runScript45;
	m_runScriptFuncs[46] = runScript46;
	m_runScriptFuncs[47] = runScript47;
	m_runScriptFuncs[48] = runScript48;
	m_runScriptFuncs[49] = runScript49;
	



	m_keyMap.insert(KeyMapTD::value_type(VK_BACK, _T("Backspace")));
	m_keyMap.insert(KeyMapTD::value_type(VK_TAB, _T("Tab")));
	m_keyMap.insert(KeyMapTD::value_type(VK_RETURN, _T("Enter")));
	m_keyMap.insert(KeyMapTD::value_type(VK_ESCAPE, _T("Esc")));
	m_keyMap.insert(KeyMapTD::value_type(VK_SPACE, _T("Spacebar")));

	m_keyMap.insert(KeyMapTD::value_type(VK_PRIOR, _T("Page up")));
	m_keyMap.insert(KeyMapTD::value_type(VK_NEXT, _T("Page down")));
	m_keyMap.insert(KeyMapTD::value_type(VK_END, _T("End")));
	m_keyMap.insert(KeyMapTD::value_type(VK_HOME, _T("Home")));
	m_keyMap.insert(KeyMapTD::value_type(VK_LEFT, _T("Left")));
	m_keyMap.insert(KeyMapTD::value_type(VK_UP, _T("Up")));
	m_keyMap.insert(KeyMapTD::value_type(VK_RIGHT, _T("Right")));
	m_keyMap.insert(KeyMapTD::value_type(VK_DOWN, _T("Down")));

	m_keyMap.insert(KeyMapTD::value_type(VK_INSERT, _T("INS")));
	m_keyMap.insert(KeyMapTD::value_type(VK_DELETE, _T("DEL")));

	m_keyMap.insert(KeyMapTD::value_type(VK_0, _T("0")));
	m_keyMap.insert(KeyMapTD::value_type(VK_1, _T("1")));
	m_keyMap.insert(KeyMapTD::value_type(VK_2, _T("2")));
	m_keyMap.insert(KeyMapTD::value_type(VK_3, _T("3")));
	m_keyMap.insert(KeyMapTD::value_type(VK_4, _T("4")));
	m_keyMap.insert(KeyMapTD::value_type(VK_5, _T("5")));
	m_keyMap.insert(KeyMapTD::value_type(VK_6, _T("6")));
	m_keyMap.insert(KeyMapTD::value_type(VK_7, _T("7")));
	m_keyMap.insert(KeyMapTD::value_type(VK_8, _T("8")));
	m_keyMap.insert(KeyMapTD::value_type(VK_9, _T("9")));
	m_keyMap.insert(KeyMapTD::value_type(VK_A, _T("A")));
	m_keyMap.insert(KeyMapTD::value_type(VK_B, _T("B")));
	m_keyMap.insert(KeyMapTD::value_type(VK_C, _T("C")));
	m_keyMap.insert(KeyMapTD::value_type(VK_D, _T("D")));
	m_keyMap.insert(KeyMapTD::value_type(VK_E, _T("E")));
	m_keyMap.insert(KeyMapTD::value_type(VK_F, _T("F")));
	m_keyMap.insert(KeyMapTD::value_type(VK_G, _T("G")));
	m_keyMap.insert(KeyMapTD::value_type(VK_H, _T("H")));
	m_keyMap.insert(KeyMapTD::value_type(VK_I, _T("I")));
	m_keyMap.insert(KeyMapTD::value_type(VK_J, _T("J")));
	m_keyMap.insert(KeyMapTD::value_type(VK_K, _T("K")));
	m_keyMap.insert(KeyMapTD::value_type(VK_L, _T("L")));
	m_keyMap.insert(KeyMapTD::value_type(VK_M, _T("M")));
	m_keyMap.insert(KeyMapTD::value_type(VK_N, _T("N")));
	m_keyMap.insert(KeyMapTD::value_type(VK_O, _T("O")));
	m_keyMap.insert(KeyMapTD::value_type(VK_P, _T("P")));
	m_keyMap.insert(KeyMapTD::value_type(VK_Q, _T("Q")));
	m_keyMap.insert(KeyMapTD::value_type(VK_R, _T("R")));
	m_keyMap.insert(KeyMapTD::value_type(VK_S, _T("S")));
	m_keyMap.insert(KeyMapTD::value_type(VK_T, _T("T")));
	m_keyMap.insert(KeyMapTD::value_type(VK_U, _T("U")));
	m_keyMap.insert(KeyMapTD::value_type(VK_V, _T("V")));
	m_keyMap.insert(KeyMapTD::value_type(VK_W, _T("W")));
	m_keyMap.insert(KeyMapTD::value_type(VK_X, _T("X")));
	m_keyMap.insert(KeyMapTD::value_type(VK_Y, _T("Y")));
	m_keyMap.insert(KeyMapTD::value_type(VK_Z, _T("Z")));

	m_keyMap.insert(KeyMapTD::value_type(VK_NUMPAD0, _T("Numpad 0")));
	m_keyMap.insert(KeyMapTD::value_type(VK_NUMPAD1, _T("Numpad 1")));
	m_keyMap.insert(KeyMapTD::value_type(VK_NUMPAD2, _T("Numpad 2")));
	m_keyMap.insert(KeyMapTD::value_type(VK_NUMPAD3, _T("Numpad 3")));
	m_keyMap.insert(KeyMapTD::value_type(VK_NUMPAD4, _T("Numpad 4")));
	m_keyMap.insert(KeyMapTD::value_type(VK_NUMPAD5, _T("Numpad 5")));
	m_keyMap.insert(KeyMapTD::value_type(VK_NUMPAD6, _T("Numpad 6")));
	m_keyMap.insert(KeyMapTD::value_type(VK_NUMPAD7, _T("Numpad 7")));
	m_keyMap.insert(KeyMapTD::value_type(VK_NUMPAD8, _T("Numpad 8")));
	m_keyMap.insert(KeyMapTD::value_type(VK_NUMPAD9, _T("Numpad 9")));
	m_keyMap.insert(KeyMapTD::value_type(VK_MULTIPLY, _T("Num *")));
	m_keyMap.insert(KeyMapTD::value_type(VK_ADD, _T("Num +")));
	//m_keyMap.insert(KeyMapTD::value_type(VK_SEPARATOR, _T("Num Enter")));	//this one doesnt seem to work
	m_keyMap.insert(KeyMapTD::value_type(VK_SUBTRACT, _T("Num -")));
	m_keyMap.insert(KeyMapTD::value_type(VK_DECIMAL, _T("Num .")));
	m_keyMap.insert(KeyMapTD::value_type(VK_DIVIDE, _T("Num /")));
	m_keyMap.insert(KeyMapTD::value_type(VK_F1, _T("F1")));
	m_keyMap.insert(KeyMapTD::value_type(VK_F2, _T("F2")));
	m_keyMap.insert(KeyMapTD::value_type(VK_F3, _T("F3")));
	m_keyMap.insert(KeyMapTD::value_type(VK_F4, _T("F4")));
	m_keyMap.insert(KeyMapTD::value_type(VK_F5, _T("F5")));
	m_keyMap.insert(KeyMapTD::value_type(VK_F6, _T("F6")));
	m_keyMap.insert(KeyMapTD::value_type(VK_F7, _T("F7")));
	m_keyMap.insert(KeyMapTD::value_type(VK_F8, _T("F8")));
	m_keyMap.insert(KeyMapTD::value_type(VK_F9, _T("F9")));
	m_keyMap.insert(KeyMapTD::value_type(VK_F10, _T("F10")));
	m_keyMap.insert(KeyMapTD::value_type(VK_F11, _T("F11")));
	m_keyMap.insert(KeyMapTD::value_type(VK_F12, _T("F12")));

	m_keyMap.insert(KeyMapTD::value_type(VK_OEM_3, _T("~")));
	m_keyMap.insert(KeyMapTD::value_type(VK_OEM_MINUS, _T("-")));
	m_keyMap.insert(KeyMapTD::value_type(VK_OEM_PLUS, _T("=")));
	m_keyMap.insert(KeyMapTD::value_type(VK_OEM_4, _T("[")));
	m_keyMap.insert(KeyMapTD::value_type(VK_OEM_6, _T("]")));
	m_keyMap.insert(KeyMapTD::value_type(VK_OEM_1, _T(";")));
	m_keyMap.insert(KeyMapTD::value_type(VK_OEM_7, _T("'")));
	m_keyMap.insert(KeyMapTD::value_type(VK_OEM_5, _T("\\")));
	m_keyMap.insert(KeyMapTD::value_type(VK_OEM_COMMA, _T(",")));
	m_keyMap.insert(KeyMapTD::value_type(VK_OEM_PERIOD, _T(".")));
	m_keyMap.insert(KeyMapTD::value_type(VK_OEM_2, _T("/")));

	m_keyMap.insert(KeyMapTD::value_type(VK_OEM_102, _T("<>")));
}



/* This code was shamefully robbed from NppExec from Dovgan Vitaliy*/
HMENU MenuManager::getOurMenu()
{
	if (NULL == m_pythonPluginMenu)
	{
		HMENU hPluginMenu = (HMENU)::SendMessage(m_hNotepad, NPPM_GETMENUHANDLE, 0, 0);

		int iMenuItems = GetMenuItemCount(hPluginMenu);
		for ( int i = 0; i < iMenuItems; i++ )
		{
			HMENU hSubMenu = ::GetSubMenu(hPluginMenu, i);
			// does our About menu command exist here?
			if ( ::GetMenuState(hSubMenu, m_funcItems[0]._cmdID, MF_BYCOMMAND) != -1 )
			{
				// this is our "Python Script" sub-menu
				m_pythonPluginMenu = hSubMenu;
				break;
			}
		}
		
	}

	return m_pythonPluginMenu;
}

void MenuManager::stopScriptEnabled(bool enabled)
{
	HMENU pythonPluginMenu = getOurMenu();
	if (pythonPluginMenu)
	{
		::EnableMenuItem(pythonPluginMenu, m_stopScriptIndex, MF_BYPOSITION | (enabled ? MF_ENABLED : MF_GRAYED));
	}

}


bool MenuManager::populateScriptsMenu()
{
	m_pythonPluginMenu = getOurMenu();
	if (!m_pythonPluginMenu)
	{
		//g_console.message("Error: Unable to find Python Plugin Menu\n");
		return false;
	}
	else
	{
		
		m_hScriptsMenu = CreateMenu();
		//funcItem[g_aboutFuncIndex]._cmdID + 1000
		s_startCommandID = m_funcItems[0]._cmdID + ADD_CMD_ID;
		
		InsertMenu(m_pythonPluginMenu, m_scriptsMenuIndex, MF_BYPOSITION | MF_POPUP, reinterpret_cast<UINT_PTR>(m_hScriptsMenu), _T("Scripts"));
		m_submenus.insert(pair<string, HMENU>("\\", m_hScriptsMenu));
		
		TCHAR pluginDir[MAX_PATH];
		TCHAR configDir[MAX_PATH];
		::SendMessage(m_hNotepad, NPPM_GETNPPDIRECTORY, MAX_PATH, reinterpret_cast<LPARAM>(pluginDir));
		::SendMessage(m_hNotepad, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(configDir));
		shared_ptr<char> path = WcharMbcsConverter::tchar2char(pluginDir);
		m_machineScriptsPath = path.get();
		m_machineScriptsPath.append("\\plugins\\PythonScript\\scripts");
		
		path = WcharMbcsConverter::tchar2char(configDir);
		m_userScriptsPath = path.get();
		m_userScriptsPath.append("\\PythonScript\\scripts");

		
		// Assume here that the func items are assigned from start to finish
		s_startFixedID = m_funcItems[m_dynamicStartIndex]._cmdID;
		s_endFixedID = m_funcItems[m_funcItemCount - 1]._cmdID;	

		// Fill the actual menu
		refreshScriptsMenu();

		// Dynamic scripts will start at one lower index now we've inserted the Scripts submenu
		++m_dynamicStartIndex;

		subclassNotepadPlusPlus();
		
	}

	return true;
}

// Fills the Scripts menu
void MenuManager::refreshScriptsMenu()
{
	
	// Remove all the menu items (or submenus) from the scripts menu
	// Remove, not destroy, as we need to destroy all the sub-submenus too
	int menuCount = GetMenuItemCount(m_hScriptsMenu);

	for(int position = 0; position < menuCount; ++position)
	{
		RemoveMenu(m_hScriptsMenu, 0, MF_BYPOSITION);
	}


	m_scriptCommands.erase(m_scriptCommands.begin(), m_scriptCommands.end());
	m_machineScriptNames.erase(m_machineScriptNames.begin(), m_machineScriptNames.end());
	
	

	// Destroy all the menus we've created
	for(SubmenusTD::iterator it = m_submenus.begin(); it != m_submenus.end(); ++it)
	{
		if (it->first != "\\")
		{
			// Destroy the menu
			DestroyMenu(it->second);
		}
	}
	
	m_submenus.erase(m_submenus.begin(), m_submenus.end());

	int nextID = findScripts(m_hScriptsMenu, m_machineScriptsPath.size(), s_startCommandID, m_machineScriptsPath);

	s_endCommandID = findScripts(m_hScriptsMenu, m_userScriptsPath.size(), nextID, m_userScriptsPath);

	
	DrawMenuBar(m_hNotepad);
}


int MenuManager::findScripts(HMENU hBaseMenu, int basePathLength, int startID, string& path)
{
	WIN32_FIND_DATAA findData;
	string indexPath;
	string searchPath(path);
	searchPath.append("\\*");
	HANDLE hFound = FindFirstFileA(searchPath.c_str(), &findData);
	BOOL found = (hFound != INVALID_HANDLE_VALUE) ? TRUE : FALSE;
	int position = 0;
	

	while (found)
	{
		if (FILE_ATTRIBUTE_DIRECTORY == (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& strcmp(findData.cFileName, ".") 
			&& strcmp(findData.cFileName, ".."))
		{
			searchPath = path;
			searchPath.append("\\");
			searchPath.append(findData.cFileName);
			HMENU hSubmenu = CreateMenu();
			// Add the submenu handle and path to the map
			indexPath.assign(searchPath.substr(basePathLength));
			
			pair< map<string, HMENU>::iterator, bool> result = m_submenus.insert(pair<string, HMENU>(indexPath, hSubmenu));
			
			// If the path has already been added, use the original HMENU
			if (!result.second)
			{
				DestroyMenu(hSubmenu);
				hSubmenu = (*result.first).second;
			}

			int nextID = findScripts(hSubmenu, basePathLength, startID, searchPath);
			if (nextID > startID)
			{
				startID = nextID;
				// Insert the submenu if it's new
				if (result.second)
				{
					InsertMenuA(hBaseMenu, position, MF_BYCOMMAND | MF_POPUP, reinterpret_cast<UINT_PTR>(hSubmenu), findData.cFileName);
				}
				
			}
			else
			{
				DestroyMenu(hSubmenu);
			}
			++position;
		}

		found = FindNextFileA(hFound, &findData);
	}
	FindClose(hFound);


	searchPath = path;
	searchPath.append("\\*.py");
	hFound = FindFirstFileA(searchPath.c_str(), &findData);
	found = (hFound != INVALID_HANDLE_VALUE) ? TRUE : FALSE;
	
	
	while(found)
	{
		string fullFilename(path);
		fullFilename.append("\\");
		fullFilename.append(findData.cFileName);
		m_scriptCommands.insert(pair<int, string>(startID, fullFilename));
		
		
		string indexedName = fullFilename.substr(basePathLength);
		
		pair<set<string>::iterator, bool> indexResult = m_machineScriptNames.insert(indexedName);
		
		char displayName[MAX_PATH];
		strcpy_s(displayName, MAX_PATH, indexedName.c_str());
		char *filename = PathFindFileNameA(displayName);
		PathRemoveExtensionA(filename);
		
		// If script name is already in the index
		// then the script name is already shown, so append " (User)" to the display name
		// as the first one must be a machine script
		if (!indexResult.second)
		{
			string sFilename(filename);
			sFilename.append(" (User)");
			InsertMenuA(hBaseMenu, position, MF_BYCOMMAND | MF_STRING | MF_UNCHECKED, startID, sFilename.c_str());
		}
		else
		{
			InsertMenuA(hBaseMenu, position, MF_BYCOMMAND | MF_STRING | MF_UNCHECKED, startID, filename);
		}
		
		++position;
		++startID;

		found = FindNextFileA(hFound, &findData);
	}
	FindClose(hFound);

	return startID;

}

void MenuManager::menuCommand(int commandID)
{
	m_runScript(m_scriptCommands[commandID].c_str(), false, NULL, false);
}

void MenuManager::toolbarCommand(int commandID)
{
	m_runScript(m_toolbarCommands[commandID].c_str(), false, NULL, false);
}



LRESULT CALLBACK notepadWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_COMMAND == message)
	{
		if (LOWORD(wParam) >= MenuManager::s_startCommandID && LOWORD(wParam) < MenuManager::s_endCommandID && HIWORD(wParam) == 0)
		{
			MenuManager::s_menuItemClicked = true;
			MenuManager::getInstance()->menuCommand(LOWORD(wParam));
			return TRUE;
		}
		else if (LOWORD(wParam) >= MenuManager::s_startFixedID && LOWORD(wParam) < MenuManager::s_endFixedID && HIWORD(wParam) == 0)
		{
			MenuManager::s_menuItemClicked = true;
		}
		else if (LOWORD(wParam) >= MenuManager::s_startDynamicEntryID && LOWORD(wParam) < MenuManager::s_endDynamicEntryID && HIWORD(wParam) == 0)
		{
			MenuManager::s_menuItemClicked = true;
			MenuManager::getInstance()->menuCommand(LOWORD(wParam));
			return TRUE;
		}
		else if (LOWORD(wParam) >= MenuManager::s_startToolbarID && LOWORD(wParam) < MenuManager::s_endToolbarID && HIWORD(wParam) == 0)
		{
			MenuManager::s_menuItemClicked = true;
			MenuManager::getInstance()->toolbarCommand(LOWORD(wParam));
			return TRUE;
		}

	}
	
	return CallWindowProc(MenuManager::s_origWndProc, hWnd, message, wParam, lParam);
	
}

void MenuManager::subclassNotepadPlusPlus()
{
	s_origWndProc = reinterpret_cast<WNDPROC>(::SetWindowLongPtr(m_hNotepad, GWLP_WNDPROC, (LONG_PTR)(notepadWndProc)));
}




FuncItem* MenuManager::getFuncItemArray(int *nbF, ItemVectorTD items, void (*runScript)(int), int dynamicStartIndex, int scriptsMenuIndex, int stopScriptIndex, int runPreviousIndex) 
{
	s_runScript = runScript;

	
	ConfigFile* configFile = ConfigFile::getInstance();
	
	ConfigFile::MenuItemsTD menuItems = configFile->getMenuItems();
   


	
	// Remove one from the count of menu items if the list is empty
	// as we'll only have one separator
	*nbF =  menuItems.size() + items.size() + (menuItems.empty() ? 0 : 1);
	
	m_funcItemCount = *nbF;

	m_funcItems = new FuncItem[*nbF];

	// Add all the static items passed in
	int position = 0;
	
	for(ItemVectorTD::iterator it = items.begin(); it != items.end(); ++it)
	{
		if (position == dynamicStartIndex)
		{
			
			for (ConfigFile::MenuItemsTD::iterator iter = menuItems.begin(); iter != menuItems.end(); ++iter)
			{
				TCHAR filenameCopy[MAX_PATH];
				_tcscpy_s(filenameCopy, MAX_PATH, iter->c_str());
				::PathRemoveExtension(filenameCopy);
				_tcscpy_s(m_funcItems[position]._itemName, 64, PathFindFileName(filenameCopy));
				m_funcItems[position]._init2Check = FALSE;
				m_funcItems[position]._pShKey = NULL;
				m_funcItems[position]._pFunc = m_runScriptFuncs[position - dynamicStartIndex];
				++position;
			}

			// Add another separator if there were one or more dynamic items
			if (!menuItems.empty())
			{
				_tcscpy_s(m_funcItems[position]._itemName, 64, _T("--"));
				m_funcItems[position]._init2Check = FALSE;
				m_funcItems[position]._pShKey = NULL;
				m_funcItems[position]._pFunc = NULL;
				++position;
			}
	
		}

		_tcscpy_s(m_funcItems[position]._itemName, 64, it->first.c_str());
		m_funcItems[position]._init2Check = FALSE;
		m_funcItems[position]._pShKey = NULL;
		m_funcItems[position]._pFunc = it->second;
		++position;
	
		

		
	}

	
	m_dynamicStartIndex = dynamicStartIndex;
	m_dynamicCount = menuItems.size();
	m_originalDynamicCount = m_dynamicCount;
	m_scriptsMenuIndex = scriptsMenuIndex;
	m_stopScriptIndex = stopScriptIndex;
	m_runPreviousIndex = runPreviousIndex;

	return m_funcItems;

}

tstring MenuManager::getKeyName(ShortcutKey& sk)
{
	tstring key;
	if (sk._isCtrl)
		key.append(_T("Ctrl+"));
	if (sk._isAlt)
		key.append(_T("Alt+"));
	if (sk._isShift)
		key.append(_T("Shift+"));
	KeyMapTD::iterator foundKey = m_keyMap.find(sk._key);
	if (foundKey != m_keyMap.end())
		key.append(foundKey->second);
	else
		key.append(_T("[UnknownKey]"));

	return key;
}

// Reconfigure the dynamic menus from the config
void MenuManager::reconfigure()
{
	ConfigFile *configFile = ConfigFile::getInstance();
	ConfigFile::MenuItemsTD menuItems = configFile->getMenuItems();

	
	TCHAR buffer[MAX_PATH];
	TCHAR *filename;
	
	// Remove the current list of script commands
	m_scriptCommands.clear();

	HMENU hPluginMenu = getOurMenu();
	int dynamicEntryID = m_funcItems[0]._cmdID + DYNAMIC_ADD_ID;
	s_startDynamicEntryID = dynamicEntryID;
	// Remove the current "extra" entries - ie. entries after the original list in funcItems
	for(int position = m_originalDynamicCount; position < m_dynamicCount; ++position)
	{
		::DeleteMenu(hPluginMenu, m_dynamicStartIndex + position, MF_BYPOSITION);
	}
	
	int position = 0;

	for(ConfigFile::MenuItemsTD::iterator it = menuItems.begin(); it != menuItems.end(); ++it)
	{
		_tcscpy_s<MAX_PATH>(buffer, (*it).c_str());
		
		filename = PathFindFileName(buffer);
		PathRemoveExtension(filename);

		// If it's less than the original funcItem count given 
		// back from getFuncItems
		if (position < m_originalDynamicCount)
		{
			ShortcutKey sk;
			BOOL hasKey = ::SendMessage(m_hNotepad, NPPM_GETSHORTCUTBYCMDID, m_funcItems[m_dynamicStartIndex + position - 1]._cmdID, reinterpret_cast<LPARAM>(&sk));
			
			tstring menuTitle(filename);

			if (hasKey)
			{
				menuTitle.append(_T("\t"));
				menuTitle.append(getKeyName(sk));
			}

			// If we're currently passed the number of CURRENT 
			// dynamic entries, then we need to create the HMENU item again
			if (position >= m_dynamicCount)
			{
				// put a menu item back with the same ID
				// (N++ will believe this to be genuine :)

				// scripts sub menu didn't exist when dynamicStartIndex was set, hence the -1 
				::InsertMenu(hPluginMenu, position + m_dynamicStartIndex, MF_BYPOSITION, m_funcItems[m_dynamicStartIndex + position - 1]._cmdID, menuTitle.c_str());
			}
			else
			{
				// Update the existing menu
				// scripts sub menu didn't exist when dynamicStartIndex was set, hence the -1 
				::ModifyMenu(hPluginMenu, position + m_dynamicStartIndex, MF_BYPOSITION, m_funcItems[m_dynamicStartIndex + position - 1]._cmdID, menuTitle.c_str());
			}

			m_scriptCommands.insert(pair<int, string>(m_funcItems[position]._cmdID, string(WcharMbcsConverter::tchar2char(it->c_str()).get())));
		}
		else // position >= m_funcItemCount, so just add a new one
		{
			::InsertMenu(hPluginMenu, position + m_dynamicStartIndex, MF_BYPOSITION, dynamicEntryID, filename);	
			m_scriptCommands.insert(pair<int, string>(dynamicEntryID, string(WcharMbcsConverter::tchar2char(it->c_str()).get())));
			++dynamicEntryID;
		}

		++position;
	}

	// Delete the extra menus
	if (menuItems.size() < static_cast<size_t>(m_dynamicCount))
	{
		for(int currentCount = position; currentCount < m_dynamicCount; ++currentCount)
		{
			::DeleteMenu(hPluginMenu, position + m_dynamicStartIndex, MF_BYPOSITION);
		}
	}

	m_dynamicCount = menuItems.size();
	s_endDynamicEntryID = dynamicEntryID;
}


void MenuManager::configureToolbarIcons()
{
	ConfigFile *configFile = ConfigFile::getInstance();
	ConfigFile::ToolbarItemsTD toolbarItems = configFile->getToolbarItems();
	s_startToolbarID = m_funcItems[0]._cmdID + ADD_TOOLBAR_ID;
	int currentToolbarID = s_startToolbarID;
	toolbarIcons icons;

	for(ConfigFile::ToolbarItemsTD::iterator it = toolbarItems.begin(); it != toolbarItems.end(); ++it)
	{
		icons.hToolbarBmp = it->second.first;
		icons.hToolbarIcon = NULL;
		m_toolbarCommands.insert(pair<int, string>(currentToolbarID, WcharMbcsConverter::tchar2char(it->first.c_str()).get()));
		::SendMessage(m_hNotepad, NPPM_ADDTOOLBARICON, currentToolbarID, reinterpret_cast<LPARAM>(&icons));
		++currentToolbarID;
	}

	if (currentToolbarID > s_startToolbarID)
		s_endToolbarID = currentToolbarID;
}


void MenuManager::deleteInstance()
{
	if (s_menuManager)
	{
		delete s_menuManager;
		s_menuManager = NULL;
	}
}


int MenuManager::findPluginCommand(const TCHAR *pluginName, const TCHAR *menuOption)
{
	
	HMENU hPluginMenu = (HMENU)::SendMessage(m_hNotepad, NPPM_GETMENUHANDLE, 0, 0);
	
	int iMenuItems = GetMenuItemCount(hPluginMenu);
	TCHAR strBuffer[500];
		
	for ( int i = 0; i < iMenuItems; ++i )
	{
		
		::GetMenuString(hPluginMenu, i, strBuffer, 500, MF_BYPOSITION);
		if (0 == _tcsicmp(pluginName, strBuffer))
		{
			HMENU hSubMenu = ::GetSubMenu(hPluginMenu, i);

			int subMenuItems = ::GetMenuItemCount(hSubMenu);
			for (int subMenuPos = 0; subMenuPos < subMenuItems; ++subMenuPos)
			{
				
				TCHAR *context = NULL;;
				::GetMenuString(hSubMenu, subMenuPos, strBuffer, 500, MF_BYPOSITION);
				TCHAR *name = _tcstok_s(strBuffer, _T("\t"), &context);

				if (name && 0 == _tcsicmp(menuOption, name))
				{
					return ::GetMenuItemID(hSubMenu, subMenuPos);
				}
			}
			// We've found the plugin, but not the option, so no point continuing
			break;
		}

	}
		
	return 0;
}



int MenuManager::findMenuCommand(const TCHAR *menuName, const TCHAR *menuOption)
{
	HMENU hMenuBar = ::GetMenu(m_hNotepad);

	return findMenuCommand(hMenuBar, menuName, menuOption);
}


int MenuManager::findMenuCommand(HMENU hParentMenu, const TCHAR *menuName, const TCHAR *menuOption)
{
	int iMenuItems = GetMenuItemCount(hParentMenu);
	int retVal = 0;

	TCHAR strBuffer[500];
		
	for ( int i = 0; i < iMenuItems; ++i )
	{
		MENUITEMINFO mii;
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
		mii.cch = 500;
		mii.dwTypeData = strBuffer;

		::GetMenuItemInfo(hParentMenu, i, TRUE, &mii);

		if (NULL != mii.hSubMenu && 0 == _tcsicmp(menuName, strBuffer))
		{
			int subMenuItems = ::GetMenuItemCount(mii.hSubMenu);
			for (int subMenuPos = 0; subMenuPos < subMenuItems; ++subMenuPos)
			{		
				TCHAR *context = NULL;;
				::GetMenuString(mii.hSubMenu, subMenuPos, strBuffer, 500, MF_BYPOSITION);
				TCHAR *name = _tcstok_s(strBuffer, _T("\t"), &context);

				if (name && 0 == _tcsicmp(menuOption, name))
				{
					return ::GetMenuItemID(mii.hSubMenu, subMenuPos);
				}
			}
		}
		
		if (NULL != mii.hSubMenu)
		{
			retVal = findMenuCommand(mii.hSubMenu, menuName, menuOption);
			// If we've found it in the sub menu (or within the sub menu)
			if (0 != retVal)
				break;
		}

	}

	return retVal;

}

void MenuManager::initPreviousScript()
{
	ShortcutKey key;
	if (::SendMessage(m_hNotepad, NPPM_GETSHORTCUTBYCMDID, m_funcItems[m_runPreviousIndex]._cmdID, reinterpret_cast<LPARAM>(&key)))
	{
		m_runLastScriptShortcut = getKeyName(key);
	}
		
	::EnableMenuItem(getOurMenu(), m_runPreviousIndex, MF_GRAYED | MF_BYPOSITION);

}

void MenuManager::updateShortcut(UINT cmdID, ShortcutKey* key)
{
	if (cmdID == static_cast<UINT>(m_funcItems[m_runPreviousIndex]._cmdID))
	{
		if (key && key->_key != VK_NULL)
		{
			m_runLastScriptShortcut = getKeyName(*key);
		}
		else
		{
			m_runLastScriptShortcut.erase();
		}

		updatePreviousScript(m_previousRunFilename.c_str());
	}
}

void MenuManager::updatePreviousScript(const char *filename)
{
	char displayName[MAX_PATH];
	strcpy_s(displayName, MAX_PATH, PathFindFileNameA(filename));
	PathRemoveExtensionA(displayName);

	m_previousRunFilename = filename;
	
	tstring tdisplayName(_T("Run Previous Script ("));
	tdisplayName.append(WcharMbcsConverter::char2tchar(displayName).get());
	tdisplayName.append(_T(")"));

	if (!m_runLastScriptShortcut.empty())
	{
		tdisplayName.append(_T("\t"));
		tdisplayName.append(m_runLastScriptShortcut);
	}

	
	MENUITEMINFO mi;
	mi.cbSize = sizeof(MENUITEMINFO);
	mi.fMask = MIIM_STATE | MIIM_STRING;
	mi.fState = MFS_ENABLED;
	
	// string is not altered in SetMenuItemInfo() call, so we're safe to cast this
	mi.dwTypeData = const_cast<TCHAR*>(tdisplayName.c_str());

	SetMenuItemInfo(getOurMenu(), m_runPreviousIndex, TRUE, &mi);
	DrawMenuBar(m_hNotepad);
	
}