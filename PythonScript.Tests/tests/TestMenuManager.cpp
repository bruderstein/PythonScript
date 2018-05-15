#include "stdafx.h"


#include <gtest/gtest.h>
#include "MenuManager.h"
#include <Notepad_plus_msgs.h>

namespace NppPythonScript 
{

static volatile HMENU g_hPluginMenu;

static LRESULT dummyWindowProc(HWND /* hWnd */, UINT message, WPARAM /* wParam */, LPARAM /* lParam */)
{
    switch(message)
	{
	case WM_CREATE:
        return FALSE;

	case NPPM_GETMENUHANDLE:
        return reinterpret_cast<LRESULT>(g_hPluginMenu);

	default:
        return TRUE;
	}
}


#define UNIT_TEST_WINDOW_CLASS  L"UnitTestDummy"


enum MenuIds {
        MENUID_FILE_OPEN = 1000,
        MENUID_FILE_CLOSE,
        MENUID_FILE_RECENT,
        MENUID_FILE_EXIT,
        MENUID_FILE_RECENT_1,
        MENUID_FILE_RECENT_2,
        MENUID_FILE_RECENT_3,
        MENUID_EDIT_COPY,
        MENUID_EDIT_PASTE,
        MENUID_XMLTOOLS_PRETTYPRINT,
        MENUID_XMLTOOLS_OPEN,
        MENUID_XMLTOOLS_RECENT_1,
        MENUID_XMLTOOLS_ABOUT,
        MENUID_SECONDPLUGIN_PRETTYPRINT,
        MENUID_SECONDPLUGIN_OPEN,
        MENUID_SECONDPLUGIN_ABOUT,
        MENUID_SECONDPLUGIN_RECENT_1,
        MENUID_SECONDPLUGIN_RECENT_2
};

class MenuManagerTest : public ::testing::Test {
    virtual void SetUp() {
        m_hModule = ::GetModuleHandle(NULL);

        RegisterTestWindowClass();
        CreateTestMenu();
        CreateTestWindow();
        ASSERT_NE(0, (LONG_PTR)m_hWindow);
        MenuManager::create(m_hWindow, reinterpret_cast<HINSTANCE>(m_hModule), NULL);
	    m_menuManager = MenuManager::getInstance();
	}

    virtual void TearDown() {
        DestroyWindow(m_hWindow);
        UnregisterClass(UNIT_TEST_WINDOW_CLASS, m_hModule);
        DestroyMenu(m_hMenu);
        MenuManager::destroyForTests();
	}

    
    
protected:
    HMENU m_hMenu;
    HWND m_hWindow;
    HMODULE m_hModule;
    MenuManager* m_menuManager;


    void RegisterTestWindowClass() {
        WNDCLASS wndClass;
        memset(&wndClass, 0, sizeof(WNDCLASS));

        wndClass.lpfnWndProc = (WNDPROC)&dummyWindowProc;
        wndClass.hInstance = m_hModule;
        wndClass.lpszClassName = UNIT_TEST_WINDOW_CLASS;
        wndClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
        
        
		::RegisterClassW(&wndClass);
	}
    

    /*
    Menu Structure:

    File
      --> Open
      --> Close
      --> Recent
          --> Recent 1
          --> Recent 2
          --> Recent 3
      --> Exit

    Edit
      --> Copy
      --> Paste

    Plugins
      --> XML Tools
          --> Pretty Print
          --> Open
          --> Recent 1
          --> About
      --> Second Plugin
          --> Pretty Print
          --> Open
          --> Recent
              --> Recent 1
              --> Recent 2
          --> About
         



    */

    void CreateTestMenu() {
		m_hMenu = ::CreateMenu();
        
        HMENU fileMenu = ::CreateMenu();
		::AppendMenu(m_hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(fileMenu), L"File"); 
        ::AppendMenu(fileMenu, MF_STRING, MENUID_FILE_OPEN, L"Open\tCtrl-O");
		::AppendMenu(fileMenu, MF_STRING, MENUID_FILE_CLOSE, L"Close\tCtrl-F4");
		
        HMENU recentMenu = ::CreateMenu();
		::AppendMenu(recentMenu, MF_STRING, MENUID_FILE_RECENT_1, L"Recent 1");
		::AppendMenu(recentMenu, MF_STRING, MENUID_FILE_RECENT_2, L"Recent 2");
		::AppendMenu(recentMenu, MF_STRING, MENUID_FILE_RECENT_3, L"Recent 3");
		::AppendMenu(fileMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(recentMenu), L"Recent");

		::AppendMenu(fileMenu, MF_STRING, MENUID_FILE_CLOSE, L"Exit\tAlt-F4");

		HMENU editMenu = ::CreateMenu();
		::AppendMenu(m_hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(editMenu), L"Edit");
		::AppendMenu(editMenu, MF_STRING, MENUID_EDIT_COPY, L"Copy");
		::AppendMenu(editMenu, MF_STRING, MENUID_EDIT_PASTE, L"Paste\tCtrl-V");
		
        g_hPluginMenu = ::CreateMenu();
		::AppendMenu(m_hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(g_hPluginMenu), L"Plugins");
		
        HMENU xmlToolsPluginMenu = ::CreateMenu();
		::AppendMenu(g_hPluginMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(xmlToolsPluginMenu), L"XML Tools");
		::AppendMenu(xmlToolsPluginMenu, MF_STRING, MENUID_XMLTOOLS_PRETTYPRINT, L"Pretty Print\tCtrl-Alt-Shift-B");
		::AppendMenu(xmlToolsPluginMenu, MF_STRING, MENUID_XMLTOOLS_OPEN, L"Open\tCtrl-O");
		::AppendMenu(xmlToolsPluginMenu, MF_STRING, MENUID_XMLTOOLS_RECENT_1, L"Recent 1");
		::AppendMenu(xmlToolsPluginMenu, MF_STRING, MENUID_XMLTOOLS_ABOUT, L"A&bout");

        HMENU secondPluginMenu = ::CreateMenu();

		::AppendMenu(g_hPluginMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(secondPluginMenu), L"Second Plugin");
		::AppendMenu(secondPluginMenu, MF_STRING, MENUID_SECONDPLUGIN_PRETTYPRINT, L"Pretty Print\tCtrl-Alt-Shift-B");
		::AppendMenu(secondPluginMenu, MF_STRING, MENUID_SECONDPLUGIN_OPEN, L"Open\tCtrl-O");
        HMENU secondPluginRecentMenu = ::CreateMenu();
		::AppendMenu(secondPluginMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(secondPluginRecentMenu), L"Recent");
		::AppendMenu(secondPluginRecentMenu, MF_STRING, MENUID_SECONDPLUGIN_RECENT_1, L"Recent 1");
		::AppendMenu(secondPluginRecentMenu, MF_STRING, MENUID_SECONDPLUGIN_RECENT_2, L"Recent 2");

		::AppendMenu(secondPluginMenu, MF_STRING, MENUID_SECONDPLUGIN_ABOUT, L"A&bout");

	}

    void CreateTestWindow() {
        m_hWindow = ::CreateWindow(
			UNIT_TEST_WINDOW_CLASS,                /* Class name */
            L"Test window",
            WS_OVERLAPPEDWINDOW,
            0, 0, 100, 100,       /* x, y, width, heigth */
            NULL,                 /* hWndParent */
			m_hMenu,
            m_hModule,
            NULL                  /* lpParam */
			);
	}

  
};

struct MenuTestCase {
	MenuTestCase() {}
    MenuTestCase(const TCHAR *pmenuName, const TCHAR *poptionName, int pexpectedID) 
		: menuName(pmenuName),
          optionName(poptionName),
          expectedID(pexpectedID)
	{}

    const TCHAR *menuName;
    const TCHAR *optionName;
    size_t expectedID;
};



class MenuManagerFindMenuCommandTest : public MenuManagerTest,
	public ::testing::WithParamInterface<MenuTestCase> {
        
};

TEST_P(MenuManagerFindMenuCommandTest, testFindOptionFindsCorrectOption) {
    MenuTestCase testCase = GetParam();
    LONG_PTR windowWndProc = GetWindowLongPtr(m_hWindow, GWLP_WNDPROC);
    LONG_PTR dummyProc = reinterpret_cast<LONG_PTR>(&dummyWindowProc);
    ASSERT_EQ(windowWndProc, dummyProc);
    size_t foundMenuId = m_menuManager->findMenuCommand(m_hMenu, NULL, testCase.menuName, testCase.optionName);
    ASSERT_EQ(testCase.expectedID, foundMenuId);
}


INSTANTIATE_TEST_CASE_P(findMenuCommand, MenuManagerFindMenuCommandTest,
	::testing::Values(MenuTestCase(L"File", L"Open", MENUID_FILE_OPEN),                     // First option
                      MenuTestCase(L"File", L"Close", MENUID_FILE_CLOSE),                   // First menu, middle option
                      MenuTestCase(L"Edit", L"Copy", MENUID_EDIT_COPY),                     // Mid menu, first option
                      MenuTestCase(L"Edit", L"Paste", MENUID_EDIT_PASTE),                   // Mid menu, last option
					  MenuTestCase(L"Recent", L"Recent 1", MENUID_FILE_RECENT_1),           // Sub menu, first option
					  MenuTestCase(L"Recent", L"Recent 2", MENUID_FILE_RECENT_2),
					  MenuTestCase(L"Recent", L"Recent 3", MENUID_FILE_RECENT_3),           // Sub menu, last option
                      MenuTestCase(L"File", L"Recent 2", 0),                                // Shouldn't find Recent 2 directly under the File menu
                      MenuTestCase(L"XML Tools", L"Open", MENUID_XMLTOOLS_OPEN),            // Repeated option name, on third menu
                      MenuTestCase(L"Second Plugin", L"About", MENUID_SECONDPLUGIN_ABOUT),  // Repeated option name, with & in title
                      MenuTestCase(L"Second Plugin", L"Not found", 0),                      // Non existent option
                      MenuTestCase(L"Not exists", L"Open", 0)                               // Non existent menu
					  ));

class MenuManagerFindPluginCommandTest : public MenuManagerTest,
	public ::testing::WithParamInterface<MenuTestCase> {

};

TEST_P(MenuManagerFindPluginCommandTest, testFindPluginCommand) {
    MenuTestCase testCase = GetParam();
    size_t foundMenuId = m_menuManager->findPluginCommand(testCase.menuName, testCase.optionName, false);
    ASSERT_EQ(testCase.expectedID, foundMenuId);
}
    

INSTANTIATE_TEST_CASE_P(findPluginMenuCommand, MenuManagerFindPluginCommandTest,
	::testing::Values(MenuTestCase(L"XML Tools", L"About", MENUID_XMLTOOLS_ABOUT),                  // First plugin, last option
                      MenuTestCase(L"Second Plugin", L"About", MENUID_SECONDPLUGIN_ABOUT),          // Second plugin, repeated option name from other plugin
                      MenuTestCase(L"XML Tools", L"Recent 1", MENUID_XMLTOOLS_RECENT_1),            // First plugin, repeated option name from main menu
                      MenuTestCase(L"Second Plugin", L"Recent 1", MENUID_SECONDPLUGIN_RECENT_1),    // Second plugin, sub menu option
                      MenuTestCase(L"Second Plugin", L"Recent 2", MENUID_SECONDPLUGIN_RECENT_2),    // Second plugin, sub menu option
                      MenuTestCase(L"Second Plugin", L"Not exists", 0),                             // Plugin option doesn't exist
                      MenuTestCase(L"New Plugin", L"Open", 0)                                       // Plugin doesn't exist
					  ));
                      
}