#ifndef _NOTEPADPLUSWRAPPER_H
#define _NOTEPADPLUSWRAPPER_H

#include "stdafx.h"
#include "Notepad_plus_msgs.h"
#include "PluginInterface.h"
#include "ScintillaWrapper.h"

enum FormatType
{
	WIN_FORMAT, 
	MAC_FORMAT, 
	UNIX_FORMAT
};

enum BufferEncoding
{
	uni8Bit			= 0, 
	uniUTF8			= 1, 
	uni16BE			= 2, 
	uni16LE			= 3, 
	uniCookie		= 4, 
	uni7Bit			= 5, 
	uni16BE_NoBOM	= 6, 
	uni16LE_NoBOM	= 7, 
	uniEnd
};

enum StatusBarSection
{
	STATUSBARSECTION_DOCTYPE = STATUSBAR_DOC_TYPE,
	STATUSBARSECTION_DOCSIZE = STATUSBAR_DOC_SIZE,
	STATUSBARSECTION_CURPOS = STATUSBAR_CUR_POS,
	STATUSBARSECTION_EOFFORMAT = STATUSBAR_EOF_FORMAT,
	STATUSBARSECTION_UNICODETYPE = STATUSBAR_UNICODE_TYPE,
	STATUSBARSECTION_TYPINGMODE = STATUSBAR_TYPING_MODE
};

// enum for the notifications
enum Notification
{
	NPPNOTIF_READY = NPPN_READY,
	NPPNOTIF_TBMODIFICATION = NPPN_TBMODIFICATION,
	NPPNOTIF_FILEBEFORECLOSE = NPPN_FILEBEFORECLOSE,
	NPPNOTIF_FILEOPENED = NPPN_FILEOPENED,
	NPPNOTIF_FILECLOSED = NPPN_FILECLOSED,
	NPPNOTIF_FILEBEFOREOPEN = NPPN_FILEBEFOREOPEN,
	NPPNOTIF_FILEBEFORESAVE = NPPN_FILEBEFORESAVE,
	NPPNOTIF_FILESAVED = NPPN_FILESAVED,
	NPPNOTIF_SHUTDOWN = NPPN_SHUTDOWN,
	NPPNOTIF_BUFFERACTIVATED = NPPN_BUFFERACTIVATED,
	NPPNOTIF_LANGCHANGED = NPPN_LANGCHANGED,
	NPPNOTIF_WORDSTYLESUPDATED = NPPN_WORDSTYLESUPDATED,
	NPPNOTIF_SHORTCUTREMAPPED = NPPN_SHORTCUTREMAPPED,
	NPPNOTIF_FILEBEFORELOAD = NPPN_FILEBEFORELOAD,
	NPPNOTIF_FILELOADFAILED = NPPN_FILELOADFAILED,
	NPPNOTIF_READONLYCHANGED = NPPN_READONLYCHANGED
};

class NotepadPlusWrapper
{
public:
	NotepadPlusWrapper(HWND nppHandle);
	~NotepadPlusWrapper();

	void notify(SCNotification *notifyCode);
	
	void newDocument();

	void save();
	void saveAs(const char *filename);
	void saveAsCopy(const char *filename);

	void open(const char *filename);

	bool activateFile(const char *filename);

	int getCurrentView();
	LangType getCurrentLangType();
	void setCurrentLangType(LangType lang);

	
	boost::python::list getFiles();
	
	
	boost::python::list getSessionFiles(const char *sessionFilename);
	
	void saveSession(const char *sessionFilename, boost::python::list files);
	void saveCurrentSession(const char *filename);
	
	ScintillaWrapper createScintilla();
	void destroyScintilla(ScintillaWrapper& buffer);

	
	int getCurrentDocIndex(int view);

	void setStatusBar(StatusBarSection section, const char *text);
	long getPluginMenuHandle();

	void activateIndex(int view, int index);
	
	void loadSession(boost::python::str filename);
	
	void activateFile(str filename);

	void reloadFile(str filename, bool withAlert);

	void saveAllFiles();

	void setFilename(const char *filename);

	boost::python::str getPluginConfigDir();

	
	
	void menuCommand(int commandID);

	
	boost::python::tuple getVersion();

	void hideTabBar();

	void showTabBar();

	bool isTabBarShown();

	int getCurrentBufferID();
	
	void reloadBuffer(int bufferID, bool withAlert = true);
	
	LangType getLangType();

	LangType getBufferLangType(int bufferID);

	void setLangType(LangType language);

	void setBufferLangType(LangType lang, int bufferID);

	BufferEncoding getEncoding();

	BufferEncoding getBufferEncoding(int bufferID);

	void setEncoding(BufferEncoding encoding);
	
	void setBufferEncoding(BufferEncoding encoding, int bufferID);
	
	FormatType getFormatType();

	FormatType getBufferFormatType(int bufferID);

	void setFormatType(FormatType format);

	void setBufferFormatType(FormatType format, int bufferID);
	
	bool callback(PyObject* callback, list events);
	
	

protected:
	LRESULT callNotepad(UINT message, WPARAM wParam = 0, LPARAM lParam = 0)
	{
		return SendMessage(m_nppHandle, message, wParam, lParam);
	}

private:
	HWND m_nppHandle;
	typedef std::multimap<int, PyObject*> callbackT;
	callbackT m_callbacks;
	bool m_notificationsEnabled;
	
};





#endif
