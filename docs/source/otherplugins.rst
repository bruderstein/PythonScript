Interface for Other Plugins
===========================

.. highlight:: c

For plugin authors, you can run Python_ scripts and expressions by calling Python Script remotely.  For this we use the Notepad++ message ``NPPM_MSGTOPLUGIN``.  

I'll give instructions for C / C++, other languages you'll need to convert the interface to - any submissions for other languages are welcome! 

First ``#include "PythonScript/NppPluginScript.h"`` from the PythonScript\include directory in the source archive.  The following instructions are also contained in this file.

A ``struct`` is defined in this file called ``PythonScript_Exec``.  This ``struct`` has a few members as follows.::

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


There are two messages you can use - ``PYSCR_EXECSCRIPT`` and ``PYSCR_EXECSTATEMENT``.  For the first, you pass the absolute filename of the script you wish to run, the second, you pass the statement directly.

Here's a sample of how to call NPPM_MSGTOPLUGIN.::

	void callPython()
	{
		PythonScript_Exec pse;
		pse.structVersion = 1;

		HANDLE completeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		pse.completedEvent = completeEvent;
		pse.deliverySuccess = FALSE;
		
		TCHAR script[] = _T("e:\\script_to_run.py");
		pse.script = script;
		pse.flags = PYSCRF_SYNC;
		

		TCHAR pluginName[] = _T("PythonScript.dll");
		CommunicationInfo commInfo;
		commInfo.internalMsg = PYSCR_EXECSCRIPT;
		commInfo.srcModuleName = _T("MyPlugin.dll");
		
		commInfo.info = reinterpret_cast<void*>(&pse);
		
		BOOL delivery = SendMessage(nppData._nppHandle, NPPM_MSGTOPLUGIN, reinterpret_cast<WPARAM>(pluginName), reinterpret_cast<LPARAM>(&commInfo));
		if (!delivery)
		{
			MessageBox(NULL, _T("Python Script not found"), _T("Msg2PluginTester"), 0);
		}


		if (pse.deliverySuccess)
		{
			MessageBox(NULL, _T("Delivery Success"), _T("Msg2PluginTester"), 0);
		}
		else
		{
			MessageBox(NULL, _T("Delivery FAILED!"), _T("Msg2PluginTester"), 0);
		}

		// Optionally now start a thread to wait for the completion
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadRun, (LPVOID)completeEvent, 0, NULL);
	}
	...
	
	void threadRun(HANDLE waitEvent)
	{
		WaitForSingleObject(waitEvent, INFINITE);
		MessageBox(NULL, _T("Finished!"), _T("I waited..."), 0);
	}
	
.. _Python: https://www.python.org/
