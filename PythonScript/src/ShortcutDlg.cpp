#include "stdafx.h"

#include "ShortcutDlg.h"
#include "PluginInterface.h"
#include "resource.h"
#include "Notepad_Plus_Msgs.h"

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
				case IDCANCEL:
					display(FALSE);
					return TRUE;

				case IDC_RADMACHINE:
					populateMachineScripts();
					break;

				case IDC_RADUSER:
					populateUserScripts();
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
							}
							else
							{
								nonScriptSelected();
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
	populateScripts(m_userScriptDir);

	nonScriptSelected();
}

void ShortcutDlg::clearScripts()
{
	TreeView_DeleteAllItems(m_hTree);
	// Hopefully this deletes the TCHAR* held under the shared_ptrs :)
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
		
		
		shared_ptr<TCHAR> item(new TCHAR[searchPath.size() + 1]);
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