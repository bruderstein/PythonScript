#include "stdafx.h"
#include "WcharMbcsConverter.h"
#include "PythonConsole.h"
#include "ConsoleDialog.h"
#include "PythonHandler.h"
#include "ProcessExecute.h"
#include "PluginInterface.h"
#include "ScintillaWrapper.h"
#include "PythonScript/NppPythonScript.h"
#include "scintilla.h"

// Sad, but we need to know if we're in an event handler when running an external command
// Not sure how I can extrapolate this info and not tie PythonConsole and NotepadPlusWrapper together.
#include "NotepadPlusWrapper.h"

PythonConsole::PythonConsole(HWND hNotepad) :
	PyProducerConsumer<std::string>(),
		mp_scintillaWrapper(new ScintillaWrapper(NULL)),
		mp_python(NULL),
		mp_mainThreadState(NULL),
		m_hNotepad(hNotepad),
		m_consumerStarted(false),
		m_nppData(new NppData)
{
	mp_consoleDlg = new ConsoleDialog();
	
	m_statementRunning = CreateEvent(NULL, FALSE, TRUE, NULL);
}

PythonConsole::~PythonConsole()
{
	try
	{
		delete mp_consoleDlg;
		delete m_nppData;
		delete mp_scintillaWrapper;
	}
	catch (...)
	{
		// I don't know what to do with that, but a destructor should never throw, so...
	}
}

void PythonConsole::init(HINSTANCE hInst, NppData& nppData)
{
	mp_consoleDlg->initDialog(hInst, nppData, this);
	*m_nppData = nppData;
	mp_scintillaWrapper->setHandle(mp_consoleDlg->getScintillaHwnd());
}

void PythonConsole::initPython(PythonHandler *pythonHandler)
{
	try
	{
		mp_python = pythonHandler;
		mp_mainThreadState = pythonHandler->getMainThreadState();
		
		PyGILState_STATE gstate = PyGILState_Ensure();

		boost::python::object main_module(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("__main__"))));
		boost::python::object main_namespace = main_module.attr("__dict__");
		
		// import code
		boost::python::object code = boost::python::import("code");
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

void PythonConsole::pythonShowDialog()
{
	// Post the message to ourselves (on the right thread) to create the window
	if (!mp_consoleDlg->isCreated())
	{
		CommunicationInfo commInfo;
		commInfo.internalMsg = PYSCR_SHOWCONSOLE;
		commInfo.srcModuleName = _T("PythonScript.dll");
		TCHAR pluginName[] = _T("PythonScript.dll");
		::SendMessage(m_hNotepad, NPPM_MSGTOPLUGIN, reinterpret_cast<WPARAM>(pluginName), reinterpret_cast<LPARAM>(&commInfo));
	}
	else
	{
		mp_consoleDlg->doDialog();
	}
}

void PythonConsole::showDialog()
{
	mp_consoleDlg->doDialog();
}

void PythonConsole::hideDialog()
{
	mp_consoleDlg->hide();
}

void PythonConsole::message(const char *msg)
{
	mp_consoleDlg->writeText(strlen(msg), msg);	
}

void PythonConsole::clear()
{
	mp_consoleDlg->clearText();
}


/** To call this function, you MUST have the GIL
 *  (it runs the __str__ attribute of the object)
 *  If you don't, or aren't sure, you can call message() instead, which takes a const char*
 */
void PythonConsole::writeText(boost::python::object text)
{
	mp_consoleDlg->writeText(len(text), (const char *)boost::python::extract<const char *>(text.attr("__str__")()));
}

void PythonConsole::writeError(boost::python::object text)
{
	mp_consoleDlg->writeError(len(text), (const char *)boost::python::extract<const char *>(text.attr("__str__")()));
}

void PythonConsole::stopStatement()
{
	DWORD threadID;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PythonConsole::stopStatementWorker, this, 0, &threadID);
	
}

long PythonConsole::runCommand(boost::python::str text, boost::python::object pyStdout, boost::python::object pyStderr)
{
	ProcessExecute process;
	std::shared_ptr<TCHAR> cmdLine = WcharMbcsConverter::char2tchar(boost::python::extract<const char *>(text));
	return process.execute(cmdLine.get(), pyStdout, pyStderr, boost::python::object(), NotepadPlusWrapper::isInEvent());
}

void PythonConsole::runStatement(const char *statement)
{
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

	produce(std::shared_ptr<std::string>(new std::string(statement)));
}

void PythonConsole::queueComplete()
{
	mp_consoleDlg->runEnabled(true);
}

void PythonConsole::consume(const std::shared_ptr<std::string>& statement)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	//const char *prompt = NULL;
	bool continuePrompt = false;
	try
	{
		boost::python::object oldStdout = m_sys.attr("stdout");
		m_sys.attr("stdout") = boost::python::ptr(this);
		boost::python::object result = m_pushFunc(boost::python::str(statement));
		m_sys.attr("stdout") = oldStdout;
	
		continuePrompt = boost::python::extract<bool>(result);
		//prompt = extract<const char *>(continuePrompt ? m_sys.attr("ps2") : m_sys.attr("ps1"));
	}
	catch(...)
	{
		PyErr_Print();
	}

	PyGILState_Release(gstate);
	mp_consoleDlg->setPrompt(continuePrompt ? "... " : ">>> ");
}

void PythonConsole::stopStatementWorker(PythonConsole *console)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	
	PyThreadState_SetAsyncExc(console->getConsumerThreadID(), PyExc_KeyboardInterrupt);
	
	PyGILState_Release(gstate);
}

void export_console()
{
	boost::python::class_<PythonConsole>("Console", boost::python::no_init)
		.def("write", &PythonConsole::writeText, "Writes text to the console.  Uses the __str__ function of the object passed.")
		.def("clear", &PythonConsole::clear, "Clears the console window")
		.def("writeError", &PythonConsole::writeError, "Writes text in the console in a red colour")
		.def("show", &PythonConsole::pythonShowDialog, "Shows the console")
		.def("hide", &PythonConsole::hideDialog, "Hides the console")
		.def("run", &PythonConsole::runCommand, "Runs a command on the console")
		.def("run", &PythonConsole::runCommandNoStderr, "Runs a command on the console")
		.def("run", &PythonConsole::runCommandNoStdout, "Runs a command on the console")
		.def_readonly("editor", &PythonConsole::mp_scintillaWrapper, "Gets an Editor object for the console window");

}

void PythonConsole::openFile(const char *filename, int lineNo)
{
	std::shared_ptr<TCHAR> tFilename = WcharMbcsConverter::char2tchar(filename);
	if (!SendMessage(m_hNotepad, NPPM_SWITCHTOFILE, 0, reinterpret_cast<LPARAM>(tFilename.get())))
	{
		SendMessage(m_hNotepad, NPPM_DOOPEN, 0, reinterpret_cast<LPARAM>(tFilename.get()));
	}

	if (lineNo != -1)
	{
		int currentView;
		SendMessage(m_hNotepad, NPPM_GETCURRENTSCINTILLA, 0, reinterpret_cast<LPARAM>(&currentView));
	
		SendMessage(currentView ? m_nppData->_scintillaSecondHandle : m_nppData->_scintillaMainHandle, SCI_GOTOLINE, lineNo, 0);
	}
}

HWND PythonConsole::getScintillaHwnd()
{
	return mp_consoleDlg->getScintillaHwnd();
}
