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
	PythonConsole();
	~PythonConsole();
	
	void init(HINSTANCE hInst, NppData nppData);
	void initPython(PythonHandler *pythonHandler);
	void showDialog();

	void message(const char *msg);
	void writeText(boost::python::object text);
	void stopScript();

	/* Console Interface members */
	void runStatement(const char *statement);
	void stopStatement();
	void setPrompt(const char *prompt);

	static void stopStatementWorker(PythonConsole *console);
	bool runStatementWorker(const char *statement);
	virtual void consume(const char *statement);
	

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
