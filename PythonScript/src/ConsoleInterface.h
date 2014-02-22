#ifndef _CONSOLEINTERFACE_H 
#define _CONSOLEINTERFACE_H

namespace NppPythonScript
{

class ConsoleInterface
{
public:
	ConsoleInterface() { };
	virtual ~ConsoleInterface() {};
	virtual void runStatement(const char *) = 0;
	virtual void stopStatement() = 0;
	virtual void openFile(const char *filename, idx_t lineNumber) = 0;
};

}

#endif
