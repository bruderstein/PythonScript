#include "StdAfx.h"
#include "AboutDialog.h"
#include "resource.h" 

AboutDialog::AboutDialog(void)
{
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