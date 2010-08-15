Console Object
==============

.. class:: Console



.. method:: Console.write(string)
      
   Writes text to the console.
   
.. method:: Console.writeError(string)

   Writes text to the console in red
   
.. method:: Console.clear()

   Clears the console


.. method:: Console.show()
  
   Shows the console window.

.. method:: Console.hide()

   Hides the console window

.. method:: Console.run(commandLine[, stdout[, stderr]])

   Runs an external program, with output optionally directed to the given objects (which must support a `write` method).
   
   Not supplying stdout means that the console is used for stdout.
   
   Not supplying stderr means that the default values from sys.stderr (normally the console, in red) is used.
   
   e.g.::
   
	   # The basic form, run a normal command line.
	   console.run('cl.exe "myfile.c"')
	   
	   # Create a new document in Notepad++
	   notepad.new()

	   # Run someprogram.exe, and have the output written to the new document.
	   console.run('someprogram.exe', editor)
	   
   
   The console will automatically parse Python, Visual Studio and GCC style errors and warnings. The filenames are 
   then clickable to open the file at the line number given.  If you would like some other form of warnings or errors
   parsed, please send me an email (davegb@pobox.com).  Also if you find a line that is parsed as an error or warning 
   and it shouldn't have been, also let me know.
   
   
.. attribute:: Console.editor

   An :ref:`Editor` object for the console window.  This enables you to change colours, styles, even add and remove text if
   you so wish, from the console window.  Note that the console window is always left in a read-only state, so in order to change
   text, you would need to first perform a ``console.editor.setReadOnly(0)``.  Any subsequent ``console.write`` or ``console.writeError`` 
   calls will make the console read-only again.
   
   Console Styles
   --------------
   
   The styles use a bit mask to represent different things within the console.
   
   +--------+---------+------------------------------+
   | Bit    | Value   | Meaning                      |
   +--------+---------+------------------------------+
   | 0      | 1       | STDERR output, or a parsed   |
   |        |         | error                        |
   +--------+---------+------------------------------+
   | 1      | 2       | Hotspot - clickable link     |
   +--------+---------+------------------------------+
   | 2      | 4       | A parsed warning             |
   +--------+---------+------------------------------+
   
   This produces the following style values:
   
   +--------+---------------------------------------+
   | Style  | Description                           |
   +--------+---------------------------------------+
   | 0      | Standard (default) style              |
   +--------+---------------------------------------+
   | 1      | STDERR or error                       |
   +--------+---------------------------------------+
   | 2      | Hotspot clickable link, but not error |
   |        | or warning                            |
   +--------+---------------------------------------+
   | 3      | Hotspot clickable link, Error         |
   +--------+---------------------------------------+
   | 4      | Warning, but not clickable            |
   +--------+---------------------------------------+
   | 5      | Warning produced on stderr, not       |
   |        | clickable                             |
   +--------+---------------------------------------+
   | 6      | Clickable warning, from stdout        |
   +--------+---------------------------------------+
   | 7      | Clickable warning on stderr           |
   +--------+---------------------------------------+
   
   