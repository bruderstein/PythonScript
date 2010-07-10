#include "stdafx.h"

#include "MenuManager.h"
#include "Notepad_Plus_Msgs.h"
#include "WcharMbcsConverter.h"
#include "ConfigFile.h"
#include <PluginInterface.h>

using namespace std;

// Static instance
MenuManager* MenuManager::s_menuManager;

WNDPROC MenuManager::s_origWndProc;

int MenuManager::s_startCommandID;
int MenuManager::s_endCommandID;

void (*MenuManager::s_runScript)(int);



MenuManager* MenuManager::create(HWND hNotepad, HINSTANCE hInst, void(*runScript)(const char *))
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


MenuManager::MenuManager(HWND hNotepad, HINSTANCE hInst, void(*runScript)(const char *))
	:
	m_hInst (hInst),
	m_hNotepad (hNotepad),
	m_runScript (runScript),
	m_pythonPluginMenu (NULL)
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
	
}



/* This code was shamefully robbed from NppExec from Dovgan Vitaliy*/
HMENU MenuManager::getOurMenu()
{
	
	HMENU hPluginMenu = (HMENU)::SendMessage(m_hNotepad, NPPM_GETMENUHANDLE, 0, 0);
	HMENU hPythonMenu = NULL;
	int iMenuItems = GetMenuItemCount(hPluginMenu);
	for ( int i = 0; i < iMenuItems; i++ )
	{
		HMENU hSubMenu = ::GetSubMenu(hPluginMenu, i);
		// does our About menu command exist here?
		if ( ::GetMenuState(hSubMenu, m_funcItems[0]._cmdID, MF_BYCOMMAND) != -1 )
		{
			// this is our "Python Script" sub-menu
			hPythonMenu = hSubMenu;
			break;
		}
	}


	return hPythonMenu;
}

void MenuManager::stopScriptEnabled(bool enabled)
{
	if (m_pythonPluginMenu)
	{
		::EnableMenuItem(m_pythonPluginMenu, m_stopScriptIndex, MF_BYPOSITION | (enabled ? MF_ENABLED : MF_DISABLED));
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
		
		HMENU hScriptsMenu = CreateMenu();
		//funcItem[g_aboutFuncIndex]._cmdID + 1000
		s_startCommandID = m_funcItems[0]._cmdID + ADD_CMD_ID;
		
		InsertMenu(m_pythonPluginMenu, m_scriptsMenuIndex, MF_BYPOSITION | MF_POPUP, reinterpret_cast<UINT_PTR>(hScriptsMenu), _T("Scripts"));
		m_submenus.insert(pair<string, HMENU>("\\", hScriptsMenu));
		
		TCHAR pluginDir[MAX_PATH];
		TCHAR configDir[MAX_PATH];
		::SendMessage(m_hNotepad, NPPM_GETNPPDIRECTORY, MAX_PATH, reinterpret_cast<LPARAM>(pluginDir));
		::SendMessage(m_hNotepad, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(configDir));
		shared_ptr<char> path = WcharMbcsConverter::tchar2char(pluginDir);
		string machineScriptsPath(path.get());
		machineScriptsPath.append("\\plugins\\PythonScript\\scripts");
		
		path = WcharMbcsConverter::tchar2char(configDir);
		string userScriptsPath(path.get());
		userScriptsPath.append("\\PythonScript\\scripts");

		int nextID = findScripts(hScriptsMenu, machineScriptsPath.size(), s_startCommandID, machineScriptsPath);

		s_endCommandID = findScripts(hScriptsMenu, userScriptsPath.size(), nextID, userScriptsPath);

		subclassNotepadPlusPlus();
		
	}

	return true;
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
	m_runScript(m_scriptCommands[commandID].c_str());
}




LRESULT CALLBACK notepadWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_COMMAND == message)
	{
		if (LOWORD(wParam) >= MenuManager::s_startCommandID && LOWORD(wParam) < MenuManager::s_endCommandID && HIWORD(wParam) == 0)
		{
			MenuManager::getInstance()->menuCommand(LOWORD(wParam));
			return TRUE;
		}
	}
	
	return CallWindowProc(MenuManager::s_origWndProc, hWnd, message, wParam, lParam);
	
}

void MenuManager::subclassNotepadPlusPlus()
{
	s_origWndProc = reinterpret_cast<WNDPROC>(::SetWindowLongPtr(m_hNotepad, GWLP_WNDPROC, (LONG_PTR)(notepadWndProc)));
}




FuncItem* MenuManager::getFuncItemArray(int *nbF, ItemVectorTD items, void (*runScript)(int), int dynamicStartIndex, int scriptsMenuIndex, int stopScriptIndex) 
{
	s_runScript = runScript;

	
	ConfigFile* configFile = ConfigFile::getInstance();
	
	ConfigFile::MenuItemsTD menuItems = configFile->getMenuItems();
   


	
	// Remove one from the count of menu items if the list is empty
	// as we'll only have one separator
	*nbF =  menuItems.size() + items.size() + (menuItems.empty() ? 0 : 1);

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

	
	
	m_scriptsMenuIndex = scriptsMenuIndex;
	m_stopScriptIndex = stopScriptIndex;
	
	return m_funcItems;

}