#include "stdafx.h"
#include "AboutDialog.h"
#include "PythonScriptVersion.h"
#include "resource.h" 
#include "PluginInterface.h"

AboutDialog::AboutDialog(void)
{
	m_hbrBackground = CreateSolidBrush(RGB(255,255,255));
}

AboutDialog::~AboutDialog(void)
{
	if (m_hbrBackground)
	{
		DeleteObject(m_hbrBackground);
		m_hbrBackground = NULL;
	}
}

void AboutDialog::doDialog()
{
	if (!isCreated())
		create(IDD_ABOUTDLG);
	
	goToCenter();
}


INT_PTR CALLBACK AboutDialog::run_dlgProc(UINT Message, WPARAM wParam, LPARAM /* lParam */)
{
	switch (Message) 
	{
		case WM_INITDIALOG :
		{
			std::string message("Python ");
			message.append(Py_GetVersion());
			message.append("\n");
			message.append(Py_GetCopyright());
			
			for (size_t pos = 0;(pos = message.find("\n", pos + 1)) != std::string::npos;)
			{
				message.replace(pos, 1, "\r\n");
				++pos;
			}

			::SetWindowTextA(GetDlgItem(_hSelf, IDC_COPYRIGHT), message.c_str());
			::SetWindowText(GetDlgItem(_hSelf, IDC_VERSION), _T(PYSCR_VERSION_STRING));
			return TRUE;
		}
		
		case WM_CTLCOLORDLG:
			return (LONG)m_hbrBackground;

		case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wParam;
			SetBkMode(hdcStatic, TRANSPARENT);
			return (LONG)m_hbrBackground;
		}

		case WM_COMMAND : 
			switch (wParam)
			{
				case IDOK :
				case IDCANCEL :
					display(false);
					return TRUE;

				default :
					break;
			}
			return FALSE;
		
		default:
			return FALSE;
	}


}

void AboutDialog::initDialog( HINSTANCE hInst, NppData& nppData )
{
	Window::init(hInst, nppData._nppHandle);
}
