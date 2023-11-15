#ifndef _PYTHONSCRIPT_H
#define _PYTHONSCRIPT_H

#define PLUGIN_NAME	_T("Python Script")
#define PLUGIN_MODULE_NAME	_T("PythonScript.dll")
#define PLUGINTEMPLATE_INI	_T("\\PythonScript.ini")

#define MAX_MENU_SCRIPTS 50
#define FIXED_MENU_ITEMS 9


// The first scintilla notification - this is 2000
#define SCN_FIRST_NOTIFICATION   SCN_STYLENEEDED

#include <windows.h>

HWND getCurrScintilla();

#endif
