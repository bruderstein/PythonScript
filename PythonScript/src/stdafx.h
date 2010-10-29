// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <WindowsX.h>
#include <ShellApi.h>
#include <Commdlg.h>
#include <shlwapi.h>
#include <CommCtrl.h>
#include <HtmlHelp.h>
#include <basetsd.h>

#include <tchar.h>

#include <queue>
#include <string>
#include <map>
#include <set>
#include <list>
#include <fstream>
#include <memory>
#include <exception>

#pragma warning( push )

/*   Warnings disabled for boost::python
 *   4244: Conversion from long double to double
 *   4512: Assignment operator could not be generated
 *   4100: Unreferenced local parameter
 *   4127: Loop expression is constant
 */
#pragma warning( disable : 4244 4512 4100 4127)
#include <boost/python.hpp>
#pragma warning( pop )

#include <Python.h>

typedef  std::basic_string<TCHAR>	tstring;
