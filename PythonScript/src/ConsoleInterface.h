#ifndef _CONSOLEINTERFACE_H 
#define _CONSOLEINTERFACE_H

#include "stdafx.h"

class ConsoleInterface
{
public:
	ConsoleInterface() { };
	virtual ~ConsoleInterface() {};
	virtual void runStatement(const char *) = 0;
};


#endif