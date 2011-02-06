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
	//::HtmlHelp(m_hNotepad, getFilename(), HH_DISPLAY_TOPIC, getTopicUrl().c_str());
	std::string helpFile = getFilename();
	bool useWeb = false;
	if (helpFile.empty())
	{
		helpFile = "http://npppythonscript.sourceforge.net/docs/";
		helpFile.append(PYSCR_VERSION_STRING);
		helpFile.append("/");
		useWeb = true;
	}

	if (useWeb)
	{
		helpFile.append(getTopicUrl());
		::ShellExecuteA(m_hNotepad, "open", helpFile.c_str(), NULL, NULL, SW_SHOWDEFAULT);
	}
	else
	{
		::HtmlHelpA(m_hNotepad, helpFile.c_str(), HH_DISPLAY_TOPIC, reinterpret_cast<DWORD_PTR>(getTopicUrl().c_str()));
	}

}

std::string HelpController::getFilename()
{
	TCHAR helpPath[MAX_PATH];
	::SendMessage(m_hNotepad, NPPM_GETNPPDIRECTORY, MAX_PATH, reinterpret_cast<LPARAM>(helpPath));
	_tcscat_s(helpPath, MAX_PATH, _T("\\plugins\\doc\\PythonScript\\PythonScript.chm"));

	if (::PathFileExists(helpPath))
	{
		return std::string(WcharMbcsConverter::tchar2char(helpPath).get());
	}
	else
	{
		return std::string();
	}
}

std::string HelpController::getTopicUrl()
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
			url = "notepad.html#Notepad.";
			url.append(buffer + dotPosition + 1);
		}

		if ((dotPosition - startPosition == 6 && 0 == _strnicmp((buffer + startPosition), "editor", dotPosition - startPosition))
			|| (dotPosition - startPosition == 7 && 0 == _strnicmp((buffer + startPosition), "editor1", dotPosition - startPosition))
			|| (dotPosition - startPosition == 7 && 0 == _strnicmp((buffer + startPosition), "editor2", dotPosition - startPosition)))
		{
			url = "scintilla.html#Editor.";
			url.append(buffer + dotPosition + 1);
		}

		if (dotPosition - startPosition == 7 && 0 == _strnicmp((buffer + startPosition), "console", dotPosition - startPosition))
		{
			url = "console.html#Console.";
			url.append(buffer + dotPosition + 1);
		}
	}
	
	return url;
}
