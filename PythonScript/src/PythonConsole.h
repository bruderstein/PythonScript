#ifndef _PYTHONCONSOLE_H
#define _PYTHONCONSOLE_H

#include "stdafx.h"
#include "PluginInterface.h"



class ConsoleDialog;


class PythonConsole
{
public:
	PythonConsole();
	~PythonConsole();
	
	void init(HINSTANCE hInst, NppData nppData);
	void initPython();
	void showDialog();
	void writeText(boost::python::str text);
	bool runStatement(const char *statement);

private:
	ConsoleDialog *mp_consoleDlg;
	boost::python::object m_console;
	boost::python::object m_pushFunc;
	boost::python::object m_sys;
};

void export_console();

void importConsole(PythonConsole *console);


#endif
