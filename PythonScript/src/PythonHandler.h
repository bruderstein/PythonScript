#include "stdafx.h"


#pragma once
class PythonHandler
{
public:
	PythonHandler::PythonHandler(char *pluginsDir, char *configDir);
	~PythonHandler();

	bool runScript(const char *filename);
	bool runScript(const std::string& filename);


	void initPython();
	void reinitPython();
	void runStartupScripts();


private:
	// Private methods
	void initModules();
	
	bool fileExists(const char *fileName);



	// Private member vars
	std::string m_machineBaseDir;
	std::string m_userBaseDir;
};

