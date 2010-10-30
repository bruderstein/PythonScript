#ifndef _PYTHONCONSOLE_H
#define _PYTHONCONSOLE_H

#ifndef _CONSOLEINTERFACE_H 
#include "ConsoleInterface.h"
#endif

#ifndef _PYPRODUCER_H
#include "PyProducerConsumer.h"
#endif

class PythonHandler;
class ScintillaWrapper;
class ConsoleDialog;
struct RunStatementArgs;
struct NppData;

class PythonConsole : public NppPythonScript::PyProducerConsumer<const char *>, ConsoleInterface
{
public:
	PythonConsole(HWND hNotepad);
	~PythonConsole();
	
	void init(HINSTANCE hInst, NppData& nppData);
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
	void openFile(const char *filename, int lineNumber);
	
	/* ConsoleInterface end */


	static void stopStatementWorker(PythonConsole *console);
	bool runStatementWorker(const char *statement);
	virtual void consume(const char *statement);
	
	long runCommand(boost::python::str text, boost::python::object pyStdout, boost::python::object pyStderr);
	long runCommandNoStderr(boost::python::str text, boost::python::object pyStdout)
	{ 
		boost::python::object sys_module( (boost::python::handle<>(PyImport_ImportModule("sys"))) );
		boost::python::object sys_namespace = sys_module.attr("__dict__");	
		return runCommand(text, pyStdout, sys_namespace["stderr"]); 
    }
	long runCommandNoStdout(boost::python::str text)
	{ 
		boost::python::object sys_module( (boost::python::handle<>(PyImport_ImportModule("sys"))) );
		boost::python::object sys_namespace = sys_module.attr("__dict__");	
		boost::python::object npp_module( (boost::python::handle<>(PyImport_ImportModule("Npp"))) );
		boost::python::object npp_namespace = npp_module.attr("__dict__");	
		return runCommand(text, npp_namespace["console"], sys_namespace["stderr"]); 
    }

	HWND getScintillaHwnd();


	ScintillaWrapper* mp_scintillaWrapper;
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

	NppData* m_nppData;
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
