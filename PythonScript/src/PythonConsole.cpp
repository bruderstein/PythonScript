
#include "stdafx.h"

#include "PythonConsole.h"
#include "ConsoleDialog.h"
#include "PythonHandler.h"

using namespace boost::python;

PythonConsole::PythonConsole()
{
	mp_consoleDlg = new ConsoleDialog();
	m_statementRunning = CreateEvent(NULL, FALSE, TRUE, NULL);
	
}

PythonConsole::~PythonConsole()
{
	delete mp_consoleDlg;
}

void PythonConsole::init(HINSTANCE hInst, NppData nppData)
{
	mp_consoleDlg->init(hInst, nppData, this);
	
}

void PythonConsole::initPython(PythonHandler *pythonHandler)
{
	try
	{
		mp_python = pythonHandler;
		mp_mainThreadState = pythonHandler->getMainThreadState();

		PyGILState_STATE gstate = PyGILState_Ensure();

		object main_module(handle<>(borrowed(PyImport_AddModule("__main__"))));
		object main_namespace = main_module.attr("__dict__");
		
		// import code
		object code = import("code");
		main_namespace["code"] = code;

		// import __main__
		main_namespace["__main__"] = main_namespace;
		
		// get ref to code.InteractiveConsole().push()
		m_console = eval("code.InteractiveConsole(__main__)", main_namespace, main_namespace);
		m_pushFunc = m_console.attr("push");

		m_sys = main_namespace["sys"];
	
		PyGILState_Release(gstate);
		
	} 
	catch(...)
	{
		PyErr_Print();
	}
	
}

void PythonConsole::showDialog()
{
	mp_consoleDlg->doDialog();
}


void PythonConsole::message(const char *msg)
{
	mp_consoleDlg->writeText(strlen(msg), msg);	
}

/** To call this function, you MUST have the GIL
 *  If you don't, or aren't sure, you can call message() instead, which takes a const char*
 */
void PythonConsole::writeText(object text)
{
	mp_consoleDlg->writeText(len(text), (const char *)extract<const char *>(text.attr("__str__")()));
}

void PythonConsole::stopScript()
{
	DWORD threadID;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PythonConsole::killStatement, this, 0, &threadID);
	// Join thread?
}


bool PythonConsole::runStatement(const char *statement)
{
	bool retVal = false;
	// Console statements executed whilst a script is in progress MUST run on a separate 
	// thread.  Otherwise, we wait for the GIL, no problem, except that that blocks the UI thread
	// so if the script happens to be sending a message to scintilla (likely), then 
	// it will deadlock.
	
	DWORD result = WaitForSingleObject(m_statementRunning, INFINITE);
	if (result == WAIT_OBJECT_0)
	{
		// Close the old thread
		if (m_hThread != NULL)
			CloseHandle(m_hThread);
	
		RunStatementArgs *args = new RunStatementArgs();
		int length = strlen(statement);
		args->statement = new char[length + 1];
		strcpy_s(args->statement, length + 1, statement);
	
		args->statementRunning = m_statementRunning;
		args->console = this;
	
		DWORD threadID;
		m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)runStatementWorkerStatic, args, 0, &threadID);
	
		if (m_hThread != NULL)
			retVal = true;
		else
			retVal = false;
	}
	else
		retVal = false;

	return retVal;
}

bool PythonConsole::runStatementWorker(const char *statement)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	object oldStdout = m_sys.attr("stdout");
	m_sys.attr("stdout") = ptr(this);
	object result = m_pushFunc(str(statement));
	m_sys.attr("stdout") = oldStdout;
	
	bool retVal = extract<bool>(result);

	PyGILState_Release(gstate);

	SetEvent(m_statementRunning);
	return retVal;
}


void PythonConsole::killStatement(PythonConsole *console)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	
	PyThreadState_SetAsyncExc(console->mp_python->getExecutingThreadID(), PyExc_KeyboardInterrupt);
	
	PyGILState_Release(gstate);
}

void PythonConsole::runStatementWorkerStatic(RunStatementArgs *args)
{
	args->console->runStatementWorker(args->statement);
}


void export_console()
{
	class_<PythonConsole>("Console", no_init)
		.def("write", &PythonConsole::writeText, "Create a new document")
		.def("stopScript", &PythonConsole::stopScript, "Stops the currently script (if there's one running)");

}