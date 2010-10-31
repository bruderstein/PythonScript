#ifndef _PYTHONHANDLER_H
#define _PYTHONHANDLER_H

#ifndef _PYPRODUCER_H
#include "PyProducerConsumer.h"
#endif

// Forward def
class ScintillaWrapper;
class NotepadPlusWrapper;
class PythonConsole;
struct SCNotification;
struct RunScriptArgs;

class PythonHandler : NppPythonScript::PyProducerConsumer<RunScriptArgs*>
{
public:
	PythonHandler::PythonHandler(char *pluginsDir, char *configDir, HINSTANCE hInst, HWND nppHandle, HWND scintilla1Handle, HWND scintilla2Handle, PythonConsole *pythonConsole);
	~PythonHandler();

	bool runScript(const char *filename, bool synchronous = false, bool allowQueuing = false, HANDLE completedEvent = NULL, bool isStatement = false);
	bool runScript(const std::string& filename, bool synchronous = false, bool allowQueuing = false, HANDLE completedEvent = NULL, bool isStatement = false);
	
	void runScriptWorker(RunScriptArgs* args);

	void consume(RunScriptArgs* args);

	void notify(SCNotification *notifyCode);

	void initPython();
	void runStartupScripts();
	void stopScript();

	PyThreadState* getMainThreadState() { return mp_mainThreadState; };

	DWORD getExecutingThreadID() { return getConsumerThreadID(); };
	

protected:
	virtual ScintillaWrapper* createScintillaWrapper();
	virtual NotepadPlusWrapper* createNotepadPlusWrapper();
	virtual void queueComplete();

	// Handles
	HWND m_nppHandle;
	HWND m_scintilla1Handle;
	HWND m_scintilla2Handle;
	


private:
	// Private methods
	void initModules();

	static void stopScriptWorker(PythonHandler *handler);

	// Private member vars

	HINSTANCE m_hInst;

	std::string m_machineBaseDir;
	std::string m_userBaseDir;
	ScintillaWrapper *mp_scintilla;
	ScintillaWrapper *mp_scintilla1;
	ScintillaWrapper *mp_scintilla2;

	NotepadPlusWrapper *mp_notepad;

	PythonConsole *mp_console;

	int m_currentView;

	PyThreadState *mp_mainThreadState;
	PythonHandler *mp_python;

	bool m_consumerStarted;
	HANDLE m_hKillWait;
	
};

struct RunScriptArgs
{
	char* filename;
	PyThreadState *threadState;
	bool synchronous;
	HANDLE completedEvent;
	bool isStatement;
};

#endif