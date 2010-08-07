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
   
   Not supplying stdout or stderr means that the default values from sys.stdout and sys.stderr (normally the console) are used.
   
   e.g.::
   
	   # The basic form, run a normal command line.
	   console.run('cl.exe "myfile.c"')
	   
	   # Create a new document in Notepad++
	   notepad.new()

	   # Run someprogram.exe, and have the output written to the new document.
	   console.run('someprogram.exe', editor)
	   
