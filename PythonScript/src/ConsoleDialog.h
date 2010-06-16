#ifndef _CONSOLEDIALOG_H
#define _CONSOLEDIALOG_H

#include "stdafx.h"
#include "DockingDlgInterface.h"
#include "PluginInterface.h"

void export_console();

class PythonConsole;

class ConsoleDialog : DockingDlgInterface
{
public:
	ConsoleDialog();
	~ConsoleDialog();
	
	void init(HINSTANCE hInst, NppData nppData, PythonConsole *console);

    void doDialog();
    BOOL CALLBACK run_dlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	
	void writeText(int length, const char *text);
	
	void setPrompt(const char *prompt);
	HWND getScintillaHwnd() { return m_scintilla; };
private:
	void createOutputWindow(HWND hParentWindow);
	bool runStatement();
	
	//HWND m_hNpp;
	tTbData m_data;
	HWND m_scintilla;
	PythonConsole *m_console;
	std::string m_prompt;
};


#endif