#ifndef _PYTHONCONSOLE_H
#define _PYTHONCONSOLE_H

#ifndef _CONSOLEINTERFACE_H 
#include "ConsoleInterface.h"
#endif

#ifndef _PYPRODUCER_H
#include "PyProducerConsumer.h"
#endif

struct NppData;

namespace NppPythonScript
{
class PythonHandler;
class ScintillaWrapper;

class ConsoleDialog;
struct RunStatementArgs;

class PythonConsole : public PyProducerConsumer<std::string>, public ConsoleInterface
{
public:
	explicit PythonConsole(HWND hNotepad);
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

	/* Console Interface members */
	void runStatement(const char *statement);
	void stopStatement();
	void openFile(const char *filename, idx_t lineNumber);
	
	/* ConsoleInterface end */


	static void stopStatementWorker(PythonConsole *console);
	
	DWORD runCommand(boost::python::str text, boost::python::object pyStdout, boost::python::object pyStderr);
	DWORD runCommandNoStderr(boost::python::str text, boost::python::object pyStdout)
	{ 
		boost::python::object sys_module( (boost::python::handle<>(PyImport_ImportModule("sys"))) );
		boost::python::object sys_namespace = sys_module.attr("__dict__");	
		return runCommand(text, pyStdout, sys_namespace["stderr"]); 
    }

	DWORD runCommandNoStdout(boost::python::str text)
	{ 
		boost::python::object sys_module( (boost::python::handle<>(PyImport_ImportModule("sys"))) );
		boost::python::object sys_namespace = sys_module.attr("__dict__");	
		boost::python::object npp_module( (boost::python::handle<>(PyImport_ImportModule("Npp"))) );
		boost::python::object npp_namespace = npp_module.attr("__dict__");	
		return runCommand(text, npp_namespace["console"], sys_namespace["stderr"]); 
    }

	HWND getScintillaHwnd();

	boost::shared_ptr<ScintillaWrapper> getScintillaWrapper() { return mp_scintillaWrapper; }

	boost::shared_ptr<ScintillaWrapper> mp_scintillaWrapper;

	static boost::python::str getEncoding() { return boost::python::str("utf-8"); }
	
protected:
	virtual void consume(std::shared_ptr<std::string> statement);
	virtual void queueComplete();

private:
	PythonConsole(); // default constructor disabled
	PythonConsole& operator = (const PythonConsole&); // assignment operator disabled
	PythonConsole(const PythonConsole& other);

	ConsoleDialog *mp_consoleDlg;
	
	boost::python::object m_console;
	boost::python::object m_pushFunc;
	boost::python::object m_sys;
	PyThreadState* mp_mainThreadState;
	HANDLE m_statementRunning;
	HWND m_hNotepad;
	bool m_consumerStarted;

	NppData* m_nppData;
};

void export_console();

void importConsole(boost::shared_ptr<PythonConsole> console);

struct RunStatementArgs
{
	char *statement;
	HANDLE statementRunning;
	PythonConsole *console;
};

}

#endif
