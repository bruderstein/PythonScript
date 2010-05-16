#pragma once


#include "stdafx.h"

// Forward def
class ScintillaWrapper;

class PythonHandler
{
public:
	PythonHandler::PythonHandler(char *pluginsDir, char *configDir, HWND nppHandle, HWND scintilla1Handle, HWND scintilla2Handle);
	~PythonHandler();

	bool runScript(const char *filename);
	bool runScript(const std::string& filename);




	void initPython();
	void runStartupScripts();

protected:
	virtual ScintillaWrapper* createScintillaWrapper();

	// Handles
	HWND m_nppHandle;
	HWND m_scintilla1Handle;
	HWND m_scintilla2Handle;

private:
	// Private methods
	void initModules();

	// Private member vars
	std::string m_machineBaseDir;
	std::string m_userBaseDir;
	ScintillaWrapper *mp_scintilla;
	
};

