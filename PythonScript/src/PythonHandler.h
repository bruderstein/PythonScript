#pragma once


#include "stdafx.h"


// Forward def
class ScintillaWrapper;
class NotepadPlusWrapper;
class PythonConsole;
struct SCNotification;
struct RunScriptArgs;



class PythonHandler
{
public:
	PythonHandler::PythonHandler(char *pluginsDir, char *configDir, HWND nppHandle, HWND scintilla1Handle, HWND scintilla2Handle, PythonConsole *pythonConsole);
	~PythonHandler();

	bool runScript(const char *filename, bool synchronous = false);
	bool runScript(const std::string& filename, bool synchronous = false);

	static void runScriptWorker(RunScriptArgs* args);


	void notify(SCNotification *notifyCode);

	void initPython();
	void runStartupScripts();

	PyThreadState* getMainThreadState() { return mp_mainThreadState; };

	DWORD getExecutingThreadID() {return m_dwThreadId; };


protected:
	virtual ScintillaWrapper* createScintillaWrapper();
	virtual NotepadPlusWrapper* createNotepadPlusWrapper();

	// Handles
	HWND m_nppHandle;
	HWND m_scintilla1Handle;
	HWND m_scintilla2Handle;
	


private:
	// Private methods
	void initModules();

	// Private member vars
	std::string m_machineBaseDir;
	std::string m_userBaseDir;
	ScintillaWrapper *mp_scintilla;
	NotepadPlusWrapper *mp_notepad;
	PythonConsole *mp_console;
	int m_currentView;
	HANDLE m_hThread;
	DWORD m_dwThreadId;
	HANDLE m_scriptRunning;
	PyThreadState *mp_mainThreadState;
	PythonHandler *mp_python;
};

struct RunScriptArgs
{
	PythonHandler* instance;
	char* filename;
	HANDLE waitHandle;
	PyThreadState *threadState;
	bool synchronous;
};