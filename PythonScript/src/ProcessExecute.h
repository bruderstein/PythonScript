#ifndef _PROCESSEXECUTE_H
#define _PROCESSEXECUTE_H

#include "stdafx.h"

class ProcessExecute
{
public:

	ProcessExecute();
	~ProcessExecute();

	int execute(const TCHAR *commandLine,  boost::python::object pyStdout, boost::python::object pyStderr, boost::python::object pyStdin);

protected:
	static bool isWindowsNT();

private:
	static DWORD WINAPI pipeReader(void *args);
	HANDLE m_hStdOutReadPipe; 
	HANDLE m_hStdOutWritePipe;
	HANDLE m_hStdErrReadPipe; 
	HANDLE m_hStdErrWritePipe;
};

struct PipeReaderArgs
{
	ProcessExecute* processExecute;
	HANDLE hPipeRead;
	HANDLE hPipeWrite;
	HANDLE stopEvent;
	HANDLE completedEvent;
	boost::python::object pythonFile;
};


#endif