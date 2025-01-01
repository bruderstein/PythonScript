#include "stdafx.h"

#include "ShortcutDlg.h"
#include "PluginInterface.h"
#include "resource.h"
#include "MenuManager.h"
#include "Notepad_plus_msgs.h"
#include "Utility.h"

ShortcutDlg::ShortcutDlg(HINSTANCE hInst, NppData& nppData, const TCHAR *scriptDirAppend)
	: m_hTree(NULL),
	  m_hListMenuItems(NULL),
	  m_hListToolbarItems(NULL),
	  m_hComboInitialisation(NULL),
	  m_hButtonColor(NULL),
	  m_hImageList(NULL),
	  m_hDefaultImageIndex(0),
	  m_hIcons(NULL),
	  m_iconFolderOpen(0),
	  m_iconFolderClosed(0),
	  m_iconPython(0),
	  m_toolbarItemCount(0),
	  m_menuItemCount(0),
	  m_toolbarColumnWidth(100),
	  m_menuItemColumnWidth(100),
	  m_currentScript(NULL)
{
	Window::init(hInst, nppData._nppHandle);
	TCHAR temp[MAX_PATH]{};
	::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(temp));

	m_userScriptDir = temp;
	m_userScriptDir.append(scriptDirAppend);

	::SendMessage(nppData._nppHandle, NPPM_GETNPPDIRECTORY, MAX_PATH, reinterpret_cast<LPARAM>(temp));

	m_machineScriptDir = temp;
	m_machineScriptDir.append(_T("\\plugins"));
	m_machineScriptDir.append(scriptDirAppend);
}


void ShortcutDlg::doDialog()
{
	if (!isCreated())
	{
		create(IDD_SCRIPTCONFIG);
	}


	clearScripts();

	if (BST_CHECKED == ::SendMessage(::GetDlgItem(_hSelf, IDC_RADMACHINE), BM_GETCHECK, 0, 0))
	{
		populateMachineScripts();
	}
	else
	{
		populateUserScripts();
	}

	populateCurrentItems();
	nonScriptSelected();

	goToCenter();
}


INT_PTR CALLBACK ShortcutDlg::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			onInitDialog();
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_COLORCHOOSER:
					ctrlOnClick();
					break;
				case IDC_CHECKCOLORIZEOUTPUT:
					if (HIWORD(wParam) == BN_CLICKED)
					{
						size_t result = SendMessage(reinterpret_cast<HWND>(lParam), BM_GETCHECK, 0, 0);
						EnableWindow(m_hButtonColor, (result == BST_CHECKED) ? true : false);
					}
					break;
			}
			switch(wParam)
			{
				case IDOK:
					saveConfig();
					MenuManager::getInstance()->reconfigure();
					[[fallthrough]];
					//lint -fallthrough

				case IDCANCEL:
					display(false);
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

				case IDC_TOOLBARSETICON:
					toolbarSetIcon();
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

						default:
							// Other notifications we can ignore
							break;
					} // end switch (hdr->code)

				}  // end case IDC_FILETREE
				break;

				default:
					// Other windows we can ignore
					break;
			} // end switch hdr->idFrom
		} // end case WM_NOTIFY
		break;

		default:
			// Other messages we can ignore
			break;

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
	m_hListToolbarItems = ::GetDlgItem(_hSelf, IDC_TOOLBARITEMLIST2);
	m_hComboInitialisation = ::GetDlgItem(_hSelf, IDC_COMBOINITIALISATION);
	m_hButtonColor = ::GetDlgItem(_hSelf, IDC_COLORCHOOSER);
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
	m_hImageList = ImageList_Create(16, 16, ILC_MASK |ILC_COLOR32, 5, 1);
	HBITMAP hPython = static_cast<HBITMAP>(LoadImage(_hInst, MAKEINTRESOURCE(IDB_PYTHON), IMAGE_BITMAP, 0, 0, LR_COLOR | LR_LOADMAP3DCOLORS | LR_DEFAULTSIZE));
	m_hDefaultImageIndex = ImageList_Add(m_hImageList, hPython, NULL);
	ListView_SetImageList(m_hListToolbarItems, m_hImageList, LVSIL_SMALL);
	LVCOLUMN lvCol{};
	lvCol.cchTextMax = 10;
	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	lvCol.iSubItem = 0;
	lvCol.pszText = _T("Script");
	lvCol.fmt = LVCFMT_LEFT;

	RECT rect;
	::GetClientRect(m_hListToolbarItems, &rect);
	m_toolbarColumnWidth = (size_t)((rect.right - rect.left) - 18);
	lvCol.cx = (int)m_toolbarColumnWidth;
	ListView_InsertColumn(m_hListToolbarItems, 0, &lvCol);

	::GetClientRect(m_hListToolbarItems, &rect);
	m_menuItemColumnWidth = (size_t)(rect.right - rect.left);
	lvCol.cx = (int)m_menuItemColumnWidth;
	ListView_InsertColumn(m_hListMenuItems, 0, &lvCol);

	ListView_SetExtendedListViewStyle(m_hListToolbarItems, LVS_EX_FULLROWSELECT);
	ListView_SetExtendedListViewStyle(m_hListMenuItems, LVS_EX_FULLROWSELECT);

	ComboBox_AddString(m_hComboInitialisation, _T("LAZY"));
	ComboBox_AddString(m_hComboInitialisation, _T("ATSTARTUP"));
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
	tstring searchPath(dir);
	searchPath.append(_T("\\*"));
	HANDLE hFound = FindFirstFile(searchPath.c_str(), &findData);
	BOOL found = (hFound != INVALID_HANDLE_VALUE) ? TRUE : FALSE;

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

		size_t length = fullFilename.size() + 1;
		std::shared_ptr<TCHAR> item(new TCHAR[length]);

		_tcscpy_s(item.get(), length, fullFilename.c_str());

		m_itemList.push_back(item);

		lastItem = addTreeItem(parent, lastItem, item.get(), findData.cFileName, false);


		found = FindNextFile(hFound, &findData);
	}

	FindClose(hFound);

}





HTREEITEM ShortcutDlg::addTreeItem(HTREEITEM parent, HTREEITEM /* lastItem */, TCHAR *fullPath, TCHAR *text, bool isDirectory)
{
	TV_INSERTSTRUCT tvInsert{};
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
	::EnableWindow(GetDlgItem(_hSelf, IDC_MENUADD), TRUE);
	::EnableWindow(GetDlgItem(_hSelf, IDC_TOOLBARADD), TRUE);
}

void ShortcutDlg::nonScriptSelected()
{
	::EnableWindow(GetDlgItem(_hSelf, IDC_MENUADD), FALSE);
	::EnableWindow(GetDlgItem(_hSelf, IDC_TOOLBARADD), FALSE);
}


void ShortcutDlg::addMenuItem()
{
	addMenuItem(m_currentScript);
	m_menuItems.push_back(m_currentScript);
}

void ShortcutDlg::addMenuItem(const TCHAR *item)
{
	LVITEM lvItem{};
	lvItem.stateMask = LVIS_SELECTED;
	lvItem.state = 0;
	lvItem.iItem = (int)(m_menuItemCount++);
	lvItem.mask = LVIF_TEXT;
	lvItem.iSubItem = 0;
	lvItem.state = 0;
	lvItem.stateMask = 0;

	TCHAR path[MAX_PATH];
	_tcscpy_s(path, MAX_PATH, item);
	::PathCompactPath(NULL, path, static_cast<UINT>(m_menuItemColumnWidth));

	lvItem.pszText = path;
	lvItem.cchTextMax = (int)_tcslen(path);
	ListView_InsertItem(m_hListMenuItems, &lvItem);
}

void ShortcutDlg::removeMenuItem()
{
	int index = ListView_GetNextItem(m_hListMenuItems, -1, LVIS_SELECTED);

	if (index != -1)
	{
		ListView_DeleteItem(m_hListMenuItems, index);
		m_menuItems.erase(m_menuItems.begin() + index);
		MenuManager::getInstance()->removeItem(index);
	}

	--m_menuItemCount;
}

void ShortcutDlg::addToolbarItem()
{
	addToolbarItem(m_currentScript, NULL);
	m_toolbarItems.push_back(std::pair<tstring, std::pair<HBITMAP, tstring> >(m_currentScript, std::pair<HBITMAP, tstring>(static_cast<HBITMAP>(NULL), _T(""))));
}

void ShortcutDlg::addToolbarItem(const TCHAR *item, HBITMAP hBitmap)
{
	int imageIndex;
	if (NULL == hBitmap)
	{
		imageIndex = m_hDefaultImageIndex;
	}
	else
	{
		imageIndex = ImageList_Add(m_hImageList, hBitmap, NULL);
	}

	LVITEM lvItem{};
	lvItem.stateMask = LVIS_SELECTED;
	lvItem.state = 0;
	lvItem.iItem = (int)(m_toolbarItemCount++);
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
	lvItem.iSubItem = 0;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iImage = imageIndex;

	TCHAR path[MAX_PATH];
	_tcscpy_s(path, MAX_PATH, item);
	::PathCompactPath(NULL, path, static_cast<UINT>(m_toolbarColumnWidth));

	lvItem.pszText = path;
	lvItem.cchTextMax = (int)_tcslen(path);
	ListView_InsertItem(m_hListToolbarItems, &lvItem);

	//int itemWidth = ListView_GetStringWidth(m_hListToolbarItems, item);
	//if (itemWidth > m_toolbarColumnWidth)
	//{
	//	ListView_SetColumnWidth(m_hListToolbarItems, 0, itemWidth + COLUMN_PADDING));
	//}


	//::SendMessage(m_hListToolbarItems, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(item));
}

void ShortcutDlg::removeToolbarItem()
{

	int index = ListView_GetNextItem(m_hListToolbarItems, -1, LVIS_SELECTED);

	if (index != -1)
	{
		ListView_DeleteItem(m_hListToolbarItems, index);
		m_toolbarItems.erase(m_toolbarItems.begin() + index);
	}
	--m_toolbarItemCount;

}


void ShortcutDlg::populateCurrentItems()
{
	ListView_DeleteAllItems(m_hListMenuItems);
	ListView_DeleteAllItems(m_hListToolbarItems);

	ConfigFile* configFile = ConfigFile::getInstance();
	configFile->refresh();

	m_menuItemCount = 0;
	m_menuItems = configFile->getMenuItems();
	for(ConfigFile::MenuItemsTD::iterator it = m_menuItems.begin(); it != m_menuItems.end(); ++it)
	{
		addMenuItem(it->c_str());
	}


	m_toolbarItemCount = 0;
	m_toolbarItems = configFile->getToolbarItems();
	for(ConfigFile::ToolbarItemsTD::iterator it = m_toolbarItems.begin(); it != m_toolbarItems.end(); ++it)
	{
		addToolbarItem(it->first.c_str(), it->second.first);
	}

	// Try to select the startup type, if it's not there, then just select the first
	if (CB_ERR == ComboBox_SelectString(m_hComboInitialisation, 0, configFile->getSetting(_T("STARTUP")).c_str()))
	{
		ComboBox_SetCurSel(m_hComboInitialisation, 0);
	}

	bool preferInstallPython = (configFile->getSetting(_T("PREFERINSTALLEDPYTHON")) == _T("1"));
	CheckDlgButton(_hSelf, IDC_CHECKPREFERINSTALLEDPYTHON, preferInstallPython ? BST_CHECKED : BST_UNCHECKED);

	bool addExtraLine = (configFile->getSetting(_T("ADDEXTRALINETOOUTPUT")) == _T("1"));
	CheckDlgButton(_hSelf, IDC_CHECKADDEXTRALINETOOUTPUT, addExtraLine ? BST_CHECKED : BST_UNCHECKED);

	bool colorOutput = (configFile->getSetting(_T("COLORIZEOUTPUT")) >= _T("0"));
	CheckDlgButton(_hSelf, IDC_CHECKCOLORIZEOUTPUT, colorOutput ? BST_CHECKED : BST_UNCHECKED);
	EnableWindow(m_hButtonColor, colorOutput);

	bool openOnError = (configFile->getSetting(_T("OPENCONSOLEONERROR")) == _T("1"));
	CheckDlgButton(_hSelf, IDC_CHECKOPENCONSOLEONERROR, openOnError ? BST_CHECKED : BST_UNCHECKED);
	
	bool disablePopupWarning = (configFile->getSetting(_T("DISABLEPOPUPWARNING")) == _T("1"));
	CheckDlgButton(_hSelf, IDC_DISABLEPOPUPWARNING, disablePopupWarning ? BST_CHECKED : BST_UNCHECKED);

}



void ShortcutDlg::saveConfig()
{
	ConfigFile* configFile = ConfigFile::getInstance();
	configFile->clearItems();

	for (ConfigFile::MenuItemsTD::iterator it = m_menuItems.begin(); it != m_menuItems.end(); ++it)
	{
		configFile->addMenuItem(*it);
	}



	for (ConfigFile::ToolbarItemsTD::iterator it = m_toolbarItems.begin(); it != m_toolbarItems.end(); ++it)
	{
		configFile->addToolbarItem(it->first, it->second.second);
	}

	TCHAR startupBuffer[50];
	ComboBox_GetText(m_hComboInitialisation, startupBuffer, 50);
	configFile->setSetting(_T("STARTUP"), startupBuffer);

	bool preferInstallPython = (BST_CHECKED == IsDlgButtonChecked(_hSelf, IDC_CHECKPREFERINSTALLEDPYTHON));
	configFile->setSetting(_T("PREFERINSTALLEDPYTHON"), preferInstallPython ? _T("1") : _T("0"));

	bool addExtraLine = (BST_CHECKED == IsDlgButtonChecked(_hSelf, IDC_CHECKADDEXTRALINETOOUTPUT));
	configFile->setSetting(_T("ADDEXTRALINETOOUTPUT"), addExtraLine ? _T("1"): _T("0"));

	bool colorOutput = (BST_CHECKED == IsDlgButtonChecked(_hSelf, IDC_CHECKCOLORIZEOUTPUT));
	configFile->setSetting(_T("COLORIZEOUTPUT"), colorOutput ? ConfigFile::getInstance()->getSetting(_T("COLORIZEOUTPUT")) : _T("-1"));

	bool openOnError = (BST_CHECKED == IsDlgButtonChecked(_hSelf, IDC_CHECKOPENCONSOLEONERROR));
	configFile->setSetting(_T("OPENCONSOLEONERROR"), openOnError ? _T("1") : _T("0"));
	
	bool disablePopupWarning = (BST_CHECKED == IsDlgButtonChecked(_hSelf, IDC_DISABLEPOPUPWARNING));
	configFile->setSetting(_T("DISABLEPOPUPWARNING"), disablePopupWarning ? _T("1") : _T("0"));

	configFile->save();
}


void ShortcutDlg::toolbarSetIcon()
{
	int index = ListView_GetNextItem(m_hListToolbarItems, -1, LVIS_SELECTED);

	if (index != -1)
	{
		OPENFILENAME ofn;
		memset(&ofn, 0, sizeof(OPENFILENAME));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = _hSelf;

		tstring configDir(ConfigFile::getInstance()->getConfigDir());
		configDir.append(_T("\\PythonScript\\icons"));
		ofn.lpstrInitialDir = configDir.c_str();
		//ofn.lpstrFileTitle = "Choose filename for new script";
		ofn.lpstrFile = new TCHAR[MAX_PATH];
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = MAX_PATH;
		//lint -e840 Use of nul character in a string literal
		// This is how it's meant to be used.
		ofn.lpstrFilter = _T("Icons (*.ico, *.bmp)\0*.ico;*.bmp\0All Files (*.*)\0*.*\0");
		//lint +e840
		ofn.nFilterIndex = 1;

		ofn.Flags = OFN_FILEMUSTEXIST;


		if (GetOpenFileName(&ofn))
		{
			ConfigFile::ToolbarItemsTD::iterator it = m_toolbarItems.begin() + index;
			HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, ofn.lpstrFile, IMAGE_BITMAP, 16, 16, LR_COLOR | LR_LOADFROMFILE);
			if (hBitmap == NULL) {
				hBitmap = ConvertIconToBitmap(ofn.lpstrFile);
			}
			it->second.first = hBitmap;
			it->second.second = ofn.lpstrFile;

			int imageIndex = ImageList_Add(m_hImageList, it->second.first, NULL);
			LVITEM lvItem{};
			lvItem.mask = LVIF_IMAGE;
			lvItem.iItem = index;
			lvItem.iImage = imageIndex;
			ListView_SetItem(m_hListToolbarItems, &lvItem);
		}
	}
}

void ShortcutDlg::ctrlOnClick() const
{
	CHOOSECOLOR cc;
	static COLORREF acrCustClr[16];
	for (int i = 0; i < 16; i++)
	{
		acrCustClr[i] = RGB(255,255,255);
	}
	const tstring strRGBCurrent = ConfigFile::getInstance()->getSetting(_T("COLORIZEOUTPUT"));
	static DWORD rgbCurrent = (strRGBCurrent == _T("-1")) ? RGB(135,214,18) : stoi(strRGBCurrent);

	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = _hSelf;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.rgbResult = rgbCurrent;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc) == TRUE)
	{
		rgbCurrent = cc.rgbResult;
		ConfigFile::getInstance()->setSetting(_T("COLORIZEOUTPUT"), std::to_wstring(rgbCurrent));
	}

}