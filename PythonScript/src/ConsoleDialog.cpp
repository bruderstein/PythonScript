#include "stdafx.h"
#include "ConsoleDialog.h"
#include "Notepad_Plus_msgs.h"
#include "DockingDlgInterface.h"
#include "Scintilla.h"
#include "resource.h"
#include "PythonConsole.h"

using namespace std;

ConsoleDialog::ConsoleDialog()
	: DockingDlgInterface(IDD_CONSOLE),
	m_prompt(">>> "),
	m_scintilla(NULL)
{
	
}

ConsoleDialog::~ConsoleDialog()
{
	if (m_scintilla)
	{
		::SendMessage(_hParent, NPPM_DESTROYSCINTILLAHANDLE, 0, reinterpret_cast<LPARAM>(m_scintilla));
		m_scintilla = NULL;
	}
}


void ConsoleDialog::init(HINSTANCE hInst, NppData nppData, ConsoleInterface* console)
{
	DockingDlgInterface::init(hInst, nppData._nppHandle);
	
	//Window::init(hInst, nppData._nppHandle);
	createOutputWindow(nppData._nppHandle);
	m_console = console;
}

BOOL ConsoleDialog::run_dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			{
				SetParent(m_scintilla, hWnd);
				ShowWindow(m_scintilla, SW_SHOW);
				m_hInput = ::GetDlgItem(_hSelf, IDC_INPUT);
				HFONT hCourier = CreateFont(14,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
					CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, FIXED_PITCH, _T("Courier New"));
				if (hCourier != NULL)
				{
					SendMessage(m_hInput, WM_SETFONT, reinterpret_cast<WPARAM>(hCourier), TRUE); 
					SendMessage(::GetDlgItem(_hSelf, IDC_PROMPT), WM_SETFONT, reinterpret_cast<WPARAM>(hCourier), TRUE); 
				}
				// Subclass the Input box
				::SetWindowLongPtr(::GetDlgItem(_hSelf, IDC_INPUT), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
				m_originalInputWndProc = reinterpret_cast<WNDPROC>(::SetWindowLongPtr(::GetDlgItem(_hSelf, IDC_INPUT), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(ConsoleDialog::inputWndProc)));

				return TRUE;
			}
		case WM_SIZE:
			MoveWindow(m_scintilla, 0, 0, LOWORD(lParam), HIWORD(lParam)-30, TRUE);
			MoveWindow(::GetDlgItem(_hSelf, IDC_PROMPT), 0, HIWORD(lParam)-25, 30, 25, TRUE);
			MoveWindow(m_hInput, 30, HIWORD(lParam)-30, LOWORD(lParam) - 85, 25, TRUE);
			MoveWindow(::GetDlgItem(_hSelf, IDC_RUN), LOWORD(lParam) - 50, HIWORD(lParam) - 30, 50, 25, TRUE);  
			// ::SendMessage(m_scintilla, WM_SIZE, 0, MAKEWORD(LOWORD(lParam) - 10, HIWORD(lParam) - 30));
			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDC_RUN)
			{
				if (m_runButtonIsRun)
				{
					runStatement();
				}
				else
				{
					m_console->stopStatement();
				}
				//MessageBox(NULL, _T("Command") , _T("Python Command"), 0);
				return TRUE;
			}
		default:
			return DockingDlgInterface::run_dlgProc(message, wParam, lParam);

	}

	return FALSE;
}


void ConsoleDialog::historyPrevious()
{
	if (m_currentHistory > 0)
	{
		char buffer[1000];
		GetWindowTextA(m_hInput, buffer, 1000);
		
		// Not an empty string and different from orig
		if (buffer[0] && (m_historyIter == m_history.end() || *m_historyIter != buffer)) 
		{
			if (m_changes.find(m_currentHistory) == m_changes.end())
			{
				m_changes.insert(pair<int, string>(m_currentHistory, string(buffer)));
			}
			else
			{
				m_changes[m_currentHistory] = string(buffer);
			}
		}

		--m_currentHistory;
		--m_historyIter;

		// If there's no changes to the line, just copy the original
		if (m_changes.find(m_currentHistory) == m_changes.end())
		{
			::SetWindowTextA(m_hInput, m_historyIter->c_str());
			::SendMessage(m_hInput, EM_SETSEL, m_historyIter->size(), m_historyIter->size());
		}
		else
		{
			// Set it as the changed string
			::SetWindowTextA(m_hInput, m_changes[m_currentHistory].c_str());
			::SendMessage(m_hInput, EM_SETSEL, m_changes[m_currentHistory].size(),  m_changes[m_currentHistory].size());
		}

	}
}

void ConsoleDialog::historyNext()
{
	if (m_currentHistory < m_history.size())
	{
		char buffer[1000];
		GetWindowTextA(m_hInput, buffer, 1000);


		// Not an empty string and different from orig
		if (buffer[0] && *m_historyIter != buffer) 
		{
			if (m_changes.find(m_currentHistory) == m_changes.end())
			{
				m_changes.insert(pair<int, string>(m_currentHistory, string(buffer)));
			}
			else
			{
				m_changes[m_currentHistory] = string(buffer);
			}
		}

		++m_currentHistory;
		++m_historyIter;

		// If there's no changes to the line, just copy the original
		if (m_changes.find(m_currentHistory) == m_changes.end())
		{
			if (m_historyIter != m_history.end())
			{
				::SetWindowTextA(m_hInput, m_historyIter->c_str());
				::SendMessage(m_hInput, EM_SETSEL, m_historyIter->size(), m_historyIter->size());
			}
			else
			{
				::SetWindowTextA(m_hInput, "");
			}
		}
		else
		{
			// Set it as the changed string
			::SetWindowTextA(m_hInput, m_changes[m_currentHistory].c_str());
			::SendMessage(m_hInput, EM_SETSEL, m_changes[m_currentHistory].size(), m_changes[m_currentHistory].size());

		}

	}
}


