#ifndef _CONSOLEDIALOG_H
#define _CONSOLEDIALOG_H

#ifndef DOCKINGDLGINTERFACE_H
#include "DockingDlgInterface.h"
#endif

struct SCNotification;
struct NppData;
class ConsoleInterface;
struct LineDetails;

class ConsoleDialog : public DockingDlgInterface
{
public:
	ConsoleDialog();
	ConsoleDialog(const ConsoleDialog& other);
	~ConsoleDialog();

	
	void initDialog(HINSTANCE hInst, NppData& nppData, ConsoleInterface *console);

    void doDialog();
	void hide();

	void writeText(size_t length, const char *text);
	void writeError(size_t length, const char *text);
	void clearText();
	void setPrompt(const char *prompt);
	HWND getScintillaHwnd() { return m_scintilla; };

	void runEnabled(bool enabled);
	
protected:
	BOOL CALLBACK run_dlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	
private:
	ConsoleDialog& operator = (const ConsoleDialog&); // assignment operator disabled

	void createOutputWindow(HWND hParentWindow);
	void runStatement();
	void stopStatement();
	
	LRESULT run_inputWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT inputWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void historyNext();
	void historyPrevious();
	void historyAdd(const TCHAR *line);
	void historyEnd();

	LRESULT callScintilla(UINT message, WPARAM wParam = 0, LPARAM lParam = 0)
	{	return ::SendMessage(m_scintilla, message, wParam, lParam); }

	/* Styler functions */
	void onStyleNeeded(SCNotification* notification);
	bool parsePythonErrorLine(LineDetails *lineDetails);
	bool parseVSErrorLine(LineDetails *lineDetails);
	bool parseGCCErrorLine(LineDetails *lineDetails);
	//void styleDefaultLine(int lineNumber, int lineLength, const char *line);
	void onHotspotClick(SCNotification* notification);
	bool parseLine(LineDetails *lineDetails);

	bool isValidFilenameChar(char ch)
	{	return (ch != '<' && ch != '>' && ch != ':' && ch != '|' && ch != '\"' && ch != '?'); };

	//HWND m_hNpp;
	tTbData* m_data;
	HWND m_scintilla;
	HWND m_hInput;  // Input TextBox
	ConsoleInterface *m_console;
	std::string m_prompt;
	WNDPROC m_originalInputWndProc;
	HICON m_hTabIcon;

	std::list<tstring> m_history;
	std::list<tstring>::iterator m_historyIter;
	std::map<idx_t, tstring> m_changes;
	idx_t m_currentHistory;
	bool m_runButtonIsRun;

	HMENU m_hContext;
	
};

enum ErrorLevel
{
	EL_UNSET,
	EL_INFO,
	EL_WARNING,
	EL_ERROR
};

struct LineDetails
{
public:
	char *line;
	size_t lineLength;
	idx_t errorLineNo;
	idx_t filenameStart;
	idx_t filenameEnd;
	ErrorLevel errorLevel;
};

#endif
