#ifndef _PROCESSEXECUTE_H
#define _PROCESSEXECUTE_H

#include "stdafx.h"

struct PipeReaderArgs;

class ProcessExecute
{
public:

	ProcessExecute();
	~ProcessExecute();

	long execute(const TCHAR *commandLine,  boost::python::object pyStdout, boost::python::object pyStderr, boost::python::object pyStdin, bool spoolToFile = false);

protected:
	static bool isWindowsNT();
	static const int STREAM_NAME_LENGTH = 6;

private:
	static DWORD WINAPI pipeReader(void *args);
	void writeToPython(PipeReaderArgs *pipeReaderArgs, int bytesRead, char *buffer);
	void writeToFile(PipeReaderArgs *pipeReaderArgs, int bytesRead, char *buffer);

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
	
	/// Set if the output should be spooled to the fileHandle member
	/// if false, then write directly to pythonFile
	bool toFile;

	HANDLE fileHandle;
	HANDLE fileMutex;

	const char *streamName;
};

class process_start_exception
{
public:
	process_start_exception(const char *what)
		: m_what(what) 
		{};
	
	const char *what() const
	{ return m_what.c_str();
	}
	
private:
	std::string m_what;
};

#endif