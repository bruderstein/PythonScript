#include "stdafx.h"

#include "ShortcutDlg.h"
#include "PluginInterface.h"
#include "resource.h"
#include "Notepad_Plus_Msgs.h"
#include "ConfigFile.h"

using namespace std;

ShortcutDlg::ShortcutDlg(HINSTANCE hInst, NppData nppData, const TCHAR *scriptDirAppend)
{
	Window::init(hInst, nppData._nppHandle);
	TCHAR temp[MAX_PATH];
	::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(temp));
		
	m_userScriptDir = temp;
	m_userScriptDir.append(scriptDirAppend);

	::SendMessage(nppData._nppHandle, NPPM_GETNPPDIRECTORY, MAX_PATH, reinterpret_cast<LPARAM>(temp));

	m_machineScriptDir = temp;
	m_machineScriptDir.append(scriptDirAppend);
}


void ShortcutDlg::doDialog()
{
	if (!isCreated())
	{
		create(IDD_SCRIPTCONFIG);
	}

	populateScripts(m_userScriptDir);
	populateCurrentItems();
	nonScriptSelected();

	goToCenter();
}


BOOL CALLBACK ShortcutDlg::run_dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			onInitDialog();
			return TRUE;

		case WM_COMMAND:
			switch(wParam)
			{
				case IDOK:
					saveConfig();
				case IDCANCEL:
					display(FALSE);
					return TRUE;

				case IDC_RADMACHINE:
					populateMachineScripts();
					break;

				case IDC_RADUSER:
					populateUserScripts();
					break;

				case IDC_MENUADD:
					addMenuItem();
					break;

				case IDC_MENUREMOVE:
					removeMenuItem();
					break;

				case IDC_TOOLBARADD:
					addToolbarItem();
					break;

				case IDC_TOOLBARREMOVE:
					removeToolbarItem();
					break;

				default:
					break;
			}
			break;
		case WM_NOTIFY:
		{
			LPNMHDR hdr = reinterpret_cast<LPNMHDR>(lParam);
			switch(hdr->idFrom)
			{
				case IDC_FILETREE:
				{
					switch (hdr->code)
					{
						case TVN_GETDISPINFO:
						{
							LPNMTVDISPINFO lptvdi = reinterpret_cast<LPNMTVDISPINFO>(lParam);
							int imageIndex;
							if (lptvdi->item.mask & (TVIF_IMAGE |TVIF_SELECTEDIMAGE))
							{
								if (lptvdi->item.state & TVIS_EXPANDED)
								{
									imageIndex = m_iconFolderOpen;
								}
								else
								{
									// a non-NULL lParam means it's a python file (with a full path)
									if (lptvdi->item.lParam)
										imageIndex = m_iconPython;
									else 
										imageIndex = m_iconFolderClosed;
								}

								if (lptvdi->item.state & TVIS_SELECTED)
								{
									lptvdi->item.iSelectedImage = imageIndex;
									lptvdi->item.mask = TVIF_SELECTEDIMAGE;
								}
								else
								{
									lptvdi->item.iImage = imageIndex;
									lptvdi->item.mask = TVIF_IMAGE;
								}
							}
							return TRUE;
						} // end case TVN_GETDISPINFO
						break;

						case TVN_SELCHANGED:
						{
							LPNMTREEVIEW pnmtv = reinterpret_cast<LPNMTREEVIEW>(lParam);
							if (pnmtv->itemNew.lParam)
							{
								scriptSelected();
								m_currentScript = reinterpret_cast<TCHAR*>(pnmtv->itemNew.lParam);
							}
							else
							{
								nonScriptSelected();
								m_currentScript = NULL;
							}
						}
						break;
					} // end switch (hdr->code)

				}  // end case IDC_FILETREE
				break;  

				
			} // end switch hdr->idFrom 
			break; 
		} // end case WM_NOTIFY
	} // end switch (message)
	return FALSE; // ::DefWindowProc(hWnd, message, wParam, lParam);
}


void ShortcutDlg::onInitDialog()
{
	// Set the radio button to default to USER SCRIPTS
	::SendMessage(GetDlgItem(_hSelf, IDC_RADUSER), BM_SETCHECK, BST_CHECKED, 0);
	::SendMessage(GetDlgItem(_hSelf, IDC_RADMACHINE), BM_SETCHECK, BST_UNCHECKED, 0);
	m_hTree = ::GetDlgItem(_hSelf, IDC_FILETREE);
	m_hListMenuItems = ::GetDlgItem(_hSelf, IDC_MENUITEMLIST);
	m_hListToolbarItems = ::GetDlgItem(_hSelf, IDC_TOOLBARITEMLIST);

	InitCommonControls();
	HICON hIcon;           // handle to icon 
 
	// Create a masked image list large enough to hold the icons. 
	m_hIcons = ImageList_Create(16, 16, ILC_MASK | ILC_COLOR32, 3, 0); 
 
	// Load the icon resources, and add the icons to the image list. 
	hIcon = LoadIcon(_hInst, MAKEINTRESOURCE(IDI_FOLDERCLOSED)); 
	m_iconFolderClosed = ImageList_AddIcon(m_hIcons, hIcon); 
 
	hIcon = LoadIcon(_hInst, MAKEINTRESOURCE(IDI_FOLDEROPEN)); 
	m_iconFolderOpen = ImageList_AddIcon(m_hIcons, hIcon); 

	hIcon = LoadIcon(_hInst, MAKEINTRESOURCE(IDI_PYTHON)); 
	m_iconPython = ImageList_AddIcon(m_hIcons, hIcon); 

	::SendMessage(m_hTree, TVM_SETIMAGELIST, TVSIL_NORMAL, reinterpret_cast<LPARAM>(m_hIcons));
	::SendMessage(m_hTree, TVM_SETIMAGELIST, TVSIL_STATE, reinterpret_cast<LPARAM>(m_hIcons));

	

	
}

void ShortcutDlg::clearScripts()
{
	TreeView_DeleteAllItems(m_hTree);
	m_itemList.erase(m_itemList.begin(), m_itemList.end());
}


void ShortcutDlg::populateUserScripts()
{
	clearScripts();
	populateScripts(m_userScriptDir);
}

void ShortcutDlg::populateMachineScripts()
{
	clearScripts();
	populateScripts(m_machineScriptDir);
}



void ShortcutDlg::populateScripts(tstring dir, HTREEITEM parent /* = TVI_ROOT */)
{
	WIN32_FIND_DATA findData;
	tstring indexPath;
	tstring searchPath(dir);
	searchPath.append(_T("\\*"));
	HANDLE hFound = FindFirstFile(searchPath.c_str(), &findData);
	BOOL found = (hFound != INVALID_HANDLE_VALUE) ? TRUE : FALSE;
	int position = 0;
	HTREEITEM lastItem = TVI_ROOT;

	while (found)
	{
		if (FILE_ATTRIBUTE_DIRECTORY == (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& _tcscmp(findData.cFileName, _T(".")) 
			&& _tcscmp(findData.cFileName, _T("..")))
		{
			searchPath = dir;
			searchPath.append(_T("\\"));
			searchPath.append(findData.cFileName);
			
			
			lastItem = addTreeItem(parent, lastItem, NULL, findData.cFileName, true);
			
			
			populateScripts(searchPath, lastItem);
			
		}

		found = FindNextFile(hFound, &findData);
	}

	FindClose(hFound);


	searchPath = dir;
	searchPath.append(_T("\\*.py"));
	hFound = FindFirstFile(searchPath.c_str(), &findData);
	found = (hFound != INVALID_HANDLE_VALUE) ? TRUE : FALSE;
	
	
	while(found)
	{
		tstring fullFilename(dir);
		fullFilename.append(_T("\\"));
		fullFilename.append(findData.cFileName);
		
		int length = fullFilename.size() + 1;
		shared_ptr<TCHAR> item(new TCHAR[length]);
		
		_tcscpy_s(item.get(), length, fullFilename.c_str());

		m_itemList.push_back(item);

		lastItem = addTreeItem(parent, lastItem, item.get(), findData.cFileName, false);
		
		
		found = FindNextFile(hFound, &findData);
	}

	FindClose(hFound);
	
}





HTREEITEM ShortcutDlg::addTreeItem(HTREEITEM parent, HTREEITEM lastItem, TCHAR *fullPath, TCHAR *text, bool isDirectory)
{
	TV_INSERTSTRUCT tvInsert;
	tvInsert.hParent = parent; 
	tvInsert.hInsertAfter = TVI_SORT;
	tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM |TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvInsert.item.lParam = reinterpret_cast<LPARAM>(fullPath);
	tvInsert.item.pszText = text;
	tvInsert.item.iImage = isDirectory ? I_IMAGECALLBACK : m_iconPython;
	tvInsert.item.iSelectedImage = isDirectory ? I_IMAGECALLBACK : m_iconPython;
	return (HTREEITEM)SendMessage(m_hTree, TVM_INSERTITEM, 0, reinterpret_cast<LPARAM>(&tvInsert));

}


void ShortcutDlg::scriptSelected()
{
	::EnableWindow(GetDlgItem(_hSelf, IDC_MENUADD), true);
	::EnableWindow(GetDlgItem(_hSelf, IDC_TOOLBARADD), true);
}

void ShortcutDlg::nonScriptSelected()
{
	::EnableWindow(GetDlgItem(_hSelf, IDC_MENUADD), false);
	::EnableWindow(GetDlgItem(_hSelf, IDC_TOOLBARADD), false);
}


void ShortcutDlg::addMenuItem()
{
	::SendMessage(m_hListMenuItems, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(m_currentScript));
}

void ShortcutDlg::addMenuItem(const TCHAR *item)
{
	::SendMessage(m_hListMenuItems, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(item));
}

void ShortcutDlg::removeMenuItem()
{
	int index = ::SendMessage(m_hListMenuItems, LB_GETCURSEL, 0, 0);

	if (index != LB_ERR)
	{
		::SendMessage(m_hListMenuItems, LB_DELETESTRING, index, 0);
	}
}

void ShortcutDlg::addToolbarItem()
{
	::SendMessage(m_hListToolbarItems, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(m_currentScript));
}

void ShortcutDlg::addToolbarItem(const TCHAR *item)
{
	::SendMessage(m_hListToolbarItems, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(item));
}

void ShortcutDlg::removeToolbarItem()
{
	int index = ::SendMessage(m_hListToolbarItems, LB_GETCURSEL, 0, 0);

	if (index != LB_ERR)
	{
		::SendMessage(m_hListToolbarItems, LB_DELETESTRING, index, 0);
	}
}


void ShortcutDlg::populateCurrentItems()
{
	::SendMessage(m_hListMenuItems, LB_RESETCONTENT, 0, 0);
	::SendMessage(m_hListToolbarItems, LB_RESETCONTENT, 0, 0);

	ConfigFile* configFile = ConfigFile::getInstance();
	configFile->refresh();


	ConfigFile::MenuItemsTD menuItems = configFile->getMenuItems();
	for(ConfigFile::MenuItemsTD::iterator it = menuItems.begin(); it != menuItems.end(); ++it)
	{
		addMenuItem(it->c_str());
	}

	ConfigFile::ToolbarItemsTD toolbarItems = configFile->getToolbarItems();
	for(ConfigFile::ToolbarItemsTD::iterator it = toolbarItems.begin(); it != toolbarItems.end(); ++it)
	{
		addToolbarItem(it->first.c_str());
	}
}



void ShortcutDlg::saveConfig()
{
	ConfigFile* configFile = ConfigFile::getInstance();
	configFile->clearItems();
	int count = ::SendMessage(m_hListMenuItems, LB_GETCOUNT, 0, 0);
	TCHAR buffer[MAX_PATH];

	for (int index = 0; index < count; ++index)
	{	
		::SendMessage(m_hListMenuItems, LB_GETTEXT, index, reinterpret_cast<LPARAM>(buffer));
		configFile->addMenuItem(buffer);
	}

	count = ::SendMessage(m_hListToolbarItems, LB_GETCOUNT, 0, 0);

	for (int index = 0; index < count; ++index)
	{	
		::SendMessage(m_hListToolbarItems, LB_GETTEXT, index, reinterpret_cast<LPARAM>(buffer));
		configFile->addToolbarItem(buffer, _T(""));
	}

	configFile->save();


}


