#include "stdafx.h"

#include "PromptDialog.h"
#include "resource.h"
#include "Notepad_Plus_msgs.h"
#include "WcharMbcsConverter.h"

PromptDialog::PromptDialog(HINSTANCE hInst, HWND hNotepad)
	: m_hInst(hInst),
	  m_hNotepad(hNotepad),
	  m_result(RESULT_CANCEL),
	  m_hSelf(NULL)
{
}

PromptDialog::~PromptDialog()
{
	m_hInst = NULL;
	m_hNotepad = NULL;
	// m_hSelf is handed down to us and not created by us. Therefore, let's just NULL the variable.
	m_hSelf = NULL;
}

PromptDialog::PROMPT_RESULT PromptDialog::showPrompt(const char *prompt, const char *title, const char *initial)
{
	m_result = RESULT_CANCEL;
	m_prompt = prompt;
	m_title = title;
	if (initial)
		m_initial = initial;

	DialogBoxParam(m_hInst, MAKEINTRESOURCE(IDD_PROMPTDIALOG), m_hNotepad, PromptDialog::dlgProc, reinterpret_cast<LPARAM>(this));
	return m_result;
}

INT_PTR CALLBACK PromptDialog::dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
		{
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, static_cast<LONG_PTR>(lParam));
			PromptDialog* dlg = reinterpret_cast<PromptDialog*>(lParam);
			return dlg->runDlgProc(hWnd, message, wParam, lParam);
		}
		default:
		{
			PromptDialog* dlg = reinterpret_cast<PromptDialog*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
			if (dlg)
				return dlg->runDlgProc(hWnd, message, wParam, lParam);
			else
				return TRUE;
		}
	}
}

INT_PTR CALLBACK PromptDialog::runDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM /* lParam */)
{
	switch(message)
	{
		case WM_INITDIALOG:
		{
			SendMessage(m_hNotepad, NPPM_MODELESSDIALOG, MODELESSDIALOGADD, reinterpret_cast<LPARAM>(hWnd));
			m_hSelf = hWnd;
			::SetWindowText(::GetDlgItem(m_hSelf, IDC_PROMPT), WcharMbcsConverter::char2tchar(m_prompt.c_str()).get());
			::SetWindowText(m_hSelf, WcharMbcsConverter::char2tchar(m_title.c_str()).get());
			::SetWindowText(::GetDlgItem(m_hSelf, IDC_USERTEXT), WcharMbcsConverter::char2tchar(m_initial.c_str()).get());
			::SendMessage(::GetDlgItem(m_hSelf, IDC_USERTEXT), EM_SETSEL, 0, -1);
			//disable selection and set cursor to end of text also if scrolling is necessary for long text
			::SendMessage(::GetDlgItem(m_hSelf, IDC_USERTEXT), EM_SETSEL, static_cast<WPARAM>(-1), -1);
			::SendMessage(::GetDlgItem(m_hSelf, IDC_USERTEXT), EM_SCROLLCARET, 0, 0);
			SetFocus(::GetDlgItem(m_hSelf, IDC_USERTEXT));

			RECT rc;
			::GetClientRect(m_hNotepad, &rc);
			POINT center{};
			center.x = rc.left + (rc.right - rc.left)/2;
			center.y = rc.top + (rc.bottom - rc.top)/2;
			::ClientToScreen(m_hNotepad, &center);
			RECT promptRC;
			::GetWindowRect(m_hSelf, &promptRC);
			int x = center.x - (promptRC.right - promptRC.left)/2;
			int y = center.y - (promptRC.bottom - promptRC.top)/2;

			::SetWindowPos(m_hSelf, HWND_TOP, x, y, promptRC.right - promptRC.left, promptRC.bottom - promptRC.top, 0);

			return FALSE;
		}

		case WM_COMMAND:
		{
			switch(wParam)
			{
				case IDOK:
					{
						TCHAR buffer[1000];
						::GetWindowText(::GetDlgItem(m_hSelf, IDC_USERTEXT), buffer, 1000);
						m_value = WcharMbcsConverter::tchar2char(buffer).get();
						m_result = RESULT_OK;
					}
					//lint -fallthrough

				case IDCANCEL:
					::EndDialog(m_hSelf, 0);
					SendMessage(m_hNotepad, NPPM_MODELESSDIALOG, MODELESSDIALOGREMOVE, reinterpret_cast<LPARAM>(hWnd));
					return TRUE;

				default:
					return FALSE;
			}
		}
		default:
			return FALSE;
	}
}
