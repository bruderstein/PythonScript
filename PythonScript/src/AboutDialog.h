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
	
	void initDialog(HINSTANCE hInst, NppData& nppData);

	void doDialog();

protected:
	INT_PTR CALLBACK run_dlgProc(UINT Message, WPARAM wParam, LPARAM lParam);

private:	
	HBRUSH m_hbrBackground;
};

#endif // _ABOUTDIALOG_H
