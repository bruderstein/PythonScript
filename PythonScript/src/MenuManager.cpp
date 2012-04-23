#include "stdafx.h"
#include "keys.h"
#include "MenuManager.h"
#include "WcharMbcsConverter.h"
#include "ConfigFile.h"
#include "PluginInterface.h"
#include "StaticIDAllocator.h"
#include "NppAllocator.h"
#include "Notepad_plus_msgs.h"
#include "DynamicIDManager.h"

// Static instance
MenuManager* MenuManager::s_menuManager = NULL;

WNDPROC MenuManager::s_origWndProc;


bool MenuManager::s_menuItemClicked;

MenuManager::runScriptIDFunc MenuManager::s_runScript;


MenuManager* MenuManager::create(HWND hNotepad, HINSTANCE hInst, runScriptFunc runScript)
{
	if (NULL == s_menuManager)
	{
		s_menuManager = new MenuManager(hNotepad, hInst, runScript);
	}
	return s_menuManager;
}


MenuManager::~MenuManager()
{
	try
	{
		// Free the Scripts menu HMENUs
		for(std::map<std::string, HMENU>::iterator iter = m_submenus.begin(); iter != m_submenus.end(); ++iter)
		{
			DestroyMenu((*iter).second);
		}

		if (m_originalDynamicMenuManager)
			delete m_originalDynamicMenuManager;

		if (m_dynamicMenuManager)
			delete m_dynamicMenuManager;

		if (m_toolbarMenuManager)
			delete m_toolbarMenuManager;

		if (m_scriptsMenuManager)
			delete m_scriptsMenuManager;

		if (m_idAllocator)
			delete m_idAllocator;
	}
	catch (...)
	{
		// I don't know what to do with that, but a destructor should never throw, so...
	}

	if (m_pythonPluginMenu)
	{
		::DestroyMenu(m_pythonPluginMenu);
		m_pythonPluginMenu = NULL;
	}

	if (m_hScriptsMenu)
	{
		::DestroyMenu(m_hScriptsMenu);
		m_hScriptsMenu = NULL;
	}

	if (m_funcItems)
	{
		delete [] m_funcItems;
		m_funcItems = NULL;
	}

	// To please Lint, let's NULL these handles and pointers
	m_hNotepad = NULL;
	m_hInst = NULL;
}

MenuManager* MenuManager::getInstance()
{
	return s_menuManager;
}

MenuManager::MenuManager(HWND hNotepad, HINSTANCE hInst, runScriptFunc runScript) :
	m_runScript (runScript),	
	m_hNotepad (hNotepad),	
	m_hInst (hInst),
	m_dynamicStartIndex(IDX_MAX),
	m_dynamicCount(0),
	m_originalDynamicCount(0),
	m_scriptsMenuIndex(IDX_MAX),
	m_stopScriptIndex(IDX_MAX),
	m_runPreviousIndex(IDX_MAX),
	m_originalLastCmdIndex(IDX_MAX),
	m_pythonPluginMenu (NULL),
	m_hScriptsMenu(NULL),
	m_funcItems(NULL),
	m_funcItemCount(0),
	m_idAllocator(NULL),
	m_dynamicMenuManager(NULL),
	m_originalDynamicMenuManager(NULL),
	m_scriptsMenuManager(NULL),
	m_toolbarMenuManager(NULL)
{

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
	assert(m_funcItems != NULL);
	if (m_funcItems && NULL == m_pythonPluginMenu)
	{
		HMENU hPluginMenu = (HMENU)::SendMessage(m_hNotepad, NPPM_GETMENUHANDLE, 0, 0);

		int iMenuItems = GetMenuItemCount(hPluginMenu);
		for ( int i = 0; i < iMenuItems; i++ )
		{
			HMENU hSubMenu = ::GetSubMenu(hPluginMenu, i);
			// does our About menu command exist here?
			// Thanks MS for having a function that returns a UNIT potentially returning -1  >:-(
			// How that makes sense is beyond me.
			//lint -e{650,737}
			if ( ::GetMenuState(hSubMenu, (UINT)m_funcItems[0]._cmdID, MF_BYCOMMAND) != -1 )
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

		
		InsertMenu(m_pythonPluginMenu, m_scriptsMenuIndex, MF_BYPOSITION | MF_POPUP, reinterpret_cast<UINT_PTR>(m_hScriptsMenu), _T("Scripts"));
		m_submenus.insert(std::pair<std::string, HMENU>("\\", m_hScriptsMenu));
		
		TCHAR pluginDir[MAX_PATH];
		TCHAR configDir[MAX_PATH];
		::SendMessage(m_hNotepad, NPPM_GETNPPDIRECTORY, MAX_PATH, reinterpret_cast<LPARAM>(pluginDir));
		::SendMessage(m_hNotepad, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(configDir));
		std::shared_ptr<char> path = WcharMbcsConverter::tchar2char(pluginDir);
		m_machineScriptsPath = path.get();
		m_machineScriptsPath.append("\\plugins\\PythonScript\\scripts");
		
		path = WcharMbcsConverter::tchar2char(configDir);
		m_userScriptsPath = path.get();
		m_userScriptsPath.append("\\PythonScript\\scripts");

		// Fill the actual menu
		refreshScriptsMenu();

		// Dynamic scripts will start at one lower index now we've inserted the Scripts submenu
		++m_dynamicStartIndex;

		
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
	
	assert(m_scriptsMenuManager != NULL);
	if (m_scriptsMenuManager)
	{
		m_scriptsMenuManager->begin();
	}

	findScripts(m_hScriptsMenu, m_machineScriptsPath.size(), m_machineScriptsPath);

	findScripts(m_hScriptsMenu, m_userScriptsPath.size(), m_userScriptsPath);

	
	DrawMenuBar(m_hNotepad);
}


idx_t MenuManager::getOriginalCommandID(idx_t scriptNumber)
{
	assert(m_funcItems != NULL);
	idx_t retVal = 0;
	if (m_funcItems && ((scriptNumber < m_originalDynamicCount) && (scriptNumber < m_dynamicCount)))
	{
		retVal = (idx_t)m_funcItems[m_dynamicStartIndex + scriptNumber - 1]._cmdID;
	}
	
	return retVal;
}

bool MenuManager::findScripts(HMENU hBaseMenu, size_t basePathLength, std::string& path)
{
	assert(m_scriptsMenuManager != NULL);
	if (!m_scriptsMenuManager)
	{
		return false;
	}

	WIN32_FIND_DATAA findData;
	std::string indexPath;
	std::string searchPath(path);
	searchPath.append("\\*");
	HANDLE hFound = FindFirstFileA(searchPath.c_str(), &findData);
	BOOL found = (hFound != INVALID_HANDLE_VALUE) ? TRUE : FALSE;
	idx_t position = 0;

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
			
			std::pair< std::map<std::string, HMENU>::iterator, bool> result = m_submenus.insert(std::pair<std::string, HMENU>(indexPath, hSubmenu));
			
			// If the path has already been added, use the original HMENU
			if (!result.second)
			{
				DestroyMenu(hSubmenu);
				hSubmenu = (*result.first).second;
			}

			bool subDirScriptsFound = findScripts(hSubmenu, basePathLength, searchPath);
			if (subDirScriptsFound )
			{
				// startID = nextID;
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
	
	bool scriptsFound = (TRUE == found);

	while(found)
	{
		std::string fullFilename(path);
		fullFilename.append("\\");
		fullFilename.append(findData.cFileName);
		m_scriptCommands.insert(std::pair<int, std::string>(m_scriptsMenuManager->currentID(), fullFilename));
		
		
		std::string indexedName = fullFilename.substr(basePathLength);
		
		std::pair<std::set<std::string>::iterator, bool> indexResult = m_machineScriptNames.insert(indexedName);
		
		char displayName[MAX_PATH];
		strcpy_s(displayName, MAX_PATH, indexedName.c_str());
		char *filename = PathFindFileNameA(displayName);
		PathRemoveExtensionA(filename);
		
		// If script name is already in the index
		// then the script name is already shown, so append " (User)" to the display name
		// as the first one must be a machine script
		if (!indexResult.second)
		{
			std::string sFilename(filename);
			sFilename.append(" (User)");
			InsertMenuA(hBaseMenu, position, MF_BYCOMMAND | MF_STRING | MF_UNCHECKED, m_scriptsMenuManager->currentID(), sFilename.c_str());
		}
		else
		{
			InsertMenuA(hBaseMenu, position, MF_BYCOMMAND | MF_STRING | MF_UNCHECKED, m_scriptsMenuManager->currentID(), filename);
		}
		
		++position;

		++(*m_scriptsMenuManager);

		found = FindNextFileA(hFound, &findData);
	}
	FindClose(hFound);

	return scriptsFound;

}

void MenuManager::menuCommand(idx_t commandID)
{
	// Try the menu commands list first, then the script commands
	ScriptCommandsTD::iterator it = m_menuCommands.find(commandID);
	if (it != m_menuCommands.end())
	{
		m_runScript(it->second.c_str(), false, NULL, false);
	}
	else
	{
		it = m_scriptCommands.find(commandID);
		if (it != m_scriptCommands.end())
		{
			m_runScript(it->second.c_str(), false, NULL, false);
		}
	}

}

void MenuManager::toolbarCommand(idx_t commandID)
{
	m_runScript(m_toolbarCommands[commandID].c_str(), false, NULL, false);
}



static LRESULT CALLBACK notepadWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_COMMAND == message)
	{
		MenuManager* menuManager = MenuManager::getInstance();
		if (TRUE == menuManager->processWmCommand(wParam, lParam))
		{
			return TRUE;
		}

	}
	
	return CallWindowProc(MenuManager::s_origWndProc, hWnd, message, wParam, lParam);
	
}

BOOL MenuManager::processWmCommand(WPARAM wParam, LPARAM /* lParam */)
{
	if (inFixedRange(LOWORD(wParam)))
	{
		MenuManager::s_menuItemClicked = true;
	}
	else if (inDynamicRange(LOWORD(wParam)))
	{
		MenuManager::s_menuItemClicked = true;
		menuCommand(LOWORD(wParam));
		return TRUE;
	}
	else if (inToolbarRange(LOWORD(wParam)))
	{
		MenuManager::s_menuItemClicked = true;
		toolbarCommand(LOWORD(wParam));
		return TRUE;
	}

	return FALSE;
}


void MenuManager::subclassNotepadPlusPlus()
{
	s_origWndProc = reinterpret_cast<WNDPROC>(::SetWindowLongPtr(m_hNotepad, GWLP_WNDPROC, (LONG_PTR)(notepadWndProc)));
}




FuncItem* MenuManager::getFuncItemArray(int *nbF, ItemVectorTD items, runScriptIDFunc runScript, idx_t dynamicStartIndex, idx_t scriptsMenuIndex, idx_t stopScriptIndex, idx_t runPreviousIndex) 
{
	s_runScript = runScript;
	
	
	ConfigFile* configFile = ConfigFile::getInstance();
	
	ConfigFile::MenuItemsTD menuItems = configFile->getMenuItems();
	


	
	// Remove one from the count of menu items if the list is empty
	// as we'll only have one separator
	*nbF =  (int)(menuItems.size() + items.size() + (menuItems.empty() ? 0 : 1));
	
	m_funcItemCount = (size_t)*nbF;

	// WARNING: If getFuncItemArray is called twice, we'll leak memory!
	m_funcItems = new FuncItem[m_funcItemCount];

	// Add all the static items passed in
	idx_t position = 0;
	
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
				m_funcItems[position]._init2Check = false;
				m_funcItems[position]._pShKey = NULL;
				m_funcItems[position]._pFunc = m_runScriptFuncs[position - dynamicStartIndex];
				++position;
			}

			// Add another separator if there were one or more dynamic items
			if (!menuItems.empty())
			{
				_tcscpy_s(m_funcItems[position]._itemName, 64, _T("--"));
				m_funcItems[position]._init2Check = false;
				m_funcItems[position]._pShKey = NULL;
				m_funcItems[position]._pFunc = NULL;
				++position;
			}
	
		}

		_tcscpy_s(m_funcItems[position]._itemName, 64, it->first.c_str());
		m_funcItems[position]._init2Check = false;
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
	m_originalLastCmdIndex = (size_t)*nbF - 1;
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
	m_menuCommands.clear();

	HMENU hPluginMenu = getOurMenu();
	
	// Ensure we have enough "extra" dynamic IDs
	m_dynamicMenuManager->reserve(menuItems.size());
	
	m_dynamicMenuManager->begin();

	// Remove the current "extra" entries - ie. entries after the original list in funcItems
	for(idx_t menuPosition = m_originalDynamicCount; menuPosition < m_dynamicCount; ++menuPosition)
	{
		::DeleteMenu(hPluginMenu, m_dynamicStartIndex + m_originalDynamicCount, MF_BYPOSITION);
	}
	
	idx_t position = 0;

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
			BOOL hasKey = ::SendMessage(m_hNotepad, NPPM_GETSHORTCUTBYCMDID, static_cast<WPARAM>(m_funcItems[m_dynamicStartIndex + position - 1]._cmdID), reinterpret_cast<LPARAM>(&sk));
			
			tstring menuTitle(filename);

			if (hasKey)
			{
				menuTitle.append(_T("\t"));
				menuTitle.append(getKeyName(sk));
			}

			
			if (position >= m_dynamicCount)
			{
				// Using m_dynamicMenuManager->currentID() will either give us the next valid 
				// ID that N++ originally assigned, or one that we've allocated after we ran out.
				// That means that shortcuts can be assigned for all scripts in the menu, 
				// up to the number of menu-scripts that were there when n++ originally started up.

				::InsertMenu(hPluginMenu, position + m_dynamicStartIndex, MF_BYPOSITION, m_dynamicMenuManager->currentID(), menuTitle.c_str());
			}
			else
			{
				// Update the existing menu
				// scripts sub menu didn't exist when dynamicStartIndex was set, hence the -1 
				
				::ModifyMenu(hPluginMenu, position + m_dynamicStartIndex, MF_BYPOSITION, m_dynamicMenuManager->currentID(), menuTitle.c_str());
			}

			m_menuCommands.insert(std::pair<int, std::string>(m_dynamicMenuManager->currentID(), std::string(WcharMbcsConverter::tchar2char(it->c_str()).get())));
		}
		else // position >= m_funcItemCount, so just add a new one
		{
			::InsertMenu(hPluginMenu, position + m_dynamicStartIndex, MF_BYPOSITION, m_dynamicMenuManager->currentID(), filename);	
			m_menuCommands.insert(std::pair<int, std::string>(m_dynamicMenuManager->currentID(), std::string(WcharMbcsConverter::tchar2char(it->c_str()).get())));
			
		}
		
		++(*m_dynamicMenuManager);

		++position;
	}

	// Delete any extra menus where we now have less menus than 
	//  a) we had last time, and 
	//  b) we had originally
	if (menuItems.size() < static_cast<size_t>(m_dynamicCount) && menuItems.size() < static_cast<size_t>(m_originalDynamicCount))
	{
		for(idx_t currentIdx = position; currentIdx < m_dynamicCount && currentIdx < m_originalDynamicCount; ++currentIdx)
		{
			::DeleteMenu(hPluginMenu, position + m_dynamicStartIndex, MF_BYPOSITION);
		}
	}

	m_dynamicCount = menuItems.size();

}


void MenuManager::configureToolbarIcons()
{
	assert(m_toolbarMenuManager != NULL);
	if (!m_toolbarMenuManager)
	{
		return;
	}

	ConfigFile *configFile = ConfigFile::getInstance();
	ConfigFile::ToolbarItemsTD toolbarItems = configFile->getToolbarItems();
	// s_startToolbarID = m_funcItems[0]._cmdID + ADD_TOOLBAR_ID;
	m_toolbarMenuManager->reserve(toolbarItems.size());
	m_toolbarMenuManager->begin();
	toolbarIcons icons;
	
	for(ConfigFile::ToolbarItemsTD::iterator it = toolbarItems.begin(); it != toolbarItems.end(); ++it)
	{
		icons.hToolbarBmp = it->second.first;
		icons.hToolbarIcon = NULL;
		m_toolbarCommands.insert(std::pair<int, std::string>(m_toolbarMenuManager->currentID(), WcharMbcsConverter::tchar2char(it->first.c_str()).get()));
		::SendMessage(m_hNotepad, NPPM_ADDTOOLBARICON, m_toolbarMenuManager->currentID(), reinterpret_cast<LPARAM>(&icons));
		++(*m_toolbarMenuManager);
	}

}


void MenuManager::deleteInstance()
{
	if (s_menuManager)
	{
		delete s_menuManager;
		s_menuManager = NULL;
	}
}


idx_t MenuManager::findPluginCommand(const TCHAR *pluginName, const TCHAR *menuOption, bool refreshCache)
{
	idx_t retVal = 0;
	bool fromCache = false;
	if (!refreshCache)
	{
		MenuCommandCacheTD::iterator it = m_pluginCommandCache.find(std::pair<tstring, tstring>(tstring(pluginName), tstring(menuOption)));
		if (it != m_pluginCommandCache.end())
		{
			retVal = it->second;
			fromCache = true;
		}
	}

	if (0 == retVal)
	{
		HMENU hPluginMenu = (HMENU)::SendMessage(m_hNotepad, NPPM_GETMENUHANDLE, 0, 0);
	
		size_t iMenuItems = (size_t)GetMenuItemCount(hPluginMenu);
		TCHAR strBuffer[500];
		for ( idx_t i = 0; i < iMenuItems; ++i )
		{
			MENUITEMINFO mii;
			mii.cbSize = sizeof(MENUITEMINFO);
			mii.fMask = MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
			mii.cch = 500;
			mii.dwTypeData = strBuffer;

			::GetMenuItemInfo(hPluginMenu, i, TRUE, &mii);
			tstring thisMenuName = formatMenuName(strBuffer);
			if (NULL != mii.hSubMenu && 0 == _tcsicmp(pluginName, thisMenuName.c_str()))
			{
				retVal = findMenuCommand(mii.hSubMenu, NULL, menuOption);
				break;
			}
		
		}
	}

	if (!fromCache && retVal != 0)
	{
		m_pluginCommandCache[std::pair<tstring,tstring>(tstring(pluginName), tstring(menuOption))] = retVal;
	}

	return retVal;
	/*
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
	*/
}



idx_t MenuManager::findMenuCommand(const TCHAR *menuName, const TCHAR *menuOption, bool refreshCache)
{
	if (!refreshCache)
	{
		MenuCommandCacheTD::iterator it = m_menuCommandCache.find(std::pair<tstring, tstring>(tstring(menuName), tstring(menuOption)));
		if (it != m_menuCommandCache.end())
		{
			return it->second;
		}
	}
	
	HMENU hMenuBar = ::GetMenu(m_hNotepad);
	idx_t retVal = findMenuCommand(hMenuBar, menuName, menuOption);
	
	if (retVal != 0)
	{
		m_menuCommandCache[std::pair<tstring,tstring>(tstring(menuName), tstring(menuOption))] = retVal;
	}

	return retVal;
}

tstring MenuManager::formatMenuName(const TCHAR *name)
{
	tstring nameStr(name);
	size_t pos = nameStr.find(_T('&'));
	if (pos != tstring::npos && pos < nameStr.size() - 1 && nameStr[pos + 1] != _T('&'))
	{
		nameStr.erase(pos, 1);
	}
	return nameStr;
}

idx_t MenuManager::findMenuCommand(HMENU hParentMenu, const TCHAR *menuName, const TCHAR *menuOption)
{
	size_t iMenuItems = (size_t)GetMenuItemCount(hParentMenu);
	idx_t retVal = 0;

	TCHAR strBuffer[500];
		
	for ( idx_t i = 0; i < iMenuItems; ++i )
	{
		MENUITEMINFO mii;
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
		mii.cch = 500;
		mii.dwTypeData = strBuffer;

		::GetMenuItemInfo(hParentMenu, i, TRUE, &mii);
		
		if (NULL != mii.hSubMenu)
		{
			tstring thisMenuName = formatMenuName(strBuffer);
			if (NULL == menuName || 0 == _tcsicmp(menuName, thisMenuName.c_str()))
			{
				size_t subMenuItems = (size_t)GetMenuItemCount(mii.hSubMenu);
				for (idx_t subMenuPos = 0; subMenuPos < subMenuItems; ++subMenuPos)
				{		
					TCHAR *context = NULL;
					::GetMenuString(mii.hSubMenu, subMenuPos, strBuffer, 500, MF_BYPOSITION);
					TCHAR *name = _tcstok_s(strBuffer, _T("\t"), &context);
					if (name)
					{
						tstring nameStr = formatMenuName(name);
						
						if (0 == _tcsicmp(menuOption, nameStr.c_str()))
						{
							return ::GetMenuItemID(mii.hSubMenu, (int)subMenuPos);
						}
					}
				}
			}
		}
		else
		{
			TCHAR *context = NULL;
			TCHAR *name = _tcstok_s(strBuffer, _T("\t"), &context);
			if (name)
			{
				tstring nameStr = formatMenuName(name);
						
				if (0 == _tcsicmp(menuOption, nameStr.c_str()))
				{
					return mii.wID;
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
	assert(m_funcItems != NULL);
	ShortcutKey key;
	if (m_funcItems && ::SendMessage(m_hNotepad, NPPM_GETSHORTCUTBYCMDID, static_cast<WPARAM>(m_funcItems[m_runPreviousIndex]._cmdID), reinterpret_cast<LPARAM>(&key)))
	{
		m_runLastScriptShortcut = getKeyName(key);
	}
		
	::EnableMenuItem(getOurMenu(), m_runPreviousIndex, MF_GRAYED | MF_BYPOSITION);

}

void MenuManager::updateShortcut(UINT cmdID, ShortcutKey* key)
{
	assert(m_funcItems != NULL);
	if (m_funcItems && cmdID == static_cast<UINT>(m_funcItems[m_runPreviousIndex]._cmdID))
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


void MenuManager::idsInitialised()
{
	assert(m_funcItems != NULL);
	if (!m_funcItems)
	{
		return;
	}

	if (::SendMessage(m_hNotepad, NPPM_ALLOCATESUPPORTED, 0, 0) == TRUE)
	{
		m_idAllocator = new NppAllocator(m_hNotepad);
	}
	else
	{
		idx_t startID = (idx_t)m_funcItems[m_dynamicStartIndex]._cmdID;
		m_idAllocator = new StaticIDAllocator(startID + DYNAMIC_ADD_ID, startID + DYNAMIC_ADD_ID + 750);
		subclassNotepadPlusPlus();
	}

	assert(m_dynamicStartIndex != IDX_MAX);
	
	m_dynamicMenuManager = new DynamicIDManager(m_idAllocator);
	m_originalDynamicMenuManager = new DynamicIDManager(m_idAllocator);

	m_scriptsMenuManager = new DynamicIDManager(m_idAllocator);
	m_toolbarMenuManager = new DynamicIDManager(m_idAllocator);

	idx_t lastID = (idx_t)m_funcItems[m_dynamicStartIndex]._cmdID;
	idx_t startBlock = lastID;
	// Check that the IDs are consecutive
	for(idx_t i = m_dynamicStartIndex + 1; i < (m_dynamicStartIndex + m_originalDynamicCount); ++i)
	{
		if ((idx_t)m_funcItems[i]._cmdID != lastID + 1)
		{
			m_originalDynamicMenuManager->addBlock(startBlock, (lastID - startBlock) + 1);
			m_dynamicMenuManager->addBlock(startBlock, (lastID - startBlock) + 1);
			startBlock = (idx_t)m_funcItems[i]._cmdID;
		}
		lastID = (idx_t)m_funcItems[i]._cmdID;
	}
	
	if (startBlock != (idx_t)m_funcItems[(m_dynamicStartIndex + m_originalDynamicCount) - 1]._cmdID)
	{
		m_originalDynamicMenuManager->addBlock(startBlock, (lastID - startBlock) + 1);
		m_dynamicMenuManager->addBlock(startBlock, (lastID - startBlock) + 1);
	}
}

bool MenuManager::inToolbarRange(idx_t commandID)
{
	assert(m_toolbarMenuManager != NULL);
	return m_toolbarMenuManager && m_toolbarMenuManager->inRange(commandID);
}

bool MenuManager::inDynamicRange(idx_t commandID)
{
	assert(m_dynamicMenuManager != NULL);
	assert(m_scriptsMenuManager != NULL);
	return ((m_dynamicMenuManager && m_dynamicMenuManager->inRange(commandID)) ||
			(m_scriptsMenuManager && m_scriptsMenuManager->inRange(commandID)));
}

bool MenuManager::inFixedRange(idx_t commandID)
{
	assert(m_originalDynamicMenuManager != NULL);
	return m_originalDynamicMenuManager && m_originalDynamicMenuManager->inRange(commandID);
}
