#ifndef _NOTEPADPLUSWRAPPER_H
#define _NOTEPADPLUSWRAPPER_H

#include "stdafx.h"
#include "Notepad_plus_msgs.h"
#include "PluginInterface.h"
#include "ScintillaWrapper.h"

class NotepadPlusWrapper
{
public:
	NotepadPlusWrapper(HWND nppHandle);
	
	void notify(SCNotification *notifyCode);

	void save();
	void saveAs(const char *filename);
	void saveAsCopy(const char *filename);

	

	int getCurrentView();
	LangType getCurrentLangType();
	void SetCurrentLangType(LangType lang);

	/*
	boost::python::list getFiles();
	

	boost::python::list getSessionFiles();
	void saveSession(const char *sessionFilename);
	void saveCurrentSession();

	ScintillaWrapper createScintilla();
	void destroyScintilla(ScintillaWrapper buffer);

	int getCurrentDocIndex(int view);

	void setStatusBar(STATUSBARSECTION section, const char *text);
	long getPluginMenuHandle();

	void activateIndex(int view, int index);

	void loadSession(const char *filename);

	void activateFile(const char *filename);

	void reloadFile(const char *filename, bool withAlert = true);

	void save();

	void saveAllFiles();

	void setFilename(const char *filename);

	boost::python::str getPluginConfigDir();

	void menuCommand(int commandID);

	boost::python::tuple getNotepadVersion();

	void hideTabBar();

	void showTabBar();

	bool isTabBarShown();

	int getCurrentBufferID();

	void reloadBuffer(int bufferID, bool withAlert = true);

	LangType getBufferLangType(int bufferID);

	void setBufferLangType(int bufferID, LangType lang);

	BufferEncoding getBufferEncoding(int bufferID);

	void setBufferEncoding(int bufferID, BufferEncoding encoding);

	FormatType getBufferFormat(int bufferID);

	void setFormatType(int bufferID, FormatType format);

	
	*/


protected:
	LRESULT callNotepad(UINT message, WPARAM wParam = 0, LPARAM lParam = 0)
	{
		return SendMessage(m_nppHandle, message, wParam, lParam);
	}

private:
	HWND m_nppHandle;
	
	
};

#endif
