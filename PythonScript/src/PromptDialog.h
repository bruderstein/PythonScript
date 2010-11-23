#ifndef _PROMPTDIALOG_H
#define _PROMPTDIALOG_H

class PromptDialog
{
public:
	enum PROMPT_RESULT
	{
		RESULT_OK,
		RESULT_CANCEL
	};

	PromptDialog(HINSTANCE hInst, HWND hNotepad);
	~PromptDialog();

	PROMPT_RESULT showPrompt(const char *prompt, const char *title, const char *initial);
	const std::string& getText() {	return m_value; }

	static BOOL CALLBACK dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


protected:
	virtual BOOL CALLBACK runDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


	HINSTANCE m_hInst;
	HWND m_hNotepad;

private:
	PromptDialog(); // default constructor disabled

	PROMPT_RESULT m_result;
	std::string m_value;
	std::string m_prompt;
	std::string m_title;
	std::string m_initial;
	HWND m_hSelf;
};

#endif
