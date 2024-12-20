Notepad++ Object
================

.. class:: Notepad


.. property:: hwnd

   Returns the handle of the Notepad++ window.


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


.. method:: notepad.allocateCmdID(numberRequested) -> int

   Allocates a range of Command ID for use in WM_COMMAND. Mainly used internally by plugins.

   Returns the start number of the requested range


.. method:: notepad.allocateMarker(numberRequested) -> int

   Allocates a range of marker numbers for Scintilla.
   Use this to stop marker number collisions with other plugins / scripts.

   Returns the start number of the requested range

.. method:: notepad.allocateIndicator(numberRequested) -> int

   Allocates a range of indicator numbers for Scintilla.
   Use this to stop indicator number collisions with other plugins / scripts.

   Returns the start number of the requested range


.. method:: notepad.allocateSupported() -> bool

   Returns ``True`` if the Command ID allocation API is supported in this version of Notepad++


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

   For Scintilla notifications, see ``editor.callback()`` -> bool

   Returns ``True`` if the registration was successful


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

.. method:: notepad.createScintilla() -> editor

   Create a new Scintilla handle.

   Returns an :ref:`Editor`


.. method:: notepad.decodeSci(view) -> int

   View is either 0 or 1

   Returns the old unicode mode


.. method:: notepad.destroyScintilla(editor)

   Destroy a Scintilla handle created with createScintilla


.. method:: notepad.disableAutoUpdate()

   Disables notepad++ auto update functionality


.. method:: notepad.docSwitcherDisableColumn(boolean)

   ``True`` if it should be hidden, ``False`` if it should be shown


.. method:: notepad.encodeSci(view) -> int

   View is either 0 or 1

   Returns the new unicode mode


.. method:: notepad.flashWindow(count, milliseconds)

   Flashes notepad++ for the given count and milliseconds


.. method:: notepad.getAppdataPluginsAllowed() -> bool

   Returns ``True`` if loading plugins from %APPDATA%\\Notepad++\\plugins is allowed, ``False`` otherwise


.. method:: notepad.getBookMarkID()

   Gets the bookmark marker ID


.. method:: notepad.getBufferFilename(bufferID)

   Gets the filename of the given buffer ID


.. method:: notepad.getCommandLine()

   Gets the command line used to start Notepad++


.. method:: notepad.getCurrentBufferID()

   Gets the bufferID of the currently active buffer


.. method:: notepad.getCurrentDocIndex(view)

   Gets the current active index for the given *view* (0 or 1)


.. method:: notepad.getCurrentFilename()

   Gets the filename of the active document


.. method:: notepad.getCurrentLang() -> LANGTYPE

   Get the current language type

   Returns :class:`LANGTYPE`


.. method:: notepad.getCurrentNativeLangEncoding() -> int

   Returns the current native language encoding


.. method:: notepad.getCurrentView()

   Get the currently active view (0 or 1)


.. method:: notepad.getEditorDefaultBackgroundColor() -> tuple

   Returns the default background color as tuple


.. method:: notepad.getEditorDefaultForegroundColor() -> tuple

   Returns the default foreground color as tuple


.. method:: notepad.getEnableThemeTextureFunc() -> bool

   Returns ``True`` if a dialog window supports background texturing, ``False`` otherwise.


.. method:: notepad.getEncoding([bufferID]) -> BUFFERENCODING

   Gets the encoding of the given *bufferID*.  If no bufferID is given, then the encoding of the
   currently active buffer is returned.

   Returns :class:`BUFFERENCODING`


.. method:: notepad.getExternalLexerAutoIndentMode() -> AUTOINDENTMODE

   Gets the auto indent mode for an installed external programming language

   Returns :class:`AUTOINDENTMODE`


.. method:: notepad.getFiles() -> list

   Gets a list of the open filenames.

   Returns A list of tuples containing ``(filename, bufferID, index, view)``


.. method:: notepad.getFormatType([bufferID]) -> FORMATTYPE

   Gets the format type (i.e. Windows, Unix or Mac) of the given *bufferID*.
   If no bufferID is given, then the format of the currently active buffer is returned.

   Returns :class:`FORMATTYPE`


.. method:: notepad.getLanguageDesc(langType) -> LANGTYPE

   Returns the programming language short description from the given :class:`LANGTYPE`


.. method:: notepad.getLanguageName(langType) -> LANGTYPE

   Returns the programming language name from the given :class:`LANGTYPE`


.. method:: notepad.getLangType([bufferID]) -> LANGTYPE

   Gets the language type of the given *bufferID*.
   If no bufferID is given, then the language of the currently active buffer is returned.

   Returns :class:`LANGTYPE`


.. method:: notepad.getLineNumberWidthMode() -> LINENUMWIDTHMODE

   Gets the line number margin width mode

   Returns :class:`LINENUMWIDTHMODE`


.. method:: notepad.getNativeLangFileName() -> str

   Gets the current native language file name


.. method:: notepad.getTabColorID([view, [tabIndex] ]) -> int

   Gets the tab color id for the given view and tab index
   If no view or tabIndex is given, then the currently active buffer is used.


.. method:: notepad.getNppDir() -> str

   Gets the directory Notepad++ is running in (i.e. the location of notepad++.exe)


.. method:: notepad.getMenuHandle(menu)

   Menu is either 0(main) or 1(plugin) -> int

   Returns the handle for the main or plugins menu.


.. method:: notepad.getNbUserLang() -> int

   Returns the number of user defined languages


.. method:: notepad.getPluginConfigDir() -> str

   Gets the plugin config directory.


.. method:: notepad.getPluginMenuHandle() -> int

   Gets the handle for the Plugins menu.


.. method:: notepad.getPluginVersion() -> str

   Gets the PythonScript plugin version as a string. There is always four parts to it. e.g. '0.9.2.0'


.. method:: notepad.getSessionFiles(sessionFileName) -> list

   Returns a list of files from given session file


.. method:: notepad.getVersion() -> tuple

   Gets the Notepad++ version as a tuple - e.g. 5.6.8 becomes ``(5,6,8)``


.. method:: notepad.getWindowsVersion() -> WINVER

   Returns current windows version :class:`WINVER`


.. method:: notepad.hideMenu(boolean)

   ``True`` if it should be hidden, ``False`` if it should be shown


.. method:: notepad.hideStatusBar(boolean)

   ``True`` if it should be hidden, ``False`` if it should be shown


.. method:: notepad.hideTabBar()

   Hides the Tab bar


.. method:: notepad.hideToolBar(boolean)

   ``True`` if it should be hidden, ``False`` if it should be shown


.. method:: notepad.isAutoIndention() -> bool

   Returns: ``True`` if auto indent mode is set, ``False`` otherwise


.. method:: notepad.isDarkModeEnabled() -> bool

   Returns ``True`` if Dark Mode is enabled else ``False``


.. method:: notepad.isDocSwitcherShown() -> bool

   Returns ``True`` if document switcher is shown else ``False``


.. method:: notepad.isMenuHidden() -> bool

   Returns ``True`` if menu is hidden else ``False``


.. method:: notepad.isSingleView()

   ``True`` if only one view is used, ``False`` otherwise


.. method:: notepad.isStatusBarHidden() -> bool

   Returns ``True`` if statusbar is hidden else ``False``


.. method:: notepad.isTabBarHidden() -> bool

   Returns ``True`` if tabbar is hidden else ``False``


.. method:: notepad.isToolBarHidden() -> bool

   Returns ``True`` if toolbar is hidden else ``False``


.. method:: notepad.launchFindInFilesDlg(dir2Search, filter)

   Launches the find in files dialog for given directory and filter arguments


.. method:: notepad.loadSession(sessionFileName)

   Loads a session from a session file


.. method:: notepad.makeCurrentBufferDirty()

   Makes current document dirty


.. method:: notepad.menuCommand(menuCommand)

   Runs a Notepad++ menu command.  Use the :class:`MENUCOMMAND` enum, or integers directly from the nativeLang.xml file.


.. method:: notepad.messageBox(message[, title[, flags]]) -> MessageBoxFlags

   Displays a message box with the given *message* and *title*.

   Flags can be 0 for a standard 'OK' message box, or a combination of :class:`MESSAGEBOXFLAGS`.
   ``title`` is "Python Script for Notepad++" by default, and flags is 0 by default.

   Returns a RESULTxxxx member of :class:`MESSAGEBOXFLAGS` as to which button was pressed.


.. method:: notepad.new()

   Create a new document.


.. method:: notepad.open(filename)

   Opens the given file.


.. method:: notepad.prompt(prompt, title[, defaultText]) -> str

   Prompts the user for some text.  Optionally provide the default text to initialise the entry field.

   Returns the string entered.

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

   Returns ``True`` if the menu command was found, otherwise ``False``

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


.. method:: notepad.saveFile(filename)

   Saves the provided file - must not be the active one


.. method:: notepad.saveCurrentSession(filename)

   Save the current session (list of open files) to a file.


.. method:: notepad.saveSession(filename, filesList)

   Saves a session file with the list of filenames.


.. method:: notepad.setCurrentLang(langType)

   Set the language type of the currently active buffer (see :class:`LANGTYPE`)


.. method:: notepad.setEditorBorderEdge(boolean)

   ``True`` if editor should use border edge, ``False`` otherwise


.. method:: notepad.setEncoding(encoding)

   Sets the encoding of the current buffer.  Use the :class:`BUFFERENCODING` constants


.. method:: notepad.setEncoding(encoding, bufferID)

   Sets the encoding of the given bufferID.  Use the :class:`BUFFERENCODING` constants


.. method:: notepad.setExternalLexerAutoIndentMode(externalLexerName, indentMode) -> bool

   Sets the auto indent mode for an external programming language

   Returns: ``True`` if indentMode was set successful, ``False`` otherwise


.. method:: notepad.setFormatType(formatType[, bufferID])

   Sets the format type (i.e. Windows, Unix or Mac) of the specified buffer ID.
   If not bufferID is passed, then the format type of the currently active buffer is set.


.. method:: notepad.setLangType(langType[, bufferID])

   Sets the language type of the given *bufferID*. If not bufferID is given, sets the language for the currently active buffer.


.. method:: notepad.setSmoothFont(boolean)

   ``True`` if smooth font should be set, ``False`` otherwise


.. method:: notepad.setLineNumberWidthMode(widthMode) -> bool

   Sets the line number margin width mode

   Returns: ``True`` if widthMode was set successful, ``False`` otherwise


.. method:: notepad.setStatusBar(statusBarSection, text)

   Sets the status bar text. For statusBarSection, use one of the :class:`STATUSBARSECTION` constants.


.. method:: notepad.setUntitledName([bufferID])

   Sets a new name for an unnamed tab. If no bufferID is specified, the current tab is assumed


.. method:: notepad.showDocSwitcher(boolean)

   ``True`` if it should be hidden, ``False`` if it should be shown


.. method:: notepad.showTabBar()

   Shows the Tab bar


.. method:: notepad.triggerTabbarContextMenu(view, index2Activate)

   Activates the context menu for given view and tab index



