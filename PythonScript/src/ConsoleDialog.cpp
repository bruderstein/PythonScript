#include "stdafx.h"
#include "ConsoleDialog.h"
#include "Scintilla.h"
#include "SciLexer.h"
#include "resource.h"
#include "PythonConsole.h"
#include "Notepad_plus_msgs.h"
#include "PluginInterface.h"
#include "Docking.h"

ConsoleDialog::ConsoleDialog() :
	DockingDlgInterface(IDD_CONSOLE),
	m_data(new tTbData),
    m_scintilla(NULL),
	m_hInput(NULL),
	m_console(NULL),
	m_prompt(">>> "),
	m_originalInputWndProc(NULL),
	m_hTabIcon(NULL),
	m_currentHistory(0),
	m_runButtonIsRun(true),
	m_hContext(NULL)
{
    m_historyIter = m_history.end();
}

//lint -e1554  Direct pointer copy of member 'name' within copy constructor: 'ConsoleDialog::ConsoleDialog(const ConsoleDialog &)')
// We indeed copy pointers, and it's okay. These are not allocated within the 
// scope of this class but rather passed in and copied anyway.
ConsoleDialog::ConsoleDialog(const ConsoleDialog& other) :
	DockingDlgInterface(other),
	m_data(other.m_data ? new tTbData(*other.m_data) : NULL),
	m_scintilla(other.m_scintilla),
	m_hInput(other.m_hInput),
	m_console(other.m_console),
	m_prompt(other.m_prompt),
	m_originalInputWndProc(other.m_originalInputWndProc),
	m_hTabIcon(other.m_hTabIcon),
	m_history(other.m_history),
	m_historyIter(other.m_historyIter),
	m_changes(other.m_changes),
	m_currentHistory(other.m_currentHistory),
	m_runButtonIsRun(other.m_runButtonIsRun),
	m_hContext(other.m_hContext)
{
}
//lint +e1554

ConsoleDialog::~ConsoleDialog()
{
    if (m_scintilla)
    {
        ::SendMessage(_hParent, NPPM_DESTROYSCINTILLAHANDLE, 0, reinterpret_cast<LPARAM>(m_scintilla));
        m_scintilla = NULL;
    }

	if (m_data)
	{
		delete m_data;
	}
}


void ConsoleDialog::initDialog(HINSTANCE hInst, NppData& nppData, ConsoleInterface* console)
{
    DockingDlgInterface::init(hInst, nppData._nppHandle);
    
    //Window::init(hInst, nppData._nppHandle);
    createOutputWindow(nppData._nppHandle);
    m_console = console;
	m_hContext = CreatePopupMenu();
	MENUITEMINFO mi;
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_ID | MIIM_STRING;
	mi.fType = MFT_STRING;
	mi.fState = MFS_ENABLED;
	
	mi.wID = 1;
	mi.dwTypeData = _T("Select all");
	InsertMenuItem(m_hContext, 0, TRUE, &mi);

	mi.wID = 2;
	mi.dwTypeData = _T("Copy");
	InsertMenuItem(m_hContext, 1, TRUE, &mi);

	mi.wID = 3;
	mi.dwTypeData = _T("Clear");
	InsertMenuItem(m_hContext, 3, TRUE, &mi);

}

BOOL CALLBACK ConsoleDialog::run_dlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
            return FALSE;

		case WM_CONTEXTMENU:
			{
				MENUITEMINFO mi;
				mi.cbSize = sizeof(mi);
				mi.fMask = MIIM_STATE;
				if (0 == (callScintilla(SCI_GETSELECTIONSTART) - callScintilla(SCI_GETSELECTIONEND)))
				{
					mi.fState = MFS_DISABLED;
				}
				else
				{
					mi.fState = MFS_ENABLED;
				}
				
				SetMenuItemInfo(m_hContext, 2, FALSE, &mi);

				// Thanks MS for corrupting the value of BOOL. :-/
				// From the documentation (http://msdn.microsoft.com/en-us/library/ms648002.aspx):
				//
				//     If you specify TPM_RETURNCMD in the uFlags parameter, the return value is the menu-item 
				//     identifier of the item that the user selected. If the user cancels the menu without making 
				//     a selection, or if an error occurs, then the return value is zero.
				INT cmdID = (INT)TrackPopupMenu(m_hContext, TPM_RETURNCMD, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, _hSelf, NULL);

				switch(cmdID)
				{
					case 1: // Select All
						callScintilla(SCI_SELECTALL);
						break;

					case 2: // Copy
						callScintilla(SCI_COPY);
						break;

					case 3: // Clear
						clearText();
						break;

					default:
						break;
				}
			}
			break;
        case WM_COMMAND:
            if (LOWORD(wParam) == IDC_RUN)
            {
                if (m_runButtonIsRun)
                {
                    runStatement();
                }
                else
                {
					assert(m_console != NULL);
                    if (m_console)
                    {
						m_console->stopStatement();
                    }
                }
                //MessageBox(NULL, _T("Command") , _T("Python Command"), 0);
                return FALSE;
            }
            break;

        case WM_NOTIFY:
            {
                LPNMHDR nmhdr = reinterpret_cast<LPNMHDR>(lParam);
                if (m_scintilla == nmhdr->hwndFrom)
                {
                    switch(nmhdr->code)
                    {
                        case SCN_STYLENEEDED:
                            onStyleNeeded(reinterpret_cast<SCNotification*>(lParam));
                            return FALSE;

                        case SCN_HOTSPOTCLICK:
                            onHotspotClick(reinterpret_cast<SCNotification*>(lParam));
							return FALSE;

						default:
							break;
                    }
                }
                break;
            }
        default:
            break;

    }

    return DockingDlgInterface::run_dlgProc(message, wParam, lParam);

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
                m_changes.insert(std::pair<int, std::string>(m_currentHistory, std::string(buffer)));
            }
            else
            {
                m_changes[m_currentHistory] = std::string(buffer);
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
    if (static_cast<size_t>(m_currentHistory) < m_history.size())
    {
        char buffer[1000];
        GetWindowTextA(m_hInput, buffer, 1000);


        // Not an empty string and different from orig
        if (buffer[0] && *m_historyIter != buffer) 
        {
            if (m_changes.find(m_currentHistory) == m_changes.end())
            {
                m_changes.insert(std::pair<int, std::string>(m_currentHistory, std::string(buffer)));
            }
            else
            {
                m_changes[m_currentHistory] = std::string(buffer);
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
        m_history.push_back(std::string(line));
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

        default:
            return CallWindowProc(m_originalInputWndProc, hWnd, message, wParam, lParam);
    }
}

void ConsoleDialog::runStatement()
{
	assert(m_console != NULL);
	if (m_console)
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
}


void ConsoleDialog::stopStatement()
{
	assert(m_console != NULL);
	if (m_console)
	{
		m_console->stopStatement();
	}
}


void ConsoleDialog::setPrompt(const char *prompt)
{
    m_prompt = prompt;
    ::SetWindowTextA(::GetDlgItem(_hSelf, IDC_PROMPT), prompt);
}


void ConsoleDialog::createOutputWindow(HWND hParentWindow)
{
    m_scintilla = (HWND)::SendMessage(_hParent, NPPM_CREATESCINTILLAHANDLE, 0, reinterpret_cast<LPARAM>(hParentWindow));
    callScintilla(SCI_SETREADONLY, 1, 0);

	/*  Style bits
	 *  LSB  0 - stderr = 1
	 *       1 - hotspot 
	 *       2 - warning
	 *       ... to be continued
	 */


	// 0 is stdout, black text
    callScintilla(SCI_STYLESETSIZE, 0 /* = style number */, 8 /* = size in points */);   

	// 1 is stderr, red text
    callScintilla(SCI_STYLESETSIZE, 1 /* = style number */, 8 /* = size in points */);   
    callScintilla(SCI_STYLESETFORE, 1, RGB(250, 0, 0));

	// 2 is stdout, black text, underline hotspot
    callScintilla(SCI_STYLESETSIZE, 2 /* = style number */, 8 /* = size in points */);   
    callScintilla(SCI_STYLESETUNDERLINE, 2 /* = style number */, 1 /* = underline */);   
	callScintilla(SCI_STYLESETHOTSPOT, 2, 1);

	// 3 is stderr, red text, underline hotspot
    callScintilla(SCI_STYLESETSIZE, 3 /* = style number */, 8 /* = size in points */);   
    callScintilla(SCI_STYLESETFORE, 3, RGB(250, 0, 0));
    callScintilla(SCI_STYLESETUNDERLINE, 3 /* = style number */, 1 /* = underline */);   
    callScintilla(SCI_STYLESETHOTSPOT, 3, 1);
	
	// 4 stdout warning without hotspot
	callScintilla(SCI_STYLESETSIZE, 4 /* = style number */, 8 /* = size in points */);   
    callScintilla(SCI_STYLESETFORE, 4, RGB(199, 175, 7));  // mucky yellow
    

	// 5 stderr warning without hotspot
	callScintilla(SCI_STYLESETSIZE, 5 /* = style number */, 8 /* = size in points */);   
    callScintilla(SCI_STYLESETFORE, 5, RGB(255, 128, 64));  // orange
    

	// 6 is hotspot, stdout, warning
	callScintilla(SCI_STYLESETSIZE, 6 /* = style number */, 8 /* = size in points */);   
    callScintilla(SCI_STYLESETFORE, 6, RGB(199, 175, 7));  // mucky yellow
    callScintilla(SCI_STYLESETUNDERLINE, 6 /* = style number */, 1 /* = underline */);   
	callScintilla(SCI_STYLESETHOTSPOT, 6, 1);

	// 7 is hotspot, stderr, warning
	callScintilla(SCI_STYLESETSIZE, 7 /* = style number */, 8 /* = size in points */);   
    callScintilla(SCI_STYLESETFORE, 7, RGB(255, 128, 64));  // orange
    callScintilla(SCI_STYLESETUNDERLINE, 7 /* = style number */, 1 /* = underline */);   
	callScintilla(SCI_STYLESETHOTSPOT, 7, 1);

	callScintilla(SCI_USEPOPUP, 0);
    callScintilla(SCI_SETLEXER, SCLEX_CONTAINER);
}

void ConsoleDialog::writeText(size_t length, const char *text)
{
	::SendMessage(m_scintilla, SCI_SETREADONLY, 0, 0);
	for (idx_t i = 0; i < length; ++i)
	{
		if (text[i] == '\r')
		{
			::SendMessage(m_scintilla, SCI_APPENDTEXT, i, reinterpret_cast<LPARAM>(text));
			text += i + 1;
			length -= i + 1;
			i = 0;
		}
	}
	
	if (length > 0)
	{
		::SendMessage(m_scintilla, SCI_APPENDTEXT, length, reinterpret_cast<LPARAM>(text));
	}

    ::SendMessage(m_scintilla, SCI_SETREADONLY, 1, 0);
    
    ::SendMessage(m_scintilla, SCI_GOTOPOS, ::SendMessage(m_scintilla, SCI_GETLENGTH, 0, 0), 0);
    
}


void ConsoleDialog::writeError(size_t length, const char *text)
{
    int docLength = callScintilla(SCI_GETLENGTH);
	int realLength = length;
    callScintilla(SCI_SETREADONLY, 0);
    for (idx_t i = 0; i < length; ++i)
	{
		if (text[i] == '\r')
		{
			if (i)
			{
				callScintilla(SCI_APPENDTEXT, i, reinterpret_cast<LPARAM>(text));
			}
			text += i + 1;
			length -= i + 1;
			realLength--;
			i = 0;
		}
	}
	
	if (length > 0)
	{
		callScintilla(SCI_APPENDTEXT, length, reinterpret_cast<LPARAM>(text));
	}

    callScintilla(SCI_SETREADONLY, 1);
    callScintilla(SCI_STARTSTYLING, docLength, 0x01);
    callScintilla(SCI_SETSTYLING, realLength, 1);

    
    callScintilla(SCI_COLOURISE, docLength, -1);
    callScintilla(SCI_GOTOPOS, docLength + realLength);
}


void ConsoleDialog::doDialog()
{
    if (!isCreated())
    {
        create(m_data);

		assert(m_data);
		if (m_data)
		{
			// define the default docking behaviour
			m_data->uMask			= DWS_DF_CONT_BOTTOM | DWS_ICONTAB;
			m_data->pszName = new TCHAR[20];
			_tcscpy_s(m_data->pszName, 20, _T("Python"));

			RECT rc;
			rc.bottom = 0;
			rc.top = 0;
			rc.left = 0;
			rc.right = 0;
			m_hTabIcon = (HICON)::LoadImage(_hInst, MAKEINTRESOURCE(IDI_PYTHON8), IMAGE_ICON, 16, 16, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);
			m_data->hIconTab			= m_hTabIcon;
			m_data->pszModuleName	= _T("Python Script");
			m_data->dlgID			= -1; /* IDD_CONSOLE */
			m_data->pszAddInfo	    = NULL; //_pExProp->szCurrentPath;
			m_data->iPrevCont		= -1;
			m_data->hClient			= _hSelf;
			m_data->rcFloat			= rc;


			::SendMessage(_hParent, NPPM_DMMREGASDCKDLG, 0, reinterpret_cast<LPARAM>(m_data));

			// Parse the whole doc, in case we've had errors that haven't been parsed yet
			callScintilla(SCI_COLOURISE, 0, -1);
		}
    }

    display(true);
}

void ConsoleDialog::hide()
{
    display(false);
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


void ConsoleDialog::onStyleNeeded(SCNotification* notification)
{
    int startPos = callScintilla(SCI_GETENDSTYLED);
    int startLine = callScintilla(SCI_LINEFROMPOSITION, startPos);
    int endPos = notification->position;
    int endLine = callScintilla(SCI_LINEFROMPOSITION, endPos);


    LineDetails lineDetails;
    for(int lineNumber = startLine; lineNumber <= endLine; ++lineNumber)
    {
        lineDetails.lineLength = (size_t)callScintilla(SCI_GETLINE, lineNumber);

        if (lineDetails.lineLength != SIZE_MAX)
        {
            lineDetails.line = new char[lineDetails.lineLength + 1];
            callScintilla(SCI_GETLINE, lineNumber, reinterpret_cast<LPARAM>(lineDetails.line));
            lineDetails.line[lineDetails.lineLength] = '\0';
            lineDetails.errorLevel = EL_UNSET;
            
            
            if (parseLine(&lineDetails))
            {
                startPos = callScintilla(SCI_POSITIONFROMLINE, lineNumber);

                // Check that it's not just a file called '<console>'
                if (strncmp(lineDetails.line + lineDetails.filenameStart, "<console>", lineDetails.filenameEnd - lineDetails.filenameStart))
                {
					int mask, style;
					switch(lineDetails.errorLevel)
					{
						case EL_WARNING:
							mask = 0x04;
							style = 0x04;
							break;

						case EL_ERROR:
							mask = 0x01;
							style = 0x01;
							break;

						case EL_UNSET:
						case EL_INFO:
						default:
							mask = 0x00;
							style = 0x00;
							break;
					}

					if (lineDetails.filenameStart > 0)
					{
						callScintilla(SCI_STARTSTYLING, startPos, mask);
						callScintilla(SCI_SETSTYLING, lineDetails.filenameStart, style);
					}


                    callScintilla(SCI_STARTSTYLING, startPos + lineDetails.filenameStart, mask | 0x02);
                    callScintilla(SCI_SETSTYLING, lineDetails.filenameEnd - lineDetails.filenameStart, style | 0x02);
					
					if (lineDetails.lineLength > lineDetails.filenameEnd)
					{
						callScintilla(SCI_STARTSTYLING, startPos + lineDetails.filenameEnd, mask);
						callScintilla(SCI_SETSTYLING, lineDetails.lineLength - lineDetails.filenameEnd, style);
					}


                }
            }

            delete[] lineDetails.line;
            
        }
    }

    // ensure that everything is set as styled (just move the endStyled variable on to the requested position)
    callScintilla(SCI_STARTSTYLING, notification->position, 0x0);

}

bool ConsoleDialog::parseLine(LineDetails *lineDetails)
{
	// Eg.
	//   File "C:\Users\Dave\AppData\Roaming\Notepad++\plugins\Config\PythonScript\scripts\fourth.py", line 2, in <module>
    if (0 == strncmp(lineDetails->line, "  File \"", 8)
		&& parsePythonErrorLine(lineDetails))
    {
        return true;
    }
	
	// Eg. 
	// e:\work\pythonscript\pythonscript\src\consoledialog.cpp(523): error C2065: 'ee' : undeclared identifier
	// Potentially with spaces in front if MSBUILD used
	// Line number can contain "," for column  (523,5)
	if (parseVSErrorLine(lineDetails))
    {
        return true;
    }

	// Eg.
	// C:/PalmDev/sdk-4/include/Core/System/NetMgr.h:550: warning: ignoring pragma: ;
	if (parseGCCErrorLine(lineDetails))
	{
		return true;
	}

    return false;
}

bool ConsoleDialog::parseVSErrorLine(LineDetails *lineDetails)
{
    enum StyleState
    {
		SS_BEGIN,
        SS_FILENAME,
        SS_LINENUMBER,
		SS_ERRORTYPE,
        SS_EXIT
    } styleState;

    bool retVal = false;
    styleState = SS_BEGIN;

    idx_t pos = 0;
    lineDetails->errorLineNo = IDX_MAX;

    while (styleState != SS_EXIT)
    {
        switch(styleState)
        {
			case SS_BEGIN:
				while(lineDetails->line[pos] == ' ')
				{
					++pos;
				}
				lineDetails->filenameStart = pos;
				styleState = SS_FILENAME;
				break;

            case SS_FILENAME:

                while(lineDetails->line[pos] != '(' && pos < lineDetails->lineLength)
                {
					if (pos - lineDetails->filenameStart == 1)
					{
						if (lineDetails->line[pos] != ':')
						{
							styleState = SS_EXIT;
							break;
						}
					}
					else if (!isValidFilenameChar(lineDetails->line[pos]))
					{
						styleState = SS_EXIT;
						break;
					}

                    ++pos;
                }
                
                if (lineDetails->line[pos] == '(') // Found the opening bracket for line no
                {
                    ++pos;
                    styleState = SS_LINENUMBER;
                }
                else
                {
                    styleState = SS_EXIT;
                }
                break;

            case SS_LINENUMBER:
                {
                    idx_t startLineNoPos = pos;
					idx_t endLineNoPos;
                    while(lineDetails->line[pos] >= '0' && lineDetails->line[pos] <= '9' && pos < lineDetails->lineLength)
                    {
                        ++pos;
                    }
					endLineNoPos = pos;

					if (pos < (lineDetails->lineLength + 1) 
                        && lineDetails->line[pos] == ',')
					{
						++pos;
						while(lineDetails->line[pos] >= '0' && lineDetails->line[pos] <= '9' && pos < lineDetails->lineLength)
						{
							++pos;
						}
					}
					

                    if (pos < (lineDetails->lineLength + 1) 
                        && lineDetails->line[pos] == ')' 
                        && lineDetails->line[pos+1] == ':')
                    {
						// If no line number, jump out
						if (endLineNoPos == startLineNoPos)
						{
							styleState = SS_EXIT;
							break;
						}

						char *lineNumber = new char[endLineNoPos - startLineNoPos + 2];
						strncpy_s(lineNumber, endLineNoPos - startLineNoPos + 2, lineDetails->line + startLineNoPos, endLineNoPos - startLineNoPos);
                        lineDetails->errorLineNo = strtoul(lineNumber, NULL, 0) - 1;
						delete[] lineNumber;
                        lineDetails->filenameEnd = startLineNoPos - 1;
                        retVal = true;
						pos += 3; // jump past "): " to either error or warning
                        styleState = SS_ERRORTYPE;
                    }
                    else
                    {
                        pos = startLineNoPos + 1;
                        styleState = SS_FILENAME;
                    }
                    break;
                }

			case SS_ERRORTYPE:
				if (0 == strncmp(lineDetails->line + pos, "error", 5))
				{
					lineDetails->errorLevel = EL_ERROR;
				}
				else if (0 == strncmp(lineDetails->line + pos, "warning", 7))
				{
					lineDetails->errorLevel = EL_WARNING;
				}
				styleState = SS_EXIT;
				break;

            case SS_EXIT:
                break;

            default:
                styleState = SS_EXIT;
                break;
        }
    }
    return retVal;
}


bool ConsoleDialog::parseGCCErrorLine(LineDetails *lineDetails)
{
    enum StyleState
    {
        SS_FILENAME,
        SS_LINENUMBER,
		SS_ERRORTYPE,
        SS_EXIT
    } styleState;

    bool retVal = false;
    styleState = SS_FILENAME;

    idx_t pos = 0;
    lineDetails->filenameStart = 0;
    lineDetails->errorLineNo = IDX_MAX;

    while (styleState != SS_EXIT)
    {
        switch(styleState)
        {
            case SS_FILENAME:
				{
					bool isEscaped = false;
					while((lineDetails->line[pos] != ':' || pos == 1) && pos < lineDetails->lineLength)
					{
						// Unescaped space, so leave - this isn't a gcc error
						if (lineDetails->line[pos] == ' ' && !isEscaped)
						{
							styleState = SS_EXIT;
							break;
						}

						// Unescaped invalid char, so it's not a gcc error
						if (!(isEscaped || lineDetails->line[pos] == '/' || isValidFilenameChar(lineDetails->line[pos]) || (lineDetails->line[pos] == ':' && pos == 1)))
						{
							styleState = SS_EXIT;
							break;
						}

						if (isEscaped)
							isEscaped = false;
						
						if (lineDetails->line[pos] == '\\')
							isEscaped = true;
						
						++pos;

						
					}
                
					if (lineDetails->line[pos] == ':') // Found the colon for line no
					{
						++pos;
						styleState = SS_LINENUMBER;
					}
					else
					{
						styleState = SS_EXIT;
					}
				}
                break;

            case SS_LINENUMBER:
                {
                    idx_t startLineNoPos = pos;
                    while(lineDetails->line[pos] >= '0' && lineDetails->line[pos] <= '9' && pos < lineDetails->lineLength)
                    {
                        ++pos;
                    }
                    if (pos < (lineDetails->lineLength + 1) 
                        && lineDetails->line[pos] == ':')
                    {
                        lineDetails->errorLineNo = strtoul(lineDetails->line + startLineNoPos, NULL, 0) - 1;

						// If the line number came out as 0, ie. there wasn't any, 
						// then the line is not a gcc error
						if (lineDetails->errorLineNo == -1)
						{
							styleState = SS_EXIT;
						}
						else
						{
							lineDetails->filenameEnd = startLineNoPos - 1;
							retVal = true;
							pos += 2;
							styleState = SS_ERRORTYPE;
						}
                    }
                    else
                    {
						// If we've found the end of the number, and it isn't followed with a colon:
						// then it's not a gcc error.
                        styleState = SS_EXIT;
                    }
                    break;
                }
			case SS_ERRORTYPE:
				if (0 == strncmp(lineDetails->line + pos, "error", 5))
				{
					lineDetails->errorLevel = EL_ERROR;
				}
				else if (0 == strncmp(lineDetails->line + pos, "warning", 7))
				{
					lineDetails->errorLevel = EL_WARNING;
				}
				styleState = SS_EXIT;
				break;

            case SS_EXIT:
                break;

            default:
                styleState = SS_EXIT;
                break;
        }
    }
    return retVal;
}

bool ConsoleDialog::parsePythonErrorLine(LineDetails *lineDetails)
{
    enum StyleState
    {
        SS_BEGIN,
        SS_FILENAME,
        SS_EXPECTLINE,
        SS_LINENUMBER,
        SS_EXIT
    } styleState;

    bool retVal = false;
    styleState = SS_BEGIN;
    lineDetails->errorLineNo = IDX_MAX;
    idx_t pos = 0;
    while(styleState != SS_EXIT)
    {
        switch(styleState)
        {
            case SS_BEGIN:
                if (strncmp(lineDetails->line, "  File \"", 8) == 0)
                {
                    pos = 8;
                    lineDetails->filenameStart = 8;
                    styleState = SS_FILENAME;
                }
                else
                {
                    styleState = SS_EXIT;
                }
                break;

            case SS_FILENAME:
                while(lineDetails->line[pos] != '\"' && pos < lineDetails->lineLength)
                {
                    ++pos;
                }
                
                if (pos >= lineDetails->lineLength) // Not found, so revert to default style
                {
                    styleState = SS_EXIT;
                }
                else
                {
                    lineDetails->filenameEnd = pos;	
                    retVal = true;
                    styleState = SS_EXPECTLINE;
                }
                break;

            case SS_EXPECTLINE:
                if (strncmp(lineDetails->line + pos, "\", line ", 8) == 0)
                {
                    pos += 8;
                    styleState = SS_LINENUMBER;
                }
                else
                {
                    styleState = SS_EXIT;
                }
                break;

            case SS_LINENUMBER:
                lineDetails->errorLineNo = strtoul(lineDetails->line + pos, NULL, 0) - 1;
                styleState = SS_EXIT;
                break;

            default:
                styleState = SS_EXIT;
                break;
        }
    }

    return retVal;
}

void ConsoleDialog::onHotspotClick(SCNotification* notification)
{
    int lineNumber = callScintilla(SCI_LINEFROMPOSITION, notification->position);
    LineDetails lineDetails;
    lineDetails.lineLength = (size_t)callScintilla(SCI_GETLINE, lineNumber);

    if (lineDetails.lineLength != SIZE_MAX)
    {
        lineDetails.line = new char[lineDetails.lineLength + 1];
        callScintilla(SCI_GETLINE, lineNumber, reinterpret_cast<LPARAM>(lineDetails.line));
        lineDetails.line[lineDetails.lineLength] = '\0';
        if (parseLine(&lineDetails))
        {
            lineDetails.line[lineDetails.filenameEnd] = '\0';
            m_console->openFile(lineDetails.line + lineDetails.filenameStart, lineDetails.errorLineNo);
        }
    }
}
