Compiling PythonScript
======================

Introduction
------------

This section is only intended for those interested in compiling the PythonScript.dll plugin from source.  
If you want to "compile" Python scripts, then you are looking in the wrong place - Python is automatically 
compiled, so isn't normally something you need to worry about.  Just run the script from the menu!

The source code for Python Script is held on Github, at https://github.com/bruderstein/PythonScript

Boost.Python
------------

Boost.Python is an excellent library that helps bridge the gap between C/C++ and Python.  In truth, it makes
the Python Script plugin possible.  

If you just need the binary library (which you is probably all you need), they're available on the download site, 
so you don't have to build them yourself.

Building the Boost.Python library is not as hard as it looks, you just
need to know which options are required.  Download boost (https://boost.org), and bjam - and have the bjam.exe
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

You need to have a valid python39.dll and the corresponding lib (python39.lib).
The official build uses nuget packages therefore.

Other than that, it's just a matter of setting the library paths in the .props file.
Depending on the modules you want to build, you may need to build the library the module 
uses first (for example the tcl/tk library).  

The libraries are available from nuget or part of the python installation, 
so you don't have to build a python39.dll & python39.lib if you 
don't need to actually debug through Python (or change something etc).

Building PythonScript itself
----------------------------

Next step is to set the paths of where your boost and python libraries are.  That's just a matter of setting the properties 
in the .vsprops file - View, Other Windows, Property Manager in VS2017.  Then set the macros in the User Macros section (under Debug and Release).

One last note on the code, a lot of the Python wrapper is automatically generated, with the CreateWrapper.py script.  If you need to
alter a Scintilla function, change the CreateWrapper.py script, not the generated code, or your changes will be lost next time CreateWrapper.py 
is run.  (This doesn't happen automatically, I just do it when I import a new Scintilla.iface file from the Scintilla project).


	

