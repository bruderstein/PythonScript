Compiling PythonScript
======================

Introduction
------------

This section is only intended for those interested in compiling the PythonScript.dll plugin from source.  
If you want to "compile" Python scripts, then you are looking in the wrong place - Python is automatically 
compiled, so isn't normally something you need to worry about.  Just run the script from the menu!

The source code for Python Script is held on Github, at http://github.com/davegb3/PythonScript

The code in Sourceforge's git repository is _normally_ the same, but not necessarily, as it may be slightly behind.

Boost.Python
------------

Boost.Python is an excellent library that helps bridge the gap between C/C++ and Python.  In truth, it makes
the Python Script plugin possible.  

If you just need the binary library (which you is probably all you need), they're available on the download site, 
so you don't have to build them yourself.

Building the Boost.Python library is not as hard as it looks, you just
need to know which options are required.  Download boost (http://boost.org), and bjam - and have the bjam.exe
in your path somewhere (or call it directly).  From the ``libs\python\build`` directory in your boost directory,
you now need to run bjam.

The options you need are::

    bjam link=static runtime-link=static threading=multi
	
To build a release version, use::

    bjam release link=static runtime-link=static threading=multi
	
And that's basically it.  I try to keep the compiled libraries on the download site, 
so you can just download pre-built binary libraries.

Please note that this is no official guide, this is just what works for me.
There's lots more info on the boost website about building Boost.Python.

Building Python
---------------

You need to have a valid python27.dll (or python26.dll for PythonScript 0.7), and the corresponding lib (python27.lib).
The "official" PythonScript is built with a custom Python27.dll, that has the /MT flag enabled, as do all the modules.
Note that VS2008 is used to build Python, as that's what the official distribution uses.  

This is slightly more involved that just setting the /MT or /MTd flag (instead of /MD or /MDd respectively) in the
C/C++ Code Generation options.  Python compiles some utilities along the way that help with the building of the modules,
and in some (maybe one) of these tools, the /MD or /MDd is hard coded.  Other than that, it's just a matter of setting the 
library paths in the .props file.  Depending on the modules you want to build, you may need to build the library the module 
uses first (for example the tcl/tk library).  

Again, the libraries are available on the download site, so you don't have to build a python27.dll & python27.lib if you 
don't need to actually debug through Python (or change something etc).

Building PythonScript itself
----------------------------

Next step is to set the paths of where your boost and python libraries are.  That's just a matter of setting the properties 
in the .vsprops file - View, Other Windows, Property Manager in VS2010.  Then set the macros in the User Macros section (under Debug and Release).

One last note on the code, a lot of the Python wrapper is automatically generated, with the CreateWrapper.py script.  If you need to
alter a Scintilla function, change the CreateWrapper.py script, not the generated code, or your changes will be lost next time CreateWrapper.py 
is run.  (This doesn't happen automatically, I just do it when I import a new Scintilla.iface file from the Scintilla project).


	

