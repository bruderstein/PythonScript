#ifndef _CONSOLEDIALOG_H
#define _CONSOLEDIALOG_H

#include "stdafx.h"
#include "DockingDlgInterface.h"
#include "PluginInterface.h"

void export_console();

class ConsoleInterface;

class ConsoleDialog : DockingDlgInterface
{
public:
	ConsoleDialog();
	~ConsoleDialog();
	
	void init(HINSTANCE hInst, NppData nppData, ConsoleInterface *console);

    void doDialog();
    BOOL CALLBACK run_dlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	
	void writeText(int length, const char *text);
	
	void setPrompt(const char *prompt);
	HWND getScintillaHwnd() { return m_scintilla; };

	void runEnabled(bool enabled);

private:
	void createOutputWindow(HWND hParentWindow);
	void runStatement();
	void stopStatement();

	LRESULT run_inputWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT inputWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void historyNext();
	void historyPrevious();
	void historyAdd(const char *line);
	void historyEnd();

	//HWND m_hNpp;
	tTbData m_data;
	HWND m_scintilla;
	HWND m_hInput;  // Input TextBox
	ConsoleInterface *m_console;
	std::string m_prompt;
	WNDPROC m_originalInputWndProc;
	HICON m_hTabIcon;

	std::list<std::string> m_history;
	std::list<std::string>::iterator m_historyIter;
	std::map<int, std::string> m_changes;
	int m_currentHistory;
	bool m_runButtonIsRun;

};


#endif