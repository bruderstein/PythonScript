Python Primer
=============

.. highlight:: python
   :linenothreshold: 3
   
   
Introduction
------------

This is a little introduction to Python_.  It is not meant to teach you Python_, just give you enough syntax to write scripts for Notepad++.  

Basics
------

The key, is indentation.  That separates out the blocks of code.  Some people initially don't like this approach, thinking it might be unclear or easy to get wrong - I did.  Now I think it's just great :)

There are no semi-colons (;) at the end of lines like in C or Java.

You can define a variable simply by using it.::

	x = 5

To call a function, just type the name, followed by brackets, with any arguments in brackets.::

	x = 5
	console.write(x)


If Statements
-------------

An ``if`` statement doesn't need brackets around the test, just a colon (:) at the end of the line.  The colon signifies the start of a new block.  The next line must be indented more than the ``if`` line.

The ``if`` block continues until the indentation reverts to the same indentation as the ``if`` line.::

	
	console.write("Let's go")

	if x == 3:
		console.write("So x is 3 then")
		console.write("This line is also run if x is 3")
	console.write("this line is run always")

To use an else, place the else at the same level as the if, and append a colon::

	if x == 3:
		console.write("So x is 3 then")
		console.write("This line is also run if x is 3")
	else:
		console.write("Guess x isn't 3 this time.")
		console.write("Better luck next time")

	console.write("this line is run always")

If you want an "else if", use ``elif``::

	if x == 3:
		console.write("So x is 3 then")
		console.write("This line is also run if x is 3")
	elif x == 6:
		console.write("Hey it's your lucky day, x is 6")
	else:
		console.write("x is not 3 or 6 this time.")
		console.write("Better luck next time")


Functions
---------

To define a function, just use ``def``.  Again, the indentation is important, the function definition lasts as long as the indentation stays the same::

	def printNameAge(name, age):
		console.write("Hello {0}, ".format(name))
		console.write("how was your {0} birthday?".format(age))

	printNameAge("Dave", 99)


Line 1 defines the function, the body of the function is lines 2 and 3.  Line 5 calls the function with the arguments "Dave" and 99.


You can read about the format() function (and what the ``{0}`` in the string means) in Python_'s documentation.

To pass the function to another function (as in callbacks in Python Script), just use the name of the function without brackets::


	def printNameAge(name, age):
		console.write("Hello {0}, ".format(name))
		console.write("how was your {0} birthday?".format(age))


	def callFunc(func)
		console.write("Calling function...\n")
		func("Dave", 99)


	callFunc(printNameAge)


Line 11 calls the ``callFunc`` function, passing the printNameAge function to it.  The ``callFunc`` function then calls printNameAge on line 8.





Strings
-------

Strings use double or single quotes.  There's no difference, but that helps if you want to include one type or other in the string.::

	someString = "this is a string with a ' single quote inside"
	someOther  = 'this is a string with a " double quote inside'

If you need both, just use ``\`` to escape them::

	someString = "here's a string with both types of \" in it"

If you need to use ``\`` itself, just include two::

	someString = "here's a demo of using a \\ backslash"


You can also use ``raw`` strings.  These don't need the ``\\`` to mean ``\``, which can make certain strings (particularly regular expressions) easier to understand.  However, you also can't escape a quote.  To make a raw string, just put an ``r`` before the string

	someString = r"this is also a string using a \ backslash"


Further Information
-------------------

This guide may be extended in the future, if people request it, and either I get time, or somebody puts some time into it.  There are many good sources of information on the Internet, I've included a few here.

<http://www.python.org>

 
 
.. _Python: http://www.python.org/

.. _Scintilla: http://www.scintilla.org/