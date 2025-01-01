﻿Introduction
============

.. highlight:: python
   :linenothreshold: 3

Python Script enables you to script every conceivable action you can perform in Notepad++.  If you want to run a macro 1000 times, do 15 different search and replaces in one go, then save the file under a different name, it's all possible, and, most importantly, easy.

Don't worry if you've not seen, or never even heard of Python_, it's incredibly easy to learn, and the beauty of it is that for the majority of scripts, you hardly need to know any Python at all.

To whet your appetite, here's a quick (complete) sample script that shows some of the power of Python Script.::
   
   editor.replace("old", "new")

   notepad.runMenuCommand("MIME Tools", "About")
   notepad.save()


Line 1 performs a normal search and replace on the current document, replacing the word "old" with the word "new".

Line 3 runs the menu command called "About" from the default Plugin MIME Tools menu.

Line 4 saves the current document (equivalent to clicking File, Save).


Objects  
========
There are 3 special objects you can use for manipulating Notepad++, and the text.

``notepad`` is the object for everything to do with Notepad++ itself, not the text in the document.  Things like opening and saving files, selecting different tabs, converting formats, running plugin commands and so on.

``editor`` is the object for everything to do with the text. It's actually a fairly complete wrapper for the Scintilla_ component, the edit component that Notepad++ uses for the text area.  This object has many functions, as Scintilla_ has many options and features that you can use - not all of them are natively supported in Notepad++, so you can actually customise it and do even more with Python Script than you can in Notepad++ alone.  There's also a healthy set of helper functions to assist in common functions (e.g. searching and replacing including Python's regular expressions and some useful functions for doing things common in scripts, like getting the start and end line numbers of the selection).  The helper functions just wrap up a series of Scintilla or Notepad++ commands, so they're just there to make life easier.

``editor1`` always refers to the "first" Scintilla window, normally on the left or the top.

``editor2`` always refers to the "second" Scintilla window, normally on the right or the bottom.

You normally only need to use ``editor1`` and ``editor2`` if you're doing something special with multiple views, or 
you're applying a setting, for instance in the ``startup.py`` script.

``console`` is the object for manipulating the console window. It's fairly simple, in that there's not much you can do - show it, clear it and write a message in it. 
However, there's also a ``run`` command, which runs a command line and diverts the output to the console.  So, you can use Python Script to call your compiler, or run any other command line tool.

Npp Module
==========

This section is really for *the Pythoneers*, but, for those of us that are just starting out, here's what you need to know.

The Npp module contains all the objects and classes that Python Script defines for manipulating Notepad++ (``notepad``, ``editor`` and ``console``).  The ``notepad`` object is an instance of class ``Notepad``, editor an instance of class ``Editor`` and ``console`` an instance of class ``Console``.

The ``startup.py`` script performs the following::

    from Npp import *

which effectively says that everything that's in Npp module, is now available locally.  So we can just use ``editor.getText()`` instead of ``Npp.editor.getText()``.  

If you create a new module (i.e. a new file), and want to use the functions defined in that module in a script (using ``import``), then you will need to either ``import Npp`` or ``from Npp import *`` at the top of your module to use the ``editor``, ``notepad`` and so on objects.

As the startup script runs in the same namespace as the scripts (__main__), you don't need to import the Npp module in the scripts.


Working with Unicode Text
=========================

Python 2.x works with multi-byte strings, as does Scintilla.  That means in most cases, you don't need to do anything special to deal with unicode data,
as both sides are talking the same language.  However, there are a few things to observe, and occasionally you'll need to do something special to achieve
what you want to do.  One important point is to make sure your script is saved in the same encoding as your target file(s) - this helps unicode strings 
be interpreted the same way. 

If you need to work with the string (for instance change the case), you need to convert the string to a Python Unicode string.  To convert the string
append ``.decode('utf8')`` to the string. Obviously if your string is in a different format, use the name of the correct encoding.

To put text back to Scintilla (so editor.something()), use .encode('utf8') from a unicode string.

For example::
	
	# define a unicode variable
	someUnicodeString = u'This häs fünny ünicode chäractêrs in it'
	
	# append the text to the current buffer - assuming the current buffer is set to utf8
	editor.addText(someUnicodeString.encode('utf8'))

	# grab the first line
	firstLine = editor.getLine(0)
	
	# convert it to unicode
	firstLineUnicode = firstLine.decode('utf8')
	
	# make it upper case
	firstLineUnicode = firstLineUnicode.upper()
	
	# and put the line back
	editor.replaceWholeLine(firstLineUnicode.encode('utf8')
	

.. _Notifications:

Handling Notifications
======================

Overview
--------

You can call a Python function when events occur in Notepad++ or Scintilla_. Events in Notepad++ are things like the active document changing, a file being opened or saved etc.  Events in Scintilla are things like a character being added, a *save point* being reached, the document being made *dirty* and so on.  

Basically, you register in a script a Python_ function to call when an event occurs, and thereafter the function always runs whenever that event occurs.  One function can be registered to handle more than one event.

You can unregister the callback later, either by using the name of the function, or the event names, or a combination.

A simple example
----------------

Let's register a callback for the FILEBEFORESAVE event - the occurs just before the file is saved, 
and we'll add a "saved on" log entry to the end of the file, if the filename ends in '.log'.::

	import datetime
	
	def addSaveStamp(args):
		if notepad.getCurrentFilename()[-4:] == '.log':
			editor.appendText("File saved on %s\r\n" % datetime.date.today())
		
	notepad.callback(addSaveStamp, [NOTIFICATION.FILEBEFORESAVE])

Line 1 imports the datetime module so we can get today's date.
	
Line 3 defines a function called ``addSaveStamp``. 

Line 4 checks that the extension of the file is '.log'.

Line 5 appends text like ``"File saved on 2009-07-15"`` to the file.

Line 7 registers the callback function for the FILEBEFORESAVE event.  Notice the square brackets around the ``NOTIFICATION.FILEBEFORESAVE``.  This is a list, and can contain more than one item (so that the function is called when any of the events are triggered).

Really, we should improve this function a little. Currently, it assumes the file being saved is the active document - in the case of using "Save All", it isn't necessarily.  However, it's easy to fix...

The ``args`` parameter to the function is a map (similar a dictionary in C# or a hashmap in Java), that contains the arguments for the event - many events are signalled for a ``BufferID``, which is the Notepad++ internal number for a particular file or tab.  We can do things with the bufferID like get the filename, switch to it to make it active and so on.

So, first we'll change it so that we check the filename of the bufferID being saved, rather than the active document. 
Then, if the filename has a '.log' extension, we'll change to it and add our "File saved on ....." line.::

	import datetime
	
	def addSaveStamp(args):
		if notepad.getBufferFilename(args["bufferID"])[-4:] == '.log':
			notepad.activateBufferID(args["bufferID"])
			editor.appendText("File saved on %s\r\n" % datetime.date.today())
		
	notepad.callback(addSaveStamp, [NOTIFICATION.FILEBEFORESAVE])



Great, now it works properly.  There's a side effect though, if we do use save-all, we might change the active document, 
which might seem a bit strange when we use it.  Again, very easy to fix.::


	import datetime

	def addSaveStamp(args):
		if notepad.getBufferFilename(args["bufferID"])[-4:] == '.log':
			currentBufferID = notepad.getCurrentBufferID()
			notepad.activateBufferID(args["bufferID"])
			editor.appendText("File saved on %s\r\n" % datetime.date.today())
			notepad.activateBufferID(currentBufferID)

	notepad.callback(addSaveStamp, [NOTIFICATION.FILEBEFORESAVE])

Now everything works as should, and it's nice and easy to see what's going on, and we leave the user with the same document they had open if they use Save-All.

See the :class:`NOTIFICATION` enum for more details on what arguments are provided for each notification, and the different events that are available.

Cancelling Callbacks
--------------------

You may wish to cancel a callback - perhaps you've finished editing log files, and want to run a script to disable the callback.

The simplest form is:: 

	notepad.clearCallbacks()

This unregisters all callbacks for all events.  If you want to just clear one or more events, just pass the list of :class:`NOTIFICATION` events you wish to clear.::

	notepad.clearCallbacks([NOTIFICATION.FILEBEFORESAVE, NOTIFICATION.FILESAVED])

*Note that if you want to clear the callback for just one event, you still need to pass a list (i.e. surrounded with square brackets)*

To unregister a callback for a particular function, just pass the function.::

	notepad.clearCallbacks(addSaveStamp)


To unregister a callback for a particular function, for particular events (perhaps you want to keep the function registered for FILEBEFORESAVE, but don't want FILESAVED anymore)

	notepad.clearCallbacks(addSaveStamp, [NOTIFICATION.FILESAVED])

*Note that redefining the function (in this case ``addSaveStamp``) will mean that this method no longer works, as the function name is now a new object.*
	  

The Callback smallprint
-----------------------

Due to the nature of Scintilla events, they are by default processed internally slightly differently to Notepad++ events.
Notepad++ events are always processed *synchronously*, i.e. your event handler finishes before Python Script lets 
Notepad++ continue.  Scintilla events are placed in a queue, and your event handlers are called as the queue is *asynchronously* processed
- i.e. the event completes before your event handler is complete (or potentially before your event handler is even called).

The only visible difference is that if you have a lot of callbacks registered, or your callbacks perform a lot of work, you might receive
the event some time after it has actually occurred.  In normal circumstances the time delay is so small it doesn't matter, but you may 
need to be aware of it if you're doing something time-sensitive.

 As of version 1.0, you can use :meth:`Editor.callbackSync` to add a synchronous callback. This allows you to perform time-sensitive 
 operations in an event handler. It also allows for calling :meth:`Editor.autoCCancel` in a ``SCINTILLANOTIFICATION.AUTOCSELECTION`` 
 notification to cancel the auto-complete.  Note that there are certain calls which cannot be made in a synchronous callback - 
 :meth:`Editor.findText`, :meth:`Editor.searchInTarget` and :meth:`Editor.setDocPointer` are notable examples.  :meth:`Notepad.createScintilla`
 and :meth:`Notepad.destroyScintilla` are other examples in the ``Notepad`` object - note that this only applies to Scintilla (``Editor``) callbacks,
 ``Notepad`` callbacks can perform any operation.



.. _Python: https://www.python.org/

.. _Scintilla: https://www.scintilla.org/
