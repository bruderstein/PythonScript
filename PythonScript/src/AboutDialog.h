#ifndef _ABOUTDIALOG_H
#define _ABOUTDIALOG_H

#ifndef STATIC_DIALOG_H
#include "StaticDialog.h"
#endif

struct NppData;

class AboutDialog : public StaticDialog
{
public:
	AboutDialog(void);
	~AboutDialog(void);
	
	void init(HINSTANCE hInst, NppData& nppData);

	void doDialog();
	BOOL CALLBACK AboutDialog::run_dlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

private:	
	HBRUSH m_hbrBackground;
};

#endif // _ABOUTDIALOG_H