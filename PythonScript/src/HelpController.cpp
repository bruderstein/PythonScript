#include "stdafx.h"

#include "HelpController.h"
#include "Scintilla.h"
#include "Notepad_plus_msgs.h"
#include "WcharMbcsConverter.h"
#include "PythonScriptVersion.h"

HelpController::HelpController(HWND hNotepad, HWND hScintilla)
	: m_hNotepad(hNotepad),
	  m_hScintilla(hScintilla)
{
}

void HelpController::callHelp()
{
	std::wstring helpFile = getFilename();
	if (helpFile.empty())
	{
		helpFile = L"http://npppythonscript.sourceforge.net/docs/";
		helpFile.append(WcharMbcsConverter::char2tchar(PYSCR_VERSION_STRING).get());
		::ShellExecute(m_hNotepad, L"open", helpFile.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
	else
	{
		TCHAR *pszOut = NULL;
		DWORD cchOUT = 0;
		HRESULT res = ::AssocQueryString(ASSOCF_INIT_IGNOREUNKNOWN, ASSOCSTR_EXECUTABLE, L".html", NULL, pszOut, &cchOUT);
		if (res == S_FALSE)
		{
			pszOut = new TCHAR[cchOUT];
			::AssocQueryString(ASSOCF_INIT_IGNOREUNKNOWN, ASSOCSTR_EXECUTABLE, L".html", NULL, pszOut, &cchOUT);

			helpFile.insert(0, L"\"file://");
			helpFile.append(L"\\");
			std::wstring topicUrl = getTopicUrl();
			helpFile.append((topicUrl == L"") ? L"index.html" : topicUrl);
			helpFile.append(L"\"");
			::ShellExecute(m_hNotepad, L"open", pszOut, helpFile.c_str(), NULL, SW_SHOWNORMAL);

			delete [] pszOut;
		}
	}
}

std::wstring HelpController::getFilename()
{
	TCHAR helpPath[MAX_PATH];
	::SendMessage(m_hNotepad, NPPM_GETNPPDIRECTORY, MAX_PATH, reinterpret_cast<LPARAM>(helpPath));
	_tcscat_s(helpPath, MAX_PATH, _T("\\plugins\\PythonScript\\doc"));

	if (::PathFileExists(helpPath))
	{
		return std::wstring(helpPath);
	}
	else
	{
		return std::wstring();
	}
}

std::wstring HelpController::getTopicUrl()
{
	size_t length = (size_t)SendMessage(m_hScintilla, SCI_GETCURLINE, 0, 0);
	char *buffer = new char[length + 1];
	SendMessage(m_hScintilla, SCI_GETCURLINE, length, reinterpret_cast<LPARAM>(buffer));

	idx_t position = (idx_t)SendMessage(m_hScintilla, SCI_GETCURRENTPOS, 0, 0);
	idx_t lineNumber = (idx_t)SendMessage(m_hScintilla, SCI_LINEFROMPOSITION, position, 0);
	idx_t lineStartPos = (idx_t)SendMessage(m_hScintilla, SCI_POSITIONFROMLINE, lineNumber, 0);


	position = position - lineStartPos;
	// position is now the character offset in buffer of the caret

	
	// Go forwards until we reach a non-acceptable char
	bool foundDot = false;
	idx_t dotPosition = 0;
	idx_t endPosition; 
	for(endPosition = position; 
		  (  (buffer[endPosition] >= 'A' && buffer[endPosition] <= 'Z')
		  || (buffer[endPosition] >= '0' && buffer[endPosition] <= '9')
		  || (buffer[endPosition] >= 'a' && buffer[endPosition] <= 'z')
		  || (!foundDot && buffer[endPosition] == '.'))
		  && endPosition < length; ++endPosition)
	{
		  if (buffer[endPosition] == '.')
		  {
			  foundDot = true;
			  dotPosition = endPosition;
		  }
	}
	

	
	// Go backwards until we reach a non-acceptable char
	idx_t startPosition = position > 0 ? position - 1 : 0;

	for(; 
		  (  (buffer[startPosition] >= 'A' && buffer[startPosition] <= 'Z')
		  || (buffer[startPosition] >= '0' && buffer[startPosition] <= '9')
		  || (buffer[startPosition] >= 'a' && buffer[startPosition] <= 'z')
		  || (!foundDot && buffer[startPosition] == '.'))
		  && startPosition != IDX_MAX; --startPosition)
	{
		  if (buffer[startPosition] == '.')
		  {
			  foundDot = true;
			  dotPosition = startPosition;
		  }
	}
	startPosition++;

	buffer[endPosition] = '\0';
	

	std::string url;

	if (foundDot && endPosition > startPosition)
	{
		if (dotPosition - startPosition == 7 && 0 == _strnicmp((buffer + startPosition), "notepad", dotPosition - startPosition))
		{
			url = "notepad.html#notepad.";
			url.append(buffer + dotPosition + 1);
		}

		if ((dotPosition - startPosition == 6 && 0 == _strnicmp((buffer + startPosition), "editor", dotPosition - startPosition))
			|| (dotPosition - startPosition == 7 && 0 == _strnicmp((buffer + startPosition), "editor1", dotPosition - startPosition))
			|| (dotPosition - startPosition == 7 && 0 == _strnicmp((buffer + startPosition), "editor2", dotPosition - startPosition)))
		{
			url = "scintilla.html#editor.";
			url.append(buffer + dotPosition + 1);
		}

		if (dotPosition - startPosition == 7 && 0 == _strnicmp((buffer + startPosition), "console", dotPosition - startPosition))
		{
			url = "console.html#console.";
			url.append(buffer + dotPosition + 1);
		}
	}

	delete[] buffer;

	return WcharMbcsConverter::char2tchar(url.c_str()).get();
}
