#ifndef _PYTHONCONSOLE_H
#define _PYTHONCONSOLE_H

#include "stdafx.h"
#include "PluginInterface.h"
#include "ConsoleDialog.h"
#include "PyProducerConsumer.h"

class PythonHandler;
struct RunStatementArgs;

class PythonConsole 
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

	bool runStatement(const char *statement);
	static void runStatementWorkerStatic(RunStatementArgs *args);
	static void killStatement(PythonConsole *console);
	bool runStatementWorker(const char *statement);
	


	HWND getScintillaHwnd() { return mp_consoleDlg->getScintillaHwnd(); };

private:
	ConsoleDialog *mp_consoleDlg;
	boost::python::object m_console;
	boost::python::object m_pushFunc;
	boost::python::object m_sys;
	PythonHandler* mp_python;
	PyThreadState* mp_mainThreadState;
	HANDLE m_statementRunning;
	HANDLE m_hThread;

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
