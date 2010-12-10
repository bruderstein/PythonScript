#ifndef _PROCESSEXECUTE_H
#define _PROCESSEXECUTE_H

struct PipeReaderArgs;

class ProcessExecute
{
public:

	ProcessExecute();
	~ProcessExecute();

	DWORD execute(const TCHAR *commandLine,  boost::python::object pyStdout, boost::python::object pyStderr, boost::python::object pyStdin, bool spoolToFile = false);

protected:
	static bool isWindowsNT();
	static const int STREAM_NAME_LENGTH = 3;
	static const char *STREAM_NAME_STDOUT;
	static const char *STREAM_NAME_STDERR;

private:
	static DWORD WINAPI pipeReader(void *args);
	void writeToPython(PipeReaderArgs *pipeReaderArgs, DWORD bytesRead, char *buffer);
	void writeToFile(PipeReaderArgs *pipeReaderArgs, DWORD bytesRead, char *buffer);
	void spoolFile(std::fstream* file, boost::python::object pyStdout, boost::python::object pyStderr);

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
	std::fstream *file;
	HANDLE fileMutex;

	const char *streamName;
};

class process_start_exception
{
public:
	explicit process_start_exception(const char *desc)
		: m_desc(desc) 
		{};
	
	const char *what() const
	{ return m_desc.c_str();
	}
	
private:
	process_start_exception(); // default constructor disabled

	std::string m_desc;
};

#endif
