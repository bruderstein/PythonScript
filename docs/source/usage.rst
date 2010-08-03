Plugin Installation and Usage
=============================


Installation is very simple, either through Plugin Manager, or manually.

The file layout is as follows::

	Notepad++            (your main Notepad++ directory, probably under "C:\Program Files")
	 +
	 |-- python26.dll     (this is important. This needs to go in the main program directory of Notepad++, next to notepad++.exe)
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
		   |	   
		   |-- Config     (this config directory can also be in %APPDATA%\Notepad++\plugins\config\)
				 |
				 \-- PythonScript
						 \
						 |-- scripts
							   \
							   |-- (user level scripts go here)
							   
						   
