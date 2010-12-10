#ifndef _MENUMANAGER_H
#define _MENUMANAGER_H

#ifndef _PYTHONSCRIPT_H
#include "PythonScript.h"
#endif

struct ShortcutKey;

// The DYNAMIC_ADD_ID is used for the CommandIDs for dynamic menu entries added between restarts
// It is added to the ID allocated to the first /real/ menu item (ie. probably "New Script")

#define DYNAMIC_ADD_ID 1700


struct FuncItem;
class IDAllocator;
class DynamicIDManager;

class MenuManager
{

public:

	~MenuManager();

	typedef std::vector<std::pair<tstring, void (*)()> > ItemVectorTD;
	typedef void (*runScriptIDFunc)(idx_t);
	typedef void(*runScriptFunc)(const char *, bool, HANDLE, bool);

	static MenuManager* create(HWND hNotepad, HINSTANCE hInst, runScriptFunc runScript);
	static MenuManager* getInstance();
	static void         deleteInstance();


	void stopScriptEnabled(bool enabled);

	FuncItem* getFuncItemArray(int *nbF, ItemVectorTD items, runScriptIDFunc runScript, idx_t dynamicStartIndex, idx_t scriptsMenuIndex, idx_t stopScriptIndex, idx_t runPreviousIndex);
	FuncItem* getFuncItems() { return m_funcItems; }

	bool populateScriptsMenu();
	void menuCommand(idx_t commandID);
	void toolbarCommand(idx_t commandID);

	void reconfigure();

	void refreshScriptsMenu();


	void configureToolbarIcons();

	idx_t findPluginCommand(const TCHAR *pluginName, const TCHAR *menuOption, bool refreshCache);
	idx_t findMenuCommand(const TCHAR *menuName, const TCHAR *menuOption, bool refreshCache);

	idx_t findMenuCommand(HMENU parentMenu, const TCHAR *menuName, const TCHAR *menuOption);

	void updatePreviousScript(const char *filename);
	void updateShortcut(UINT cmdID, ShortcutKey* key);
	void initPreviousScript();

	void idsInitialised();

	bool inDynamicRange(idx_t commandID);
	bool inToolbarRange(idx_t commandID);
	bool inFixedRange(idx_t commandID);
	

	BOOL processWmCommand(WPARAM wParam, LPARAM lParam);
	idx_t getOriginalCommandID(idx_t scriptNumber);

	static bool s_menuItemClicked;
	static WNDPROC s_origWndProc;
	
	

private:
	MenuManager(); // default constructor disabled

	MenuManager(HWND hNotepad, HINSTANCE hInst, runScriptFunc runScript);

	HMENU getOurMenu();
	bool findScripts(HMENU hBaseMenu, size_t basePathLength, std::string& path);
	void subclassNotepadPlusPlus();
	

	tstring formatMenuName(const TCHAR *name);

	

	runScriptFunc m_runScript;

	typedef std::set<std::string> MachineScriptNamesTD;
	typedef std::map<idx_t, std::string> ScriptCommandsTD;
	typedef std::map<std::string, HMENU> SubmenusTD;
	typedef std::map<std::pair<tstring, tstring>, idx_t> MenuCommandCacheTD;
	MenuCommandCacheTD m_menuCommandCache;
	MenuCommandCacheTD m_pluginCommandCache;

	MachineScriptNamesTD m_machineScriptNames;
	ScriptCommandsTD m_scriptCommands;
	ScriptCommandsTD m_toolbarCommands;
	ScriptCommandsTD m_menuCommands;

	SubmenusTD m_submenus;
	typedef std::map<int, tstring>  KeyMapTD;
	KeyMapTD m_keyMap;

	tstring getKeyName(ShortcutKey& sk);

	static MenuManager* s_menuManager;

	HWND		m_hNotepad;
	HINSTANCE	m_hInst;
	idx_t		m_dynamicStartIndex;
	size_t		m_dynamicCount;
	size_t		m_originalDynamicCount;
	idx_t		m_scriptsMenuIndex;
	idx_t		m_stopScriptIndex;
	idx_t		m_runPreviousIndex;
	idx_t		m_originalLastCmdIndex;
	HMENU		m_pythonPluginMenu;
	HMENU		m_hScriptsMenu;
	FuncItem*   m_funcItems;
	size_t		m_funcItemCount;
	std::string m_machineScriptsPath;
	std::string m_userScriptsPath;
	tstring		m_runLastScriptShortcut;
	std::string m_previousRunFilename;

	IDAllocator* m_idAllocator;
	DynamicIDManager* m_dynamicMenuManager;
	DynamicIDManager* m_originalDynamicMenuManager;
	DynamicIDManager* m_scriptsMenuManager;
	DynamicIDManager* m_toolbarMenuManager;

	// Function pointer to the real run script function
	static runScriptIDFunc s_runScript;

	// static functions to run the scripts
	static void runScript0()  { s_runScript(0);  }
	static void runScript1()  { s_runScript(1);  }
	static void runScript2()  { s_runScript(2);  }
	static void runScript3()  { s_runScript(3);  }
	static void runScript4()  { s_runScript(4);  }
	static void runScript5()  { s_runScript(5);  }
	static void runScript6()  { s_runScript(6);  }
	static void runScript7()  { s_runScript(7);  }
	static void runScript8()  { s_runScript(8);  }
	static void runScript9()  { s_runScript(9);  }
	static void runScript10() { s_runScript(10); }
	static void runScript11() { s_runScript(11); }
	static void runScript12() { s_runScript(12); }
	static void runScript13() { s_runScript(13); }
	static void runScript14() { s_runScript(14); }
	static void runScript15() { s_runScript(15); }
	static void runScript16() { s_runScript(16); }
	static void runScript17() { s_runScript(17); }
	static void runScript18() { s_runScript(18); }
	static void runScript19() { s_runScript(19); }
	static void runScript20() { s_runScript(20); }
	static void runScript21() { s_runScript(21); }
	static void runScript22() { s_runScript(22); }
	static void runScript23() { s_runScript(23); }
	static void runScript24() { s_runScript(24); }
	static void runScript25() { s_runScript(25); }
	static void runScript26() { s_runScript(26); }
	static void runScript27() { s_runScript(27); }
	static void runScript28() { s_runScript(28); }
	static void runScript29() { s_runScript(29); }
	static void runScript30() { s_runScript(30); }
	static void runScript31() { s_runScript(31); }
	static void runScript32() { s_runScript(32); }
	static void runScript33() { s_runScript(33); }
	static void runScript34() { s_runScript(34); }
	static void runScript35() { s_runScript(35); }
	static void runScript36() { s_runScript(36); }
	static void runScript37() { s_runScript(37); }
	static void runScript38() { s_runScript(38); }
	static void runScript39() { s_runScript(39); }
	static void runScript40() { s_runScript(40); }
	static void runScript41() { s_runScript(41); }
	static void runScript42() { s_runScript(42); }
	static void runScript43() { s_runScript(43); }
	static void runScript44() { s_runScript(44); }
	static void runScript45() { s_runScript(45); }
	static void runScript46() { s_runScript(46); }
	static void runScript47() { s_runScript(47); }
	static void runScript48() { s_runScript(48); }
	static void runScript49() { s_runScript(49); }

	void (*m_runScriptFuncs[MAX_MENU_SCRIPTS])(void);

	
};

#endif // _MENUMANAGER_H
