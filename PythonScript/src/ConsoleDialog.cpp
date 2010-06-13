#include "stdafx.h"
#include "ConsoleDialog.h"
#include "Notepad_Plus_msgs.h"
#include "DockingDlgInterface.h"
#include "Scintilla.h"
#include "resource.h"
#include "PythonConsole.h"

ConsoleDialog::ConsoleDialog()
	: DockingDlgInterface(IDD_CONSOLE),
	m_prompt(">>> ")
{
	
}

ConsoleDialog::~ConsoleDialog()
{
}


void ConsoleDialog::init(HINSTANCE hInst, NppData nppData, PythonConsole* console)
{
	DockingDlgInterface::init(hInst, nppData._nppHandle);
	
	Window::init(hInst, nppData._nppHandle);
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
				HFONT hCourier = CreateFont(14,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
					CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, FIXED_PITCH, TEXT("Courier New"));
				if (hCourier != NULL)
					SendMessage(::GetDlgItem(_hSelf, IDC_INPUT), WM_SETFONT, reinterpret_cast<WPARAM>(hCourier), TRUE); 
				return TRUE;
			}
		case WM_SIZE:
			MoveWindow(m_scintilla, 0, 0, LOWORD(lParam), HIWORD(lParam)-30, TRUE);
			MoveWindow(::GetDlgItem(_hSelf, IDC_PROMPT), 0, HIWORD(lParam)-30, 30, 25, TRUE);
			MoveWindow(::GetDlgItem(_hSelf, IDC_INPUT), 30, HIWORD(lParam)-30, LOWORD(lParam) - 85, 25, TRUE);
			MoveWindow(::GetDlgItem(_hSelf, IDC_RUN), LOWORD(lParam) - 50, HIWORD(lParam) - 30, 50, 25, TRUE);  
			// ::SendMessage(m_scintilla, WM_SIZE, 0, MAKEWORD(LOWORD(lParam) - 10, HIWORD(lParam) - 30));
			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDC_RUN)
			{
				runStatement();
				//MessageBox(NULL, _T("Command") , _T("Python Command"), 0);
				return TRUE;
			}
		default:
			return DockingDlgInterface::run_dlgProc(message, wParam, lParam);

	}

	return FALSE;
}


bool ConsoleDialog::runStatement()
{
	char buffer[1000];
	GetWindowTextA(::GetDlgItem(_hSelf, IDC_INPUT), buffer, 1000);
	writeText(m_prompt.size(), m_prompt.c_str());
	writeText(strlen(buffer), buffer);
	writeText(1, "\n");
	SetWindowTextA(::GetDlgItem(_hSelf, IDC_INPUT), "");
	bool more = m_console->runStatement(buffer);
	if (more)
		setPrompt("... ");
	else
		setPrompt(">>> ");
	return more;
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
		

		// define the default docking behaviour
		m_data.uMask			= DWS_DF_CONT_BOTTOM;
		m_data.pszName = new TCHAR[20];
		_tcscpy_s(m_data.pszName, 20, _T("Python"));
		
		m_data.pszAddInfo	    = NULL; //_pExProp->szCurrentPath;
		// m_data.hIconTab		= (HICON)::LoadImage(_hInst, MAKEINTRESOURCE(IDI_EXPLORE), IMAGE_ICON, 0, 0, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);
		m_data.pszModuleName	= getPluginFileName();
		m_data.dlgID			= IDD_CONSOLE;
		create(&m_data);

		::SendMessage(_hParent, NPPM_DMMREGASDCKDLG, 0, (LPARAM)&m_data);
	}

	 display(true);
}


