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
	print(x)
	console.write(str(x))


If Statements
-------------

An ``if`` statement doesn't need brackets around the test, just a colon (:) at the end of the line.  The colon signifies the start of a new block.  The next line must be indented more than the ``if`` line.

The ``if`` block continues until the indentation reverts to the same indentation as the ``if`` line.::

	x=3
	console.write("Let's go\n")

	if x == 3:
		console.write("So x is 3 then\n")
		console.write("This line is also run if x is 3\n")
	console.write("this line is run always\n")

To use an else, place the else at the same level as the if, and append a colon::

	x=4
	if x == 3:
		console.write("So x is 3 then\n")
		console.write("This line is also run if x is 3\n")
	else:
		console.write("Guess x isn't 3 this time.\n")
		console.write("Better luck next time\n")

	console.write("this line is run always\n")

If you want an "else if", use ``elif``::

	x=7
	if x == 3:
		console.write("So x is 3 then\n")
		console.write("This line is also run if x is 3\n")
	elif x == 6:
		console.write("Hey it's your lucky day, x is 6\n")
	else:
		console.write("x is not 3 or 6 this time.\n")
		console.write("Better luck next time\n")


Functions
---------

To define a function, just use ``def``.  Again, the indentation is important, the function definition lasts as long as the indentation stays the same::

	def printNameAge(name, age):
		console.write("Hello {0}, ".format(name))
		console.write("how was your {0}th birthday?\n".format(age))

	printNameAge("Dave", 99)


Line 1 defines the function, the body of the function is lines 2 and 3.  Line 5 calls the function with the arguments "Dave" and 99.


You can read about the format() function (and what the ``{0}`` in the string means) in Python_'s documentation.

To pass the function to another function (as in callbacks in Python Script), just use the name of the function without brackets::


	def printNameAge(name, age):
		console.write("Hello {0}, ".format(name))
		console.write("how was your {0}th birthday?\n".format(age))


	def callFunc(func):
		console.write("Calling a function...\n")
		func("Dave", 99)


	callFunc(printNameAge)


Line 11 calls the ``callFunc`` function, passing the printNameAge function to it.  The ``callFunc`` function then calls printNameAge on line 8.


Scope
-----

One other important benefit of using functions is that they will create a *local scope*: the variables you define inside the function definition will not be available to code outside this region (while from inside the region, you can still access variables from outside). 
Variables, and functions, defined at top level of the script, outside functions definitions, are in what is called *global scope*: accessible from everywhere. 
You can see this by running the following 3 scripts:
Script1:

	x=1
	print(x)

Script2:

	print(x)
	x=2

Script3:

	print(x)

Most of the time, allowing scripts to create and access mutually accessible variable can lead to nasty bugs. 
One simple and effective way to avoid that is to use the following structure for EVERY script that you write, at least in your beginner's journey:

	def main():
		// the "meat" of your code

	main()

Not only it prevents bad script inter-action, but it will also allow you to use `return` (among other ways) to terminate your script early, if needed (see the Warning on the danger of `exit()` in Introduction section, if you haven't). 


Strings
-------

Strings use double or single quotes.  There's no difference, but that helps if you want to include one type or other in the string.::

	someString = "this is a string with a ' single quote inside"
	someOther  = 'this is a string with a " double quote inside'
	print(someString)
	print(someOther)

If you need both, just use ``\`` to escape them::

	someString = "here's a string with both types of \" in it"

If you need to use ``\`` itself, just include two::

	someString = "here's a demo of using a \\ backslash"


You can also use ``raw`` strings.  These don't need the ``\\`` to mean ``\``, which can make certain strings (particularly regular expressions) easier to understand.  However, you also can't escape a quote.  
To make a raw string, just put an ``r`` before the string::

	someString = r"this is also a string using a \\ backslash"

Exception: you cannot use a backslash right before the ending quote; it will escapes that quote, producing an error. 


Further Information
-------------------

This guide may be extended in the future, if people request it, and either I get time, or somebody puts some time into it.  There are many good sources of information on the Internet, I've included a couple here.

https://docs.python.org/tutorial/

https://www.tutorialspoint.com/python/python_quick_guide.htm

And all the information is available on https://www.python.org

 
 
.. _Python: https://www.python.org/

.. _Scintilla: https://www.scintilla.org/
