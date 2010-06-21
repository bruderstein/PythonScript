
#include "stdafx.h"

#include "PythonConsole.h"
#include "ConsoleDialog.h"
#include "PythonHandler.h"

using namespace boost::python;
using namespace NppPythonScript;

PythonConsole::PythonConsole() :
	PyProducerConsumer<const char *>(),
		m_consumerStarted(false)
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
	
}


void PythonConsole::runStatement(const char *statement)
{
	bool retVal = false;

	mp_consoleDlg->runEnabled(false);

	// Console statements executed whilst a script is in progress MUST run on a separate 
	// thread.  Otherwise, we wait for the GIL, no problem, except that that blocks the UI thread
	// so if the script happens to be sending a message to scintilla (likely), then 
	// it will deadlock.
	// PyProducerConsumer used here to keep one thread running the actual statements

	if (!m_consumerStarted)
	{
		m_consumerStarted = true;
		startConsumer();
	}
	int length = strlen(statement);
	char *copy = new char[length + 1];
	strcpy_s(copy, length + 1, statement);

	produce(copy);
}

void PythonConsole::queueComplete()
{
	mp_consoleDlg->runEnabled(true);
}


void PythonConsole::consume(const char *statement)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	//const char *prompt = NULL;
	bool continuePrompt = false;
	try
	{
		object oldStdout = m_sys.attr("stdout");
		m_sys.attr("stdout") = ptr(this);
		object result = m_pushFunc(str(statement));
		m_sys.attr("stdout") = oldStdout;
	
		continuePrompt = extract<bool>(result);
		//prompt = extract<const char *>(continuePrompt ? m_sys.attr("ps2") : m_sys.attr("ps1"));
	}
	catch(...)
	{
		PyErr_Print();
	}

	PyGILState_Release(gstate);
	mp_consoleDlg->setPrompt(continuePrompt ? "... " : ">>> ");
	

	delete [] statement;

	
}


void PythonConsole::killStatement(PythonConsole *console)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	
	PyThreadState_SetAsyncExc(console->mp_python->getExecutingThreadID(), PyExc_KeyboardInterrupt);
	
	PyGILState_Release(gstate);
}



void export_console()
{
	class_<PythonConsole>("Console", no_init)
		.def("write", &PythonConsole::writeText, "Create a new document")
		.def("stopScript", &PythonConsole::stopScript, "Stops the currently script (if there's one running)");

}