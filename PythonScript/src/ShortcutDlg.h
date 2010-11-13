#ifndef _SHORTCUTDLG_H
#define _SHORTCUTDLG_H

#ifndef STATIC_DIALOG_H
#include "StaticDialog.h"
#endif

#ifndef _CONFIGFILE_H 
#include "ConfigFile.h"
#endif

struct NppData;

class ShortcutDlg : StaticDialog
{
public:
	ShortcutDlg(HINSTANCE hInst, NppData& nppData, const TCHAR *scriptDirAppend);
	
	void doDialog();

protected: 
	BOOL CALLBACK run_dlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

private:
	void populateScripts(tstring dir, HTREEITEM parent = TVI_ROOT);
	void onInitDialog();
	void populateUserScripts();
	void populateMachineScripts();
	void clearScripts();

	void addMenuItem();
	void addMenuItem(const TCHAR *item);
	void removeMenuItem();
	void addToolbarItem();
	void addToolbarItem(const TCHAR *item, HBITMAP iconPath);
	void removeToolbarItem();

	void populateCurrentItems();

	void scriptSelected();
	void nonScriptSelected();
	void toolbarSetIcon();
	
	void saveConfig();

	HTREEITEM addTreeItem(HTREEITEM parent, HTREEITEM lastItem, TCHAR *fullPath, TCHAR *text, bool isDirectory);

	tstring m_userScriptDir;
	tstring m_machineScriptDir;
	HWND m_hTree;
	HWND m_hListMenuItems;
	HWND m_hListToolbarItems;
	HWND m_hComboInitialisation;

	HIMAGELIST m_hImageList;
	int m_hDefaultImageIndex;
	HIMAGELIST m_hIcons;
	int m_iconFolderOpen;
	int m_iconFolderClosed;
	int m_iconPython;
	int m_toolbarItemCount;
	int m_menuItemCount;
	int m_toolbarColumnWidth;
	int m_menuItemColumnWidth;
	std::list< std::shared_ptr<TCHAR> > m_itemList;
	TCHAR *m_currentScript;
	ConfigFile::MenuItemsTD m_menuItems;
	ConfigFile::ToolbarItemsTD m_toolbarItems;
		

	static const int COLUMN_PADDING = 5;
};

#endif
