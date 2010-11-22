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


const char *ProcessExecute::STREAM_NAME_STDOUT = "OUT";
const char *ProcessExecute::STREAM_NAME_STDERR = "ERR";

ProcessExecute::ProcessExecute()
	: m_hStdOutReadPipe (NULL), 
	  m_hStdOutWritePipe (NULL),
	  m_hStdErrReadPipe (NULL), 
	  m_hStdErrWritePipe (NULL)
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

long ProcessExecute::execute(const TCHAR *commandLine, boost::python::object pyStdout, boost::python::object pyStderr, boost::python::object pyStdin, bool spoolToFile /* = false */)
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
	
	PipeReaderArgs stdoutReaderArgs;
	PipeReaderArgs stderrReaderArgs;
	// Only used if spooling, but we need to delete it later.
	TCHAR tmpFilename[MAX_PATH] = {0};

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
	

		stdoutReaderArgs.processExecute = this;
		stdoutReaderArgs.hPipeRead = m_hStdOutReadPipe;
		stdoutReaderArgs.hPipeWrite = m_hStdOutWritePipe;
		stdoutReaderArgs.pythonFile = pyStdout;
		stdoutReaderArgs.stopEvent = stopEvent;
		stdoutReaderArgs.completedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		stdoutReaderArgs.streamName = STREAM_NAME_STDOUT;
		stdoutReaderArgs.toFile = false;

		stderrReaderArgs.processExecute = this;
		stderrReaderArgs.hPipeRead = m_hStdErrReadPipe;
		stderrReaderArgs.hPipeWrite = m_hStdErrWritePipe;
		stderrReaderArgs.pythonFile = pyStderr;
		stderrReaderArgs.stopEvent = stopEvent;
		stderrReaderArgs.completedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		stderrReaderArgs.streamName = STREAM_NAME_STDERR;
		stderrReaderArgs.toFile = false;

	
	


		/* If we're in an event, we need to spool the output to a file 
		 * first, before we write it to python - so that the python writing is 
		 * done in *this* thread
		 */
		if (spoolToFile)
		{
			stdoutReaderArgs.toFile = true;
			stderrReaderArgs.toFile = true;
		
			/// Create the mutex for writing to the file
			stdoutReaderArgs.fileMutex = CreateMutex(NULL, FALSE, NULL);
			stderrReaderArgs.fileMutex = stdoutReaderArgs.fileMutex;

			/// Create the temp file
			TCHAR tmpPath[MAX_PATH];

			GetTempPath(MAX_PATH, tmpPath);
			if(!GetTempFileName(tmpPath, _T("py"), 0, tmpFilename))
			{
				throw process_start_exception("Error creating temporary filename for output spooling");
			}
			stdoutReaderArgs.file = new fstream(tmpFilename, fstream::binary | fstream::in | fstream::out);
			stderrReaderArgs.file = stdoutReaderArgs.file;
			/*
			stdoutReaderArgs.fileHandle = CreateFile(tmpFilename, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			stderrReaderArgs.fileHandle = stdoutReaderArgs.fileHandle;
		
			if (INVALID_HANDLE_VALUE == stdoutReaderArgs.fileHandle)
			{
				throw process_start_exception("Error opening temporary file for output spooling");
			}
			*/


		}
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
		// We use an auto_ptr here because of a potential early out due to an exception thrown.
		auto_ptr<TCHAR> cmdLine(new TCHAR[commandLineLength]);
		_tcscpy_s(cmdLine.get(), commandLineLength, commandLine);
		bool processStartSuccess;

		if ( ::CreateProcess(
					NULL, 
					cmdLine.get(),
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
	
	if (spoolToFile)
	{
		CloseHandle(stdoutReaderArgs.fileMutex);
		
		spoolFile(stdoutReaderArgs.file, pyStdout, pyStderr);
		
		stdoutReaderArgs.file->close();
		if (tmpFilename[0] != 0)
		{
			DeleteFile(tmpFilename);
		}
	}

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
	BOOL processFinished = FALSE;
	BOOL dataFinished = FALSE;
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
			if (ReadFile(pipeReaderArgs->hPipeRead, buffer, PIPE_READBUFSIZE - 1, &bytesRead, NULL))
			{
				if (pipeReaderArgs->toFile)
				{
					pipeReaderArgs->processExecute->writeToFile(pipeReaderArgs, bytesRead, buffer);
				}
				else
				{
					pipeReaderArgs->processExecute->writeToPython(pipeReaderArgs, bytesRead, buffer);
				}
			}
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

				default:
					// Do nothing
					break;
			}
		}

		
	}

	CloseHandle(pipeReaderArgs->hPipeRead);
	CloseHandle(pipeReaderArgs->hPipeWrite);
	SetEvent(pipeReaderArgs->completedEvent);

	return 0;
}

void ProcessExecute::writeToPython(PipeReaderArgs *pipeReaderArgs, int bytesRead, char *buffer)
{
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


void ProcessExecute::writeToFile(PipeReaderArgs *pipeReaderArgs, int bytesRead, char *buffer)
{
	WaitForSingleObject(pipeReaderArgs->fileMutex, INFINITE);

	pipeReaderArgs->file->write(pipeReaderArgs->streamName, ProcessExecute::STREAM_NAME_LENGTH);


	if (pipeReaderArgs->file->bad())
		throw process_start_exception("Error writing to spool file");

	char byteCount[20];
	_itoa_s(bytesRead, byteCount, 20, 10);
	strcat_s(byteCount, 20, "\n");
	
	pipeReaderArgs->file->write(byteCount, strlen(byteCount));

	pipeReaderArgs->file->write(buffer, bytesRead);

	
	if (pipeReaderArgs->file->bad())
		throw process_start_exception("Error writing buffer to spool file");

	ReleaseMutex(pipeReaderArgs->fileMutex);
}


void ProcessExecute::spoolFile(fstream* file, object pyStdout, object pyStderr)
{
	// Rewind
	file->seekg(0);
	char infoBuffer[30];
	char *buffer = NULL;
	int bufferSize = 0;
	int bytesToRead;

	while (!file->eof())
	{
		file->getline(infoBuffer, 30, '\n');
		if (file->eof())
			break;

		bytesToRead = atoi(infoBuffer + STREAM_NAME_LENGTH);
		if (bufferSize < bytesToRead || !buffer)
		{
			if (buffer)
				delete[] buffer;
			
			bufferSize = bytesToRead + 1;
			buffer = new char[bufferSize];
		}

		file->read(buffer, bytesToRead);
		buffer[bytesToRead] = '\0';
		if (0 == strncmp(infoBuffer, STREAM_NAME_STDOUT, STREAM_NAME_LENGTH))
		{
			// Is a stdout message
			pyStdout.attr("write")(boost::python::str(const_cast<const char *>(buffer)));
		}
		else
		{
			// is a stderr message
			pyStderr.attr("write")(boost::python::str(const_cast<const char *>(buffer)));
		}
	}

	if (buffer)
	{
		delete[] buffer;
	}
}
