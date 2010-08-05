#ifndef _NPPPYTHONSCRIPT_H
#define _NPPPYTHONSCRIPT_H



#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef  WIN32_LEAN_AND_MEAN

#include <tchar.h>

#define PYSCR_START (WM_USER + 3500)

/**  Execute a script in a file
 *   (wParam = 0, PythonScript_Exec *pse)
 */
#define PYSCR_EXECSCRIPT			(PYSCR_START+0)



/**  Execute a script as a statement
 *   (wParam = 0, PythonScript_Exec *pse)
 */
#define PYSCR_EXECSTATEMENT			(PYSCR_START+1)

/** Show/Create the console window
 * No arguments
 */
#define PYSCR_SHOWCONSOLE			(PYSCR_START+2)

/** Use PYSCRF_SYNC in the flags member to run the script or statement
 *  synchronously (i.e. within the same thread).  The SendMessage() call
 *  will return when the script or statement has completed.
 *  
 *  Note that the completedEvent is NOT signalled if PYSCRF_SYNC is used.
 *  Also bear in mind, that running scripts synchronously runs the script on the 
 *  UI thread, so everything is blocked - in a long running script, it will look like
 *  N++ has frozen - short scripts may just cause a jitter.  You have been warned :)
 *  Better is to use the completedEvent of the struct to wait on a separate thread for 
 *  the script to finish.
 */
#define PYSCRF_SYNC					(0x0001)


struct PythonScript_Exec 
{
	/// PythonScript_Exec structure version - must always be 1
	int structVersion;

	/// Python Script (for PYSCR_EXECSTATEMENT) or absolute filename (for PYSCR_EXECSCRIPT)
	TCHAR *script;

	/** HANDLE to an event created with CreateEvent.  This event is signalled (with SetEvent)
	 *  when the script has completed. 
	 *  This can be NULL, in which case no event is signalled
	 * 
	 *  Important: Do NOT wait on the handle on the thread that Notepad++ runs on 
	 *  (ie. the plugin default thread) unless you really know what you are doing :)
	 *  It will almost certainly deadlock (as Scintilla or N++ may need to process a message) 
	 */

	HANDLE completedEvent;

	/** Flags for running the script
	 *  Combination of PYSCRF_xxxxx constants
	 */
	int flags;

	/** PythonScripts sets this to TRUE when the PythonScript plugin 
	 *  has successfully received the message. 
	 *  Note that a TRUE here does NOT mean the script has run, or ran without
	 *  errors, merely that the script has been added to the queue to run
	 * 
	 *  You should set this to FALSE initially, and check for TRUE when SendMessage() returns.
	 */
	BOOL deliverySuccess;
};
	

#endif
