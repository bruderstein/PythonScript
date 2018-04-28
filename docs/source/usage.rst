Plugin Installation and Usage
=============================

Installation
------------

Installation is very simple, either through Plugin Manager, or manually.  The zip or 7zip archive contains the 
the files in the correct layout - just copy to your Notepad++ directory (e.g. `c:\\Program Files\\Notepad++`)

The file layout is as follows::

	Notepad++            (your main Notepad++ directory, probably under "C:\Program Files")
	 +
	 |-- python27.dll     (this is important. This needs to go in the main program directory of Notepad++, next to notepad++.exe)
	 |                    (unless you have a Python installation already, in which case it's probably in C:\windows already)
	 +-- plugins
		   \
		   |-- PythonScript.dll
		   |
		   |-- PythonScript
		   |   \
		   |   |-- lib
		   |   |    \
		   |   |     |-- (*.py)   lots of *.py files and subdirectories
		   |   |
		   |   |-- scripts
		   |		 \
		   |	  	 |-- (machine-level scripts)
		   |		 
		   |
		   |-- doc
		   |    \
		   |     |-- PythonScript
		   |             \
		   |             |-- PythonScript.chm         (optional, if it's not there context-sensitive help will use the web)
		   |
		   |
		   |
		   |
		   |	   
		   |-- Config     (this config directory can also be in %APPDATA%\Notepad++\plugins\config\)
				 |
				 \-- PythonScript
						 \
						 |-- scripts
							   \
							   |-- (user level scripts go here)
							   
						   


Usage
-----

To use Python Script, you'll first need to create a script.  Click New Script from the Python Script menu in the Plugins menu.
This creates a new user script (in your config directory).  When you've typed your script out, you can run it from the `Scripts` submenu.

*The `Scripts` submenu is automatically updated whenever you save a script, so if you copy a file into the scripts directory manually, 
you'll need to make a change (a dummy new line or comment etc) and save a script (any will do) in Notepad++ for it to update.*

To edit the script, just hold Ctrl down and click the script in the `Scripts` menu.  

If you click `Configuration`, you can assign the script either to a toolbar icon, or to the Python Script menu itself.  If you assign a 
script to the menu, then it will appear immediately, but you will not be able to assign a shortcut to it until next time Notepad++ starts.

If you assign it to a toolbar icon, then it will only appear on the next start of Notepad++.

Context-Sensitive help is available - if your cursor is on and notepad, editor or console function, the ``Context-Help`` menu
will take you straight to the documentation for that function.  If the CHM file is not in the relevant location, it will open the 
documentation from the web.


Startup
-------

.. _startup.py:

The script called startup.py (in either the "machine" directory or "user" directory - see Installation_) is run when Python Script
starts up.  Normally, that is the first time a script is called.  The default startup.py script does some small things like directing 
the error output to the console, and importing some commonly used modules.  

If you want to register an callback (see :ref:`Notifications`) to run from when Notepad++ starts up, you need to change the startup type
to ATSTARTUP, instead of LAZY.  You can do this in the Configuration dialog.

