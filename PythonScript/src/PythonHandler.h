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

struct RunScriptArgs
{
public:
	RunScriptArgs(
		const char* filename,
		PyThreadState *threadState,
		bool synchronous,
		HANDLE completedEvent,
		bool isStatement
	):
		m_filename(filename?filename:""),
		m_threadState(threadState),
		m_synchronous(synchronous),
		m_completedEvent(completedEvent),
		m_isStatement(isStatement)
	{

	}

	std::string m_filename;
	PyThreadState *m_threadState;
	bool m_synchronous;
	HANDLE m_completedEvent;
	bool m_isStatement;
};


class PythonHandler : public NppPythonScript::PyProducerConsumer<RunScriptArgs>
{
public:
	PythonHandler::PythonHandler(char *pluginsDir, char *configDir, HINSTANCE hInst, HWND nppHandle, HWND scintilla1Handle, HWND scintilla2Handle, PythonConsole *pythonConsole);
	~PythonHandler();

	bool runScript(const char *filename, bool synchronous = false, bool allowQueuing = false, HANDLE completedEvent = NULL, bool isStatement = false);
	bool runScript(const std::string& filename, bool synchronous = false, bool allowQueuing = false, HANDLE completedEvent = NULL, bool isStatement = false);
	
	void runScriptWorker(const std::shared_ptr<RunScriptArgs>& args);

	void notify(SCNotification *notifyCode);

	void initPython();
	void runStartupScripts();
	void stopScript();

	PyThreadState* getMainThreadState() { return mp_mainThreadState; };

	DWORD getExecutingThreadID() { return getConsumerThreadID(); };
	

protected:
	void consume(const std::shared_ptr<RunScriptArgs>& args);

	ScintillaWrapper* createScintillaWrapper();
	NotepadPlusWrapper* createNotepadPlusWrapper();
	virtual void queueComplete();

	// Handles
	HWND m_nppHandle;
	HWND m_scintilla1Handle;
	HWND m_scintilla2Handle;
	


private:
	PythonHandler(); // default constructor disabled

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

	bool m_consumerStarted;
};

#endif
