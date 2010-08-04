#include "stdafx.h"

#include "HelpController.h"
#include "Scintilla.h"
#include <ShellApi.h>
#include "Notepad_plus_msgs.h"
#include "WcharMbcsConverter.h"
#include "PythonScriptVersion.h"

using namespace std;


HelpController::HelpController(HWND hNotepad, HWND hScintilla)
	: m_hNotepad(hNotepad),
	  m_hScintilla(hScintilla)
{
}

void HelpController::callHelp()
{
	//::HtmlHelp(m_hNotepad, getFilename(), HH_DISPLAY_TOPIC, getTopicUrl().c_str());
	string helpFile = getFilename();
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

string HelpController::getFilename()
{
	TCHAR helpPath[MAX_PATH];
	::SendMessage(m_hNotepad, NPPM_GETNPPDIRECTORY, MAX_PATH, reinterpret_cast<LPARAM>(helpPath));
	_tcscat_s(helpPath, MAX_PATH, _T("\\plugins\\doc\\PythonScript\\PythonScript.chm"));

	if (::PathFileExists(helpPath))
	{
		return string(WcharMbcsConverter::tchar2char(helpPath).get());
	}
	else
	{
		return string();
	}
}

string HelpController::getTopicUrl()
{
	int length = SendMessage(m_hScintilla, SCI_GETCURLINE, 0, 0);
	char *buffer = new char[length + 1];
	SendMessage(m_hScintilla, SCI_GETCURLINE, length, reinterpret_cast<LPARAM>(buffer));

	int position = SendMessage(m_hScintilla, SCI_GETCURRENTPOS, 0, 0);
	int lineNumber = SendMessage(m_hScintilla, SCI_LINEFROMPOSITION, position, 0);
	int lineStartPos = SendMessage(m_hScintilla, SCI_POSITIONFROMLINE, lineNumber, 0);


	position = position - lineStartPos;
	// position is now the character offset in buffer of the caret

	
	// Go forwards until we reach a non-acceptable char
	bool foundDot = false;
	int dotPosition = 0;
	int endPosition; 
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
	int startPosition = position - 1;
	if (startPosition < 0)
		startPosition = 0;

	for(; 
		  (  (buffer[startPosition] >= 'A' && buffer[startPosition] <= 'Z')
		  || (buffer[startPosition] >= '0' && buffer[startPosition] <= '9')
		  || (buffer[startPosition] >= 'a' && buffer[startPosition] <= 'z')
		  || (!foundDot && buffer[startPosition] == '.'))
		  && startPosition >= 0; --startPosition)
	{
		  if (buffer[startPosition] == '.')
		  {
			  foundDot = true;
			  dotPosition = startPosition;
		  }
	}
	startPosition++;

	buffer[endPosition] = '\0';
	

	string url;

	if (foundDot && endPosition > startPosition)
	{
		if (dotPosition - startPosition == 7 && 0 == _strnicmp((buffer + startPosition), "notepad", dotPosition - startPosition))
		{
			url = "notepad.html#Notepad.";
			url.append(buffer + dotPosition + 1);
		}

		if (dotPosition - startPosition == 6 && 0 == _strnicmp((buffer + startPosition), "editor", dotPosition - startPosition))
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
