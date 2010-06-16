#include "stdafx.h"

#define ADD_CMD_ID 1750

class MenuManager
{

public:

	static MenuManager* create(HWND hNotepad, int aboutCommandID, int aboutCommandIndex, void(*runScript)(const char *));
	static MenuManager* getInstance();


	~MenuManager();

	bool populateScriptsMenu();
	void menuCommand(int commandID);

	static int s_startCommandID;
	static int s_endCommandID;
	static WNDPROC s_origWndProc;

private:
	MenuManager(HWND hNotepad, int aboutCommandID, int aboutCommandIndex, void(*runScript)(const char *));

	

	HMENU getOurMenu();
	int findScripts(HMENU hBaseMenu, int basePathLength, int startID, std::string& path);
	void subclassNotepadPlusPlus();

	void (*m_runScript)(const char*);


	std::set<std::string> m_machineScriptNames;
	std::map<int, std::string> m_scriptCommands;
	std::map<std::string, HMENU> m_submenus;

	static MenuManager* m_menuManager;

	HWND m_hNotepad;
	
	int m_aboutCommandID;
	int m_aboutCommandIndex;


	
};

