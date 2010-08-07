#include "stdafx.h"
#include "ProcessExecute.h"
#include "WcharMbcsConverter.h"

/* The Console Redirection is taken from the TagsView plugin from Vitaliy Dovgan.  
 * My thanks to him for pointing me in the right direction. :)
 * And of course for NppExec, without which, Notepad++ would only 
 * be half as powerful.
 */

#define DEFAULT_PIPE_SIZE 1
#define PIPE_READBUFSIZE  4096

using namespace boost::python;
using namespace std;

ProcessExecute::ProcessExecute()
{
}

ProcessExecute::~ProcessExecute()
{
}


bool ProcessExecute::isWindowsNT()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(osv);
	::GetVersionEx(&osv);
	return (osv.dwPlatformId >= VER_PLATFORM_WIN32_NT);
}

long ProcessExecute::execute(const TCHAR *commandLine, boost::python::object pyStdout, boost::python::object pyStderr, boost::python::object pyStdin)
{
	DWORD returnValue = 0;

	if (pyStdout.is_none())
		throw process_start_exception("stdout cannot be None");
	if (pyStderr.is_none())
		throw process_start_exception("stderr cannot be None");

	// Create out, err, and in pipes (ignore in, initially)
	SECURITY_DESCRIPTOR sd;
	SECURITY_ATTRIBUTES sa;
	process_start_exception exceptionThrown("");
	bool thrown = false;

	Py_BEGIN_ALLOW_THREADS
	try
	{
	if (isWindowsNT())
	{
		::InitializeSecurityDescriptor( &sd, SECURITY_DESCRIPTOR_REVISION );
		::SetSecurityDescriptorDacl( &sd, TRUE, NULL, FALSE );
		sa.lpSecurityDescriptor = &sd;
	}
	else
	{
		sa.lpSecurityDescriptor = NULL;
	}
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;

	if (!::CreatePipe(&m_hStdOutReadPipe, &m_hStdOutWritePipe, &sa, DEFAULT_PIPE_SIZE))
	{
		throw process_start_exception("Error creating pipe for stdout");
	}

	if (!::CreatePipe(&m_hStdErrReadPipe, &m_hStdErrWritePipe, &sa, DEFAULT_PIPE_SIZE))
	{
		throw process_start_exception("Error creating pipe for stderr");
	}

	HANDLE stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	DWORD dwThreadId;
	PipeReaderArgs stdoutReaderArgs;
	PipeReaderArgs stderrReaderArgs;

	stdoutReaderArgs.processExecute = this;
	stdoutReaderArgs.hPipeRead = m_hStdOutReadPipe;
	stdoutReaderArgs.hPipeWrite = m_hStdOutWritePipe;
	stdoutReaderArgs.pythonFile = pyStdout;
	stdoutReaderArgs.stopEvent = stopEvent;
	stdoutReaderArgs.completedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	stderrReaderArgs.processExecute = this;
	stderrReaderArgs.hPipeRead = m_hStdErrReadPipe;
	stderrReaderArgs.hPipeWrite = m_hStdErrWritePipe;
	stderrReaderArgs.stopEvent = stopEvent;
	stderrReaderArgs.pythonFile = pyStderr;
	stderrReaderArgs.completedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	// start thread functions for stdout and stderr
	HANDLE hStdoutThread = CreateThread( 
			NULL,							// no security attribute 
			0,								// default stack size 
			pipeReader,						// thread proc
			(LPVOID) &stdoutReaderArgs,		// thread parameter 
			0,								// not suspended 
			&dwThreadId);					// returns thread ID 

	HANDLE hStderrThread = CreateThread(
			NULL,							// no security attribute 
			0,								// default stack size 
			pipeReader,						// thread proc
			(LPVOID) &stderrReaderArgs,		// thread parameter 
			0,								// not suspended 
			&dwThreadId);					// returns thread ID 


	// start process
	PROCESS_INFORMATION pi;
	STARTUPINFO         si;
	

	::SetHandleInformation(m_hStdOutReadPipe, HANDLE_FLAG_INHERIT, 0);
	::SetHandleInformation(m_hStdErrReadPipe, HANDLE_FLAG_INHERIT, 0);

	// initialize STARTUPINFO struct
	::ZeroMemory( &si, sizeof(STARTUPINFO) );
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdInput = NULL;
	si.hStdOutput = m_hStdOutWritePipe;
	si.hStdError = m_hStdErrWritePipe;

	::ZeroMemory( &pi, sizeof(PROCESS_INFORMATION) );
	
	int commandLineLength = _tcslen(commandLine) + 1;
	TCHAR *cmdLine = new TCHAR[commandLineLength];
	_tcscpy_s(cmdLine, commandLineLength, commandLine);
	bool processStartSuccess;

	if ( ::CreateProcess(
				NULL, 
				cmdLine,
				NULL,                     // security
				NULL,                     // security
				TRUE,                     // inherits handles
				CREATE_NEW_PROCESS_GROUP, // creation flags
				NULL,                     // environment
				NULL,                     // current directory
				&si,                      // startup info
				&pi                       // process info
		))
	{
		// wait for process to exit
		::WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hThread);	

		processStartSuccess = true;

	}
	else
	{
		processStartSuccess = false;
	}

	
	
	// Stop the reader threads
	SetEvent(stopEvent);	
	// Wait for the pipe reader threads to complete
	HANDLE handles[] = { stdoutReaderArgs.completedEvent, stderrReaderArgs.completedEvent };
	WaitForMultipleObjects(2, handles, TRUE, INFINITE);
	CloseHandle(stdoutReaderArgs.completedEvent);
	CloseHandle(stderrReaderArgs.completedEvent);
	CloseHandle(hStdoutThread);
	CloseHandle(hStderrThread);
	CloseHandle(stopEvent);

	if (processStartSuccess)
	{
		GetExitCodeProcess(pi.hProcess, &returnValue);
		CloseHandle(pi.hProcess);
	}
	else
	{
		DWORD errorNo = ::GetLastError();
		TCHAR *buffer;

		::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, errorNo, 0, reinterpret_cast<LPTSTR>(&buffer), 0, NULL);

		shared_ptr<char> message = WcharMbcsConverter::tchar2char(buffer);
		process_start_exception ex(message.get());

		::LocalFree(buffer);
		throw ex;
	
	}
	}
	catch(process_start_exception& ex)
	{
		exceptionThrown = ex;
		thrown = true;
	}
	

	Py_END_ALLOW_THREADS
	
	if (thrown)
	{
		throw exceptionThrown;
	}
	
	return returnValue;
}


DWORD WINAPI ProcessExecute::pipeReader(void *args)
{
	PipeReaderArgs* pipeReaderArgs = reinterpret_cast<PipeReaderArgs*>(args);

	DWORD bytesRead;
	
	
	char buffer[PIPE_READBUFSIZE];
	BOOL success = TRUE;
	BOOL processFinished = FALSE;
	BOOL dataFinished = FALSE;
	OVERLAPPED oOverlap;
	oOverlap.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	HANDLE handles[2];
	handles[0] = pipeReaderArgs->stopEvent;
	int handleIndex;

	while(!dataFinished)
	{
		::PeekNamedPipe(pipeReaderArgs->hPipeRead, NULL, 0, NULL, &bytesRead, NULL);

		if (processFinished && 0 == bytesRead)
		{
			dataFinished = TRUE;
			break;
		}



		if (bytesRead > 0)
		{
			success = ReadFile(pipeReaderArgs->hPipeRead, buffer, PIPE_READBUFSIZE - 1, &bytesRead, NULL);
			buffer[bytesRead] = '\0';
			PyGILState_STATE gstate = PyGILState_Ensure();
			try
			{
				pipeReaderArgs->pythonFile.attr("write")(boost::python::str(const_cast<const char *>(buffer)));
			}
			catch(...)
			{
				PyErr_Print();
			}
			PyGILState_Release(gstate);
		}
		else
		{
			handleIndex = WaitForSingleObject(pipeReaderArgs->stopEvent, 100);
			switch(handleIndex)
			{
				case WAIT_OBJECT_0:
					// Process Stopped
					{
						processFinished = TRUE;
					}
					break;
			}
		}

		
	}

	CloseHandle(pipeReaderArgs->hPipeRead);
	CloseHandle(pipeReaderArgs->hPipeWrite);
	SetEvent(pipeReaderArgs->completedEvent);

	return 0;
}
