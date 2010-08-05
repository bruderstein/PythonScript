#ifndef _PYTHONCONSOLE_H
#define _PYTHONCONSOLE_H

#include "stdafx.h"
#include "PluginInterface.h"
#include "ConsoleDialog.h"
#include "PyProducerConsumer.h"
#include "ConsoleInterface.h"

class PythonHandler;
struct RunStatementArgs;

class PythonConsole : public NppPythonScript::PyProducerConsumer<const char *>, ConsoleInterface
{
public:
	PythonConsole(HWND hNotepad);
	~PythonConsole();
	
	void init(HINSTANCE hInst, NppData nppData);
	void initPython(PythonHandler *pythonHandler);

	void showDialog();
	// Show console but fire a message to get it created on the right thread.
	void pythonShowDialog();
	void hideDialog();

	void message(const char *msg);
	void writeText(boost::python::object text);
	void writeError(boost::python::object text);
	void clear();
	void stopScript();

	/* Console Interface members */
	void runStatement(const char *statement);
	void stopStatement();
	void setPrompt(const char *prompt);

	static void stopStatementWorker(PythonConsole *console);
	bool runStatementWorker(const char *statement);
	virtual void consume(const char *statement);
	
	void runCommand(boost::python::str text, boost::python::object pyStdout, boost::python::object pyStderr);
	void runCommandNoStderr(boost::python::str text, boost::python::object pyStdout)
		{ runCommand(text, pyStdout, boost::python::object(boost::python::ptr(this))); }
	void runCommandNoStdout(boost::python::str text)
		{ runCommand(text, boost::python::object(boost::python::ptr(this)), boost::python::object(boost::python::ptr(this))); }



	HWND getScintillaHwnd() { return mp_consoleDlg->getScintillaHwnd(); };


protected:
	virtual void queueComplete();

private:
	ConsoleDialog *mp_consoleDlg;
	boost::python::object m_console;
	boost::python::object m_pushFunc;
	boost::python::object m_sys;
	PythonHandler* mp_python;
	PyThreadState* mp_mainThreadState;
	HANDLE m_statementRunning;
	HANDLE m_hThread;
	HWND m_hNotepad;
	bool m_consumerStarted;
};

void export_console();

void importConsole(PythonConsole *console);

struct RunStatementArgs
{
	char *statement;
	HANDLE statementRunning;
	PythonConsole *console;
};

#endif
