#include "stdafx.h"

#include "PythonScript.h"


// The DYNAMIC_ADD_ID is used for the CommandIDs for dynamic menu entries added between restarts
// It is added to the ID allocated to the first /real/ menu item (ie. probably "New Script")
// Therefore, it should be a minimum of 50 below the ADD_CMD_ID, as there can only be 50
// dynamic entries.
#define DYNAMIC_ADD_ID 1700

// Added to first real menu item command ID for the dynamic Scripts sub menu
#define ADD_CMD_ID     1750

// Added to first real menu item for toolbar commandIDs 
#define ADD_TOOLBAR_ID 2150

struct FuncItem;

class MenuManager
{

public:
	~MenuManager();

	typedef std::vector<std::pair<tstring, void (*)()> > ItemVectorTD;


	static MenuManager* create(HWND hNotepad, HINSTANCE hInst, void(*runScript)(const char *));
	static MenuManager* getInstance();



	void stopScriptEnabled(bool enabled);

	FuncItem* getFuncItemArray(int *nbF, ItemVectorTD items, void (*runScript)(int), int dynamicStartIndex, int scriptsMenuIndex, int stopScriptIndex);

	bool populateScriptsMenu();
	void menuCommand(int commandID);
	void toolbarCommand(int commandID);

	void reconfigure();

	void refreshScriptsMenu();


	void configureToolbarIcons();

	static int s_startCommandID;
	static int s_endCommandID;
	static int s_startFixedID;
	static int s_endFixedID;
	static int s_startDynamicEntryID;
	static int s_endDynamicEntryID;
	static int s_startToolbarID;
	static int s_endToolbarID;

	static bool s_menuItemClicked;
	static WNDPROC s_origWndProc;
	
	

private:
	MenuManager(HWND hNotepad, HINSTANCE hInst, void(*runScript)(const char *));

	

	HMENU getOurMenu();
	int findScripts(HMENU hBaseMenu, int basePathLength, int startID, std::string& path);
	void subclassNotepadPlusPlus();

	void (*m_runScript)(const char*);

	typedef std::set<std::string> MachineScriptNamesTD;
	typedef std::map<int, std::string> ScriptCommandsTD;
	typedef std::map<std::string, HMENU> SubmenusTD;

	MachineScriptNamesTD m_machineScriptNames;
	ScriptCommandsTD m_scriptCommands;
	ScriptCommandsTD m_toolbarCommands;
	SubmenusTD m_submenus;


	static MenuManager* s_menuManager;

	HWND		m_hNotepad;
	HINSTANCE	m_hInst;
	int			m_dynamicStartIndex;
	int			m_dynamicCount;
	int			m_originalDynamicCount;
	int			m_scriptsMenuIndex;
	int			m_stopScriptIndex;
	HMENU		m_pythonPluginMenu;
	HMENU		m_hScriptsMenu;
	FuncItem*   m_funcItems;
	int         m_funcItemCount;
	std::string m_machineScriptsPath;
	std::string m_userScriptsPath;

	// Function pointer to the real run script function
	static void (*s_runScript)(int);

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

