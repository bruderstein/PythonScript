#ifndef _CONSOLEDIALOG_H
#define _CONSOLEDIALOG_H

#ifndef DOCKINGDLGINTERFACE_H
#include "DockingDlgInterface.h"
#endif

struct SCNotification;
struct NppData;

namespace NppPythonScript
{

class ConsoleInterface;
struct LineDetails;

class ConsoleDialog : public DockingDlgInterface
{
public:
	ConsoleDialog();
	ConsoleDialog(const ConsoleDialog& other) = delete;
	~ConsoleDialog();


	void initDialog(HINSTANCE hInst, NppData& nppData, ConsoleInterface *console);

    void doDialog();
	void hide();

	void writeCmdText(size_t length, const char *text);
	void writeText(size_t length, const char *text);
	void writeColoredText(size_t length, const char *text);
	void writeError(size_t length, const char *text);
	void clearText();
	void setPrompt(std::string prompt);
	std::string getStandardPrompt();
	std::string getContinuePrompt();
	HWND getScintillaHwnd() { return m_scintilla; }

	void giveInputFocus() { SetFocus(m_hInput); }

	void runEnabled(bool enabled);

	NppData m_nppData;

protected:
	virtual INT_PTR CALLBACK run_dlgProc(UINT Message, WPARAM wParam, LPARAM lParam);

private:
	ConsoleDialog& operator = (const ConsoleDialog&); // assignment operator disabled

	void createOutputWindow(HWND hParentWindow);
	void runStatement();
	void stopStatement();

	LRESULT run_inputWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT inputWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT scintillaWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


	void historyNext();
	void historyPrevious();
	void historyAdd(const TCHAR *line);
	void historyEnd();
	void selectComboboxItem();

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
	static WNDPROC s_originalScintillaWndProc;
	HWND m_hCombo; // ComboBox
	HWND m_hInput;  // Input ComboBox
	ConsoleInterface *m_console;
	std::string m_standardPrompt;
	std::string m_continuePrompt;
	std::string m_currentPrompt;
	WNDPROC m_originalInputWndProc;
	HICON m_hTabIcon;

	bool m_runButtonIsRun;

	HMENU m_hContext;
	bool m_colorOutput;
	int m_user_color;

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

}

#endif
