#ifndef _PYTHONSCRIPT_H
#define _PYTHONSCRIPT_H

constexpr auto PLUGIN_NAME = _T("Python Script");
constexpr auto PLUGIN_MODULE_NAME = _T("PythonScript.dll");

constexpr auto MAX_MENU_SCRIPTS = 50;
constexpr auto FIXED_MENU_ITEMS = 9;


// The first scintilla notification - this is 2000
#define SCN_FIRST_NOTIFICATION   SCN_STYLENEEDED

#include <windows.h>

HWND getCurrScintilla();

#endif
