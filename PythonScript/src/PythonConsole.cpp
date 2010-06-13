
#include "stdafx.h"

#include "PythonConsole.h"
#include "ConsoleDialog.h"

using namespace boost::python;

PythonConsole::PythonConsole()
{
	mp_consoleDlg = new ConsoleDialog();
	
}

PythonConsole::~PythonConsole()
{
	delete mp_consoleDlg;
}

void PythonConsole::init(HINSTANCE hInst, NppData nppData)
{
	mp_consoleDlg->init(hInst, nppData, this);
	
}

void PythonConsole::initPython()
{
	try
	{
		
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

void PythonConsole::writeText(str text)
{
	mp_consoleDlg->writeText(len(text), (const char *)extract<const char *>(text));
}

bool PythonConsole::runStatement(const char *statement)
{
	object currentStdout = m_sys.attr("stdout");
	m_sys.attr("stdout") = ptr(this);
	object result = m_pushFunc(str(statement));
	m_sys.attr("stdout") = currentStdout;

	return extract<bool>(result);
}


void export_console()
{
	class_<PythonConsole>("Console", no_init)
		.def("write", &PythonConsole::writeText, "Create a new document");

}