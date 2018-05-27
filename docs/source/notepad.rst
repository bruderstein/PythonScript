Notepad++ Object
================

.. class:: Notepad



.. method:: notepad.activateBufferID(bufferID)
      
   Activates the given *bufferID*::
   
	  bufferID = notepad.getCurrentBufferID()
	  ...
	  notepad.activateBufferID(bufferID)
  

.. method:: notepad.activateFile(filename)
	
   Activates the document with the given filename
    
   

.. method:: notepad.activateIndex(view, index)

   Activates the document with the given view and index.
   *view* is 0 or 1.
    

.. method:: notepad.callback(function, notifications)
   
   Registers a callback function for a notification. 
   *notifications* is a list of messages to call the function for.::
      
	  def my_callback(args):
		  console.write("Buffer Activated %d\n" % args["bufferID"]
		
	  notepad.callback(my_callback, [NOTIFICATION.BUFFERACTIVATED])
	  
   The :class:`NOTIFICATION` enum corresponds to the NPPN_* plugin notifications. 
   The function arguments is a map, and the contents vary dependant on the notification.
   
   Note that the callback will live on past the life of the script, so you can use this 
   to perform operations whenever a document is opened, saved, changed etc.  
   
   Also note that it is good practice to put the function in another module (file), and
   then import that module in the script that calls ``notepad.callback()``.  This way
   you can unregister the callback easily.
   
   For Scintilla notifications, see ``editor.callback()``
   
   Returns:
		``True`` if the registration was successful
		
   
.. method:: notepad.clearCallbacks()
   
   Unregisters all callbacks
    
.. method:: notepad.clearCallbacks(function)
  
   Unregisters all callbacks for the given function.  Note that this uses the actual function object, so if the function has
   been redefined since it was registered, this will fail.  If this has happened, use one of the other ``clearCallbacks()`` 
   functions.
   
   
  
.. method:: notepad.clearCallbacks(eventsList)
	
   Unregisters all callbacks for the given list of events.::
   
		notepad.clearCallbacks([NOTIFICATION.BUFFERACTIVATED, NOTIFICATION.FILESAVED])
   
   See :class:`NOTIFICATION`
    
.. method:: notepad.clearCallbacks(function, eventsList)
        
   Unregisters the callback for the given callback function for the list of events.
    
        
.. method:: notepad.close()

   Closes the currently active document

   
.. method:: notepad.closeAll()
       
   Closes all open documents
    

.. method:: notepad.closeAllButCurrent()
   
   Closes all but the currently active document
    
.. method:: notepad.createScintilla()

   Create a new Scintilla handle. Returns an Editor object
    

.. method:: notepad.destroyScintilla(editor)

   Destroy a Scintilla handle created with createScintilla
    
.. method:: notepad.getCommandLine()

   Gets the command line used to start Notepad++
   
	
.. method:: notepad.getCurrentBufferID()
       
   Gets the bufferID of the currently active buffer

   
.. method:: notepad.getCurrentDocIndex(view)
   
   Gets the current active index for the given *view* (0 or 1)
    

.. method:: notepad.getCurrentFilename()

   Gets the filename of the active document

.. method:: notepad.getCurrentLang()
   
   Get the current language type 
   
   Returns:
     :class:`LANGTYPE`
   

.. method:: notepad.getCurrentView()
   
   Get the currently active view (0 or 1)

.. method:: notepad.getEncoding([bufferID]) -> BUFFERENCODING

   Gets the encoding of the given *bufferID*.  If no bufferID is given, then the encoding of the 
   currently active buffer is returned.
   
   Returns:
		:class:`BUFFERENCODING`
    

.. method:: notepad.getFiles()
    
   Gets a list of the open filenames.
   
   Returns:
		A list of tuples containing ``(filename, bufferID, index, view)``
    

.. method:: notepad.getFormatType([bufferID]) -> FORMATTYPE
   
   Gets the format type (i.e. Windows, Unix or Mac) of the given *bufferID*.  
   If no bufferID is given, then the format of the currently active buffer is returned.
    
   Returns:
		:class:`FORMATTYPE`

		
.. method:: notepad.getLangType([bufferID]) -> LANGTYPE
   
   Gets the language type of the given *bufferID*. 
   If no bufferID is given, then the language of the currently active buffer is returned.

   Returns:
		:class:`LANGTYPE`
		
.. method:: notepad.getNppDir() -> str

   Gets the directory Notepad++ is running in (i.e. the location of notepad++.exe)
   
 

.. method:: notepad.getPluginConfigDir() -> str
   
   Gets the plugin config directory.
    

.. method:: notepad.getPluginMenuHandle() -> int

   Gets the handle for the Plugins menu.
    

.. method:: notepad.getVersion() -> tuple
   
   Gets the Notepad++ version as a tuple - e.g. 5.6.8 becomes ``(5,6,8)``

.. method:: notepad.getPluginVersion() -> str

   Gets the PythonScript plugin version as a string. There is always four parts to it. e.g. '0.9.2.0'
   
.. method:: notepad.hideTabBar()

   Hides the Tab bar
    

.. method:: notepad.menuCommand(menuCommand)
    
   Runs a Notepad++ menu command.  Use the :class:`MENUCOMMAND` enum, or integers directly from the nativeLang.xml file. 
    

.. method:: notepad.messageBox(message[, title[, flags]]) -> MessageBoxFlags
    
   Displays a message box with the given *message* and *title*.  
   
   Flags can be 0 for a standard 'OK' message box, or a combination of :class:`MESSAGEBOXFLAGS`.
   ``title`` is "Python Script for Notepad++" by default, and flags is 0 by default.
   
   Returns:
      A RESULTxxxx member of :class:`MESSAGEBOXFLAGS` as to which button was pressed.
	

.. method:: notepad.new()
   
   Create a new document.
   
.. method:: notepad.open(filename)

   Opens the given file.
   

.. method:: notepad.prompt(prompt, title[, defaultText]) -> str
    
   Prompts the user for some text.  Optionally provide the default text to initialise the entry field.
		
   Returns:
        The string entered.  
        
		``None`` if cancel was pressed 
		(note that is different to an empty string, which means that no input was given)
 
 
.. method:: notepad.reloadBuffer(bufferID)
    
   Reloads the given bufferID
    

.. method:: notepad.reloadCurrentDocument()
       
   Reloads the current document
    

.. method:: notepad.reloadFile(filename)
    
   Reloads a filename.

.. method:: notepad.runMenuCommand(menuName, menuOption[, refreshCache]) -> bool
    
   Runs a command from the menus. 
   For built-in menus use notepad.menuCommand(), for non built-in menus (e.g. TextFX and macros you've defined), 
   use ``notepad.runMenuCommand(menuName, menuOption)``.  For other plugin commands (in the plugin menu),
   use ``notepad.runPluginCommand(pluginName, menuOption)_``
   
   Menus are searched for the text, and when found, the internal ID of the menu command is cached. 
   When ``runMenuCommand`` is called, the cache is first checked if it holds the internal ID for the given 
   ``menuName`` and ``menuOption``.  If it does, it simply uses the value from the cache.  If the ID could have
   been updated (for example, you're calling the name of macro that has been removed and added again), set refreshCache 
   to ``True``.  This is ``False`` by default.
   
   Returns:
		``True`` if the menu command was found, otherwise ``False``
   
   e.g.::
         
		notepad.runMenuCommand('TextFX Edit', 'Delete Blank Lines')
    
        
.. method:: notepad.runPluginCommand(pluginName, menuOption[, refreshCache])
    
	Runs a command from the plugin menu.
	Use to run direct commands from the Plugins menu.
	To call TextFX or other menu functions, either use ``notepad.menuCommand(menuCommand)_`` (for Notepad++ menu commands), 
	or ``notepad.runMenuCommand(menuName, menuOption)_`` for TextFX or non standard menus (such as macro names).
	
	Note that menuOption can be a submenu in a plugin's menu.  So::
	
		notepad.runPluginCommand('Python Script', 'demo script')
		
	Could run a script called "demo script" from the Scripts submenu of Python Script.
	
	Menus are searched for the text, and when found, the internal ID of the menu command is cached. 
	When ``runPluginCommand`` is called, the cache is first checked if it holds the internal ID for the given 
	``menuName`` and ``menuOption``.  If it does, it simply uses the value from the cache.  If the ID could have
	been updated (for example, you're calling the name of macro that has been removed and added again), set refreshCache 
	to ``True``.  This is ``False`` by default.
	
	e.g.::
		notepad.runPluginCommand('XML Tools', 'Pretty Print (XML only)')
		

.. method:: notepad.save()
    
   Save the current file

.. method:: notepad.saveAllFiles()

   Saves all currently unsaved files

.. method:: notepad.saveAs(filename)
    
   Save the current file as the specified filename
   
   *Only works in Notepad++ 5.7 onwards*
        

.. method:: notepad.saveAsCopy(filename)

   Save the current file as the specified filename, but don't change the filename for the buffer in Notepad++
   
   *Only works in Notepad++ 5.7 onwards*

   
.. method:: notepad.saveCurrentSession(filename)

   Save the current session (list of open files) to a file.

   
.. method:: notepad.saveSession(filename, filesList)
    
   Saves a session file with the list of filenames.

   
.. method:: notepad.setCurrentLang(langType)

	
   Set the language type of the currently active buffer (see :class:`LANGTYPE`)
   
   
.. method:: notepad.setFormatType(formatType[, bufferID])

   Sets the format type (i.e. Windows, Unix or Mac) of the specified buffer ID. 
   If not bufferID is passed, then the format type of the currently active buffer is set.
    
    
.. method:: notepad.setLangType(langType[, bufferID])
    
   Sets the language type of the given *bufferID*. If not bufferID is given, sets the language for the currently active buffer.
    

.. method:: notepad.setStatusBar(statusBarSection, text)

   Sets the status bar text. For statusBarSection, use one of the :class:`STATUSBARSECTION` constants.

.. method:: notepad.showTabBar()
   
   Shows the Tab bar

   
