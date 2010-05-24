#include "stdafx.h"

#include "WcharMbcsConverter.h"
#include "NotepadPlusWrapper.h"

NotepadPlusWrapper::NotepadPlusWrapper(HWND nppHandle)
	: m_nppHandle(nppHandle)
{
}
	

void NotepadPlusWrapper::notify(SCNotification *notifyCode)
{
}

void NotepadPlusWrapper::save()
{
	callNotepad(NPPM_SAVECURRENTFILE);
}


void NotepadPlusWrapper::saveAs(const char *filename)
{
	callNotepad(NPPM_SAVECURRENTFILEAS, FALSE, reinterpret_cast<LPARAM>(WcharMbcsConverter::char2tchar(filename).get()));
}
	
void NotepadPlusWrapper::saveAsCopy(const char *filename)
{
	callNotepad(NPPM_SAVECURRENTFILEAS, TRUE, reinterpret_cast<LPARAM>(WcharMbcsConverter::char2tchar(filename).get()));
}

int NotepadPlusWrapper::getCurrentView()
{
	return callNotepad(NPPM_GETCURRENTSCINTILLA);
}

LangType NotepadPlusWrapper::getCurrentLangType()
{
	int lang;
	callNotepad(NPPM_GETCURRENTLANGTYPE, 0, reinterpret_cast<LPARAM>(&lang));
	return static_cast<LangType>(lang);
}


void NotepadPlusWrapper::SetCurrentLangType(LangType lang)
{
	callNotepad(NPPM_SETCURRENTLANGTYPE, 0, static_cast<LPARAM>(lang));
}


