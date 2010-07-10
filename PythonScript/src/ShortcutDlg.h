#ifndef _SHORTCUTDLG_H
#define _SHORTCUTDLG_H

#include "stdafx.h"
#include "StaticDialog.h"
#include "PluginInterface.h"

class ShortcutDlg : StaticDialog
{
public:
	ShortcutDlg(HINSTANCE hInst, NppData nppData, const TCHAR *scriptDirAppend);
	BOOL CALLBACK run_dlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	
	void doDialog();

private:
	void populateScripts(tstring dir, HTREEITEM parent = TVI_ROOT);
	void onInitDialog();
	void populateUserScripts();
	void populateMachineScripts();
	void clearScripts();

	void scriptSelected();
	void nonScriptSelected();

	HTREEITEM addTreeItem(HTREEITEM parent, HTREEITEM lastItem, TCHAR *fullPath, TCHAR *text, bool isDirectory);

	tstring m_userScriptDir;
	tstring m_machineScriptDir;
	HWND m_hTree;
	HIMAGELIST m_hIcons;
	int m_iconFolderOpen;
	int m_iconFolderClosed;
	int m_iconPython;
	std::list< std::shared_ptr<TCHAR> > m_itemList;
};


#endif