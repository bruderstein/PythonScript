#include "StdAfx.h"
#include "AboutDialog.h"
#include "resource.h" 

AboutDialog::AboutDialog(void)
{
	m_hbrBackground = CreateSolidBrush(RGB(255,255,255));
}

AboutDialog::~AboutDialog(void)
{
}

void AboutDialog::doDialog()
{
    if (!isCreated())
        create(IDD_ABOUTDLG);
	
	goToCenter();
}


BOOL CALLBACK AboutDialog::run_dlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message) 
	{
        case WM_INITDIALOG :
		{
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
		{
			
				
				switch (wParam)
				{
					case IDOK :
					case IDCANCEL :
						display(FALSE);
						return TRUE;

					default :
						break;
				}
			
		}
	}
	return FALSE;
}