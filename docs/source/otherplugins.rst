Interface for Other Plugins
===========================

For plugin authors, you can run Python_ scripts and expressions by calling Python Script remotely.  For this we use the Notepad++ message ``NPPM_MSGTOPLUGIN``.  

I'll give instructions for C / C++, other languages you'll need to convert the interface to - any submissions for other languages are welcome! 

First ``#include "PythonScript/NppPluginScript.h"`` from the PythonScript\include directory in the source archive.  The following instructions are also contained in this file.

A ``struct`` is defined in this file called ``PythonScript_Exec``.  This ``struct`` has a few members as follows.

-- Include PythonScript_Exec details --


There are two messages you can use - ``PYSCR_EXECSCRIPT`` and ``PYSCR_EXECSTATEMENT``.  For the first, you pass the absolute filename of the script you wish to run, the second, you pass the statement directly.

Here's a sample of how to call NPPM_MSGTOPLUGIN.::

-- Include MsgToPlugin Sample --