void ConsoleDialog::historyAdd(const char *line)
{
	if (line && line[0])
	{
		m_history.push_back(string(line));
		m_currentHistory = m_history.size();
	}

	m_historyIter = m_history.end();
	m_changes.clear();
}

void ConsoleDialog::historyEnd()
{
	m_currentHistory = m_history.size();
	m_historyIter = m_history.end();
	::SetWindowTextA(m_hInput, "");
}


LRESULT ConsoleDialog::inputWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ConsoleDialog *dlg = reinterpret_cast<ConsoleDialog*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return dlg->run_inputWndProc(hWnd, message, wParam, lParam);
}

LRESULT ConsoleDialog::run_inputWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch(message)
	{
		case WM_KEYDOWN:
			switch(wParam)
			{
				case VK_UP:
					historyPrevious();
					return FALSE;

				case VK_DOWN:
					historyNext();
					return FALSE;


				default:
					return CallWindowProc(m_originalInputWndProc, hWnd, message, wParam, lParam);
			}
			break;

		case WM_KEYUP:
			switch(wParam)
			{
				case VK_RETURN:
					runStatement();
					return FALSE;

				case VK_ESCAPE:
					historyEnd();
					return FALSE;

				default:
					return CallWindowProc(m_originalInputWndProc, hWnd, message, wParam, lParam);
			}
			break;

		default:
			return CallWindowProc(m_originalInputWndProc, hWnd, message, wParam, lParam);
	}

}

void ConsoleDialog::runStatement()
{
	char buffer[1000];
	GetWindowTextA(::GetDlgItem(_hSelf, IDC_INPUT), buffer, 1000);
	historyAdd(buffer);
	writeText(m_prompt.size(), m_prompt.c_str());
	writeText(strlen(buffer), buffer);
	writeText(1, "\n");
	SetWindowTextA(::GetDlgItem(_hSelf, IDC_INPUT), "");
	m_console->runStatement(buffer);

}


void ConsoleDialog::stopStatement()
{
	m_console->stopStatement();
}


void ConsoleDialog::setPrompt(const char *prompt)
{
	m_prompt = prompt;
	::SetWindowTextA(::GetDlgItem(_hSelf, IDC_PROMPT), prompt);
}


void ConsoleDialog::createOutputWindow(HWND hParentWindow)
{
	m_scintilla = (HWND)::SendMessage(_hParent, NPPM_CREATESCINTILLAHANDLE, 0, reinterpret_cast<LPARAM>(hParentWindow));
	::SendMessage(m_scintilla, SCI_SETREADONLY, 1, 0);
	::SendMessage(m_scintilla, SCI_STYLESETSIZE, 0 /* = style number */, 8 /* = size in points */);   
	::SendMessage(m_scintilla, SCI_STYLESETSIZE, 1 /* = style number */, 8 /* = size in points */);   
}

void ConsoleDialog::writeText(int length, const char *text)
{
	::SendMessage(m_scintilla, SCI_SETREADONLY, 0, 0);
	::SendMessage(m_scintilla, SCI_APPENDTEXT, length, reinterpret_cast<LPARAM>(text));
	::SendMessage(m_scintilla, SCI_SETREADONLY, 1, 0);
	
	
	::SendMessage(m_scintilla, SCI_GOTOPOS, ::SendMessage(m_scintilla, SCI_GETLENGTH, 0, 0), 0);
	
}


void ConsoleDialog::doDialog()
{
	 if (!isCreated())
	{
		create(&m_data);

		// define the default docking behaviour
		m_data.uMask			= DWS_DF_CONT_BOTTOM | DWS_ICONTAB;
		m_data.pszName = new TCHAR[20];
		_tcscpy_s(m_data.pszName, 20, _T("Python"));
		
		RECT rc;
		rc.bottom = 0;
		rc.top = 0;
		rc.left = 0;
		rc.right = 0;
		m_hTabIcon = (HICON)::LoadImage(_hInst, MAKEINTRESOURCE(IDI_PYTHON), IMAGE_ICON, 16, 16, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);
		m_data.hIconTab			= m_hTabIcon;
		m_data.pszModuleName	= _T("Python Script");
		m_data.dlgID			= -1; /* IDD_CONSOLE */
		m_data.pszAddInfo	    = NULL; //_pExProp->szCurrentPath;
		m_data.iPrevCont		= -1;
		m_data.hClient			= _hSelf;
		m_data.rcFloat			= rc;


		::SendMessage(_hParent, NPPM_DMMREGASDCKDLG, 0, (LPARAM)&m_data);
	}

	 display(true);
}

void ConsoleDialog::runEnabled(bool enabled)
{
	//EnableWindow(GetDlgItem(_hSelf, IDC_RUN), enabled);
	::SetWindowText(GetDlgItem(_hSelf, IDC_RUN), enabled ? _T("Run") : _T("Stop"));
	m_runButtonIsRun = enabled;

	if (enabled)
	{
		::SetForegroundWindow(_hSelf);
		//::SetActiveWindow(_hSelf);
		::SetFocus(m_hInput);
	}
}

void ConsoleDialog::clearText()
{
	::SendMessage(m_scintilla, SCI_SETREADONLY, 0, 0);
	::SendMessage(m_scintilla, SCI_CLEARALL, 0, 0);
	::SendMessage(m_scintilla, SCI_SETREADONLY, 1, 0);
}