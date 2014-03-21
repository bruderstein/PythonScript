Enums
=====


FORMATTYPE
----------

.. _FORMATTYPE:
.. class:: FORMATTYPE

.. attribute:: FORMATTYPE.MAC

.. attribute:: FORMATTYPE.UNIX

.. attribute:: FORMATTYPE.WIN





BUFFERENCODING
--------------

.. _BUFFERENCODING:
.. class:: BUFFERENCODING

.. attribute:: BUFFERENCODING.ANSI

.. attribute:: BUFFERENCODING.COOKIE

.. attribute:: BUFFERENCODING.ENC8BIT

.. attribute:: BUFFERENCODING.UCS2BE

.. attribute:: BUFFERENCODING.UCS2BE_NOBOM

.. attribute:: BUFFERENCODING.UCS2LE

.. attribute:: BUFFERENCODING.UCS2LE_NOBOM

.. attribute:: BUFFERENCODING.UTF8




STATUSBARSECTION
----------------

.. _STATUSBARSECTION:
.. class:: STATUSBARSECTION

.. attribute:: STATUSBARSECTION.CURPOS

.. attribute:: STATUSBARSECTION.DOCSIZE

.. attribute:: STATUSBARSECTION.DOCTYPE

.. attribute:: STATUSBARSECTION.EOFFORMAT

.. attribute:: STATUSBARSECTION.TYPINGMODE

.. attribute:: STATUSBARSECTION.UNICODETYPE


SCINTILLANOTIFICATION
---------------------
.. _SCINTILLANOTFICATION:
.. class:: SCINTILLANOTFICATION
.. attribute:: SCINTILLANOTIFICATION.STYLENEEDED

   Arguments contains: ``position``
   
.. attribute:: SCINTILLANOTIFICATION.CHARADDED

   Arguments contains: ``ch`` - the character added (as an int)
   
.. attribute:: SCINTILLANOTIFICATION.SAVEPOINTREACHED

.. attribute:: SCINTILLANOTIFICATION.SAVEPOINTLEFT

.. attribute:: SCINTILLANOTIFICATION.MODIFYATTEMPTRO

.. attribute:: SCINTILLANOTIFICATION.KEY

   Arguments contains: ``ch`` - the key code, ``modifiers``, elements from the KEYMOD enum
   
.. attribute:: SCINTILLANOTIFICATION.DOUBLECLICK

   Arguments contains: ``position`` (in the file), ``modifiers`` (from KEYMOD), ``line``, line number
   
.. attribute:: SCINTILLANOTIFICATION.UPDATEUI

.. attribute:: SCINTILLANOTIFICATION.MODIFIED
	
   Arguments contains: ``position``, ``modificationType`` (a set of flags from :class:`MODIFICATIONFLAGS`), ``text``, ``length``, ``linesAdded``, ``line``, ``foldLevelNow``, ``foldLevelPrev``,
   ``annotationLinesAdded`` (only for :attr:`MODIFICATIONFLAGS.CHANGEANNOTATION`), ``token`` (only for :attr:`MODIFICATIONFLAGS.CONTAINER`)

.. attribute:: SCINTILLANOTIFICATION.MACRORECORD

   Arguments contains: ``message``, ``wParam``, ``lParam``
	
.. attribute:: SCINTILLANOTIFICATION.MARGINCLICK

   Arguments contains: ``margin``
   
.. attribute:: SCINTILLANOTIFICATION.NEEDSHOWN

.. attribute:: SCINTILLANOTIFICATION.PAINTED

   Note: Because Scintilla events are processed by Python asynchronously, care must be taken if handling a callback for this event
   - the window may have just been painted, or it may have been painted again since etc.

.. attribute:: SCINTILLANOTIFICATION.USERLISTSELECTION

   Arguments contains: ``text``, ``listType``, ``position``

.. attribute:: SCINTILLANOTIFICATION.URIDROPPED

.. attribute:: SCINTILLANOTIFICATION.DWELLSTART

   Arguments contains: ``position``, ``x``, ``y``
   
.. attribute:: SCINTILLANOTIFICATION.DWELLEND

   Arguments contains: ``position``, ``x``, ``y``
   
.. attribute:: SCINTILLANOTIFICATION.ZOOM

.. attribute:: SCINTILLANOTIFICATION.HOTSPOTCLICK

   Arguments contains: ``position``, ``modifiers`` (from the :class:`KEYMOD` enum)
   
.. attribute:: SCINTILLANOTIFICATION.HOTSPOTDOUBLECLICK

   Arguments contains: ``position``, ``modifiers`` (from the :class:`KEYMOD` enum)

.. attribute:: SCINTILLANOTIFICATION.CALLTIPCLICK

   Arguments contains: ``position``

.. attribute:: SCINTILLANOTIFICATION.AUTOCSELECTION

   Arguments contains: ``text``, ``position``
   
.. attribute:: SCINTILLANOTIFICATION.INDICATORCLICK

.. attribute:: SCINTILLANOTIFICATION.INDICATORRELEASE

.. attribute:: SCINTILLANOTIFICATION.AUTOCCANCELLED

.. attribute:: SCINTILLANOTIFICATION.AUTOCCHARDELETED

.. attribute:: SCINTILLANOTIFICATION.FOCUSIN

.. attribute:: SCINTILLANOTIFICATION.FOCUSOUT


SCINTILLAMESSAGE
----------------

.. _SCINTILLAMESSAGE:
.. class:: SCINTILLAMESSAGE

   This enum contains all the messages defined in Scintilla - e.g. SCI_APPENDTEXT.  For a complete list, see the `Scintilla <http://www.scintilla.org/ScintillaDoc.html>`_ documentation.


NOTIFICATION
------------

.. _NOTIFICATION:
.. class:: NOTIFICATION

.. attribute:: NOTIFICATION.BUFFERACTIVATED

   Arguments contains: ``bufferID``

.. attribute:: NOTIFICATION.FILEBEFORECLOSE
		
   Arguments contains: ``bufferID``
   
.. attribute:: NOTIFICATION.FILEBEFORELOAD

.. attribute:: NOTIFICATION.FILEBEFOREOPEN

   Arguments contains: ``bufferID``

.. attribute:: NOTIFICATION.FILEBEFORESAVE

   Arguments contains: ``bufferID``

.. attribute:: NOTIFICATION.FILECLOSED

   Arguments contains: ``bufferID``

.. attribute:: NOTIFICATION.FILELOADFAILED

   Arguments contains: ``bufferID``

.. attribute:: NOTIFICATION.FILEOPENED

   Arguments contains: ``bufferID``

.. attribute:: NOTIFICATION.FILESAVED

.. attribute:: NOTIFICATION.LANGCHANGED

   Arguments contains: ``bufferID``

.. attribute:: NOTIFICATION.READONLYCHANGED

   Arguments contains: ``bufferID``, ``readonly``, and ``dirty``.  ``dirty`` is set to ``True`` if the file is currently marked as dirty.

.. attribute:: NOTIFICATION.READY

.. attribute:: NOTIFICATION.SHORTCUTREMAPPED

   Arguments contains: ``commandID``, ``key``, ``isCtrl``, ``isAlt`` and ``isShift``.
   
.. attribute:: NOTIFICATION.SHUTDOWN

.. attribute:: NOTIFICATION.TBMODIFICATION

.. attribute:: NOTIFICATION.WORDSTYLESUPDATED

   Arguments contains: ``bufferID``





MESSAGEBOXFLAGS
---------------

.. _MESSAGEBOXFLAGS:
.. class:: MESSAGEBOXFLAGS

.. attribute:: MESSAGEBOXFLAGS.ABORTRETRYIGNORE

.. attribute:: MESSAGEBOXFLAGS.CANCELTRYCONTINUE

.. attribute:: MESSAGEBOXFLAGS.DEFBUTTON1

.. attribute:: MESSAGEBOXFLAGS.DEFBUTTON2

.. attribute:: MESSAGEBOXFLAGS.DEFBUTTON3

.. attribute:: MESSAGEBOXFLAGS.DEFBUTTON4

.. attribute:: MESSAGEBOXFLAGS.ICONASTERISK

.. attribute:: MESSAGEBOXFLAGS.ICONERROR

.. attribute:: MESSAGEBOXFLAGS.ICONEXCLAMATION

.. attribute:: MESSAGEBOXFLAGS.ICONHAND

.. attribute:: MESSAGEBOXFLAGS.ICONINFORMATION

.. attribute:: MESSAGEBOXFLAGS.ICONQUESTION

.. attribute:: MESSAGEBOXFLAGS.ICONSTOP

.. attribute:: MESSAGEBOXFLAGS.ICONWARNING

.. attribute:: MESSAGEBOXFLAGS.OK

.. attribute:: MESSAGEBOXFLAGS.OKCANCEL

.. attribute:: MESSAGEBOXFLAGS.RETRYCANCEL

.. attribute:: MESSAGEBOXFLAGS.YESNO

.. attribute:: MESSAGEBOXFLAGS.YESNOCANCEL

.. attribute:: MESSAGEBOXFLAGS.RESULTCONTINUE

.. attribute:: MESSAGEBOXFLAGS.RESULTABORT
		
.. attribute:: MESSAGEBOXFLAGS.RESULTCANCEL

.. attribute:: MESSAGEBOXFLAGS.RESULTIGNORE

.. attribute:: MESSAGEBOXFLAGS.RESULTNO

.. attribute:: MESSAGEBOXFLAGS.RESULTOK

.. attribute:: MESSAGEBOXFLAGS.RESULTRETRY

.. attribute:: MESSAGEBOXFLAGS.RESULTTRYAGAIN

.. attribute:: MESSAGEBOXFLAGS.RESULTYES


MENUCOMMAND
-----------

.. _MENUCOMMAND:
.. class:: MENUCOMMAND

.. attribute:: MENUCOMMAND.CLEAN_RECENT_FILE_LIST

.. attribute:: MENUCOMMAND.EDIT_AUTOCOMPLETE

.. attribute:: MENUCOMMAND.EDIT_AUTOCOMPLETE_CURRENTFILE

.. attribute:: MENUCOMMAND.EDIT_BLOCK_COMMENT

.. attribute:: MENUCOMMAND.EDIT_BLOCK_COMMENT_SET

.. attribute:: MENUCOMMAND.EDIT_BLOCK_UNCOMMENT

.. attribute:: MENUCOMMAND.EDIT_CLEARREADONLY

.. attribute:: MENUCOMMAND.EDIT_COLUMNMODE

.. attribute:: MENUCOMMAND.EDIT_COPY

.. attribute:: MENUCOMMAND.EDIT_CURRENTDIRTOCLIP

.. attribute:: MENUCOMMAND.EDIT_CUT

.. attribute:: MENUCOMMAND.EDIT_DELETE

.. attribute:: MENUCOMMAND.EDIT_DUP_LINE

.. attribute:: MENUCOMMAND.EDIT_FILENAMETOCLIP

.. attribute:: MENUCOMMAND.EDIT_FULLPATHTOCLIP

.. attribute:: MENUCOMMAND.EDIT_FUNCCALLTIP

.. attribute:: MENUCOMMAND.EDIT_INS_TAB

.. attribute:: MENUCOMMAND.EDIT_JOIN_LINES

.. attribute:: MENUCOMMAND.EDIT_LINE_DOWN

.. attribute:: MENUCOMMAND.EDIT_LINE_UP

.. attribute:: MENUCOMMAND.EDIT_LOWERCASE

.. attribute:: MENUCOMMAND.EDIT_LTR

.. attribute:: MENUCOMMAND.EDIT_PASTE

.. attribute:: MENUCOMMAND.EDIT_REDO

.. attribute:: MENUCOMMAND.EDIT_RMV_TAB

.. attribute:: MENUCOMMAND.EDIT_RTL

.. attribute:: MENUCOMMAND.EDIT_SELECTALL

.. attribute:: MENUCOMMAND.EDIT_SETREADONLY

.. attribute:: MENUCOMMAND.EDIT_SPLIT_LINES

.. attribute:: MENUCOMMAND.EDIT_STREAM_COMMENT

.. attribute:: MENUCOMMAND.EDIT_TRANSPOSE_LINE

.. attribute:: MENUCOMMAND.EDIT_TRIMTRAILING

.. attribute:: MENUCOMMAND.EDIT_UNDO

.. attribute:: MENUCOMMAND.EDIT_UPPERCASE

.. attribute:: MENUCOMMAND.FILE_CLOSE

.. attribute:: MENUCOMMAND.FILE_CLOSEALL

.. attribute:: MENUCOMMAND.FILE_CLOSEALL_BUT_CURRENT

.. attribute:: MENUCOMMAND.FILE_DELETE

.. attribute:: MENUCOMMAND.FILE_EXIT

.. attribute:: MENUCOMMAND.FILE_LOADSESSION

.. attribute:: MENUCOMMAND.FILE_NEW

.. attribute:: MENUCOMMAND.FILE_OPEN

.. attribute:: MENUCOMMAND.FILE_PRINT

.. attribute:: MENUCOMMAND.FILE_PRINTNOW

.. attribute:: MENUCOMMAND.FILE_RELOAD

.. attribute:: MENUCOMMAND.FILE_RENAME

.. attribute:: MENUCOMMAND.FILE_SAVE

.. attribute:: MENUCOMMAND.FILE_SAVEALL

.. attribute:: MENUCOMMAND.FILE_SAVEAS

.. attribute:: MENUCOMMAND.FILE_SAVECOPYAS

.. attribute:: MENUCOMMAND.FILE_SAVESESSION

.. attribute:: MENUCOMMAND.FOCUS_ON_FOUND_RESULTS

.. attribute:: MENUCOMMAND.FORMAT_ANSI

.. attribute:: MENUCOMMAND.FORMAT_AS_UTF_8

.. attribute:: MENUCOMMAND.FORMAT_BIG5

.. attribute:: MENUCOMMAND.FORMAT_CONV2_ANSI

.. attribute:: MENUCOMMAND.FORMAT_CONV2_AS_UTF_8

.. attribute:: MENUCOMMAND.FORMAT_CONV2_UCS_2BE

.. attribute:: MENUCOMMAND.FORMAT_CONV2_UCS_2LE

.. attribute:: MENUCOMMAND.FORMAT_CONV2_UTF_8

.. attribute:: MENUCOMMAND.FORMAT_DOS_437

.. attribute:: MENUCOMMAND.FORMAT_DOS_720

.. attribute:: MENUCOMMAND.FORMAT_DOS_737

.. attribute:: MENUCOMMAND.FORMAT_DOS_775

.. attribute:: MENUCOMMAND.FORMAT_DOS_850

.. attribute:: MENUCOMMAND.FORMAT_DOS_852

.. attribute:: MENUCOMMAND.FORMAT_DOS_855

.. attribute:: MENUCOMMAND.FORMAT_DOS_857

.. attribute:: MENUCOMMAND.FORMAT_DOS_858

.. attribute:: MENUCOMMAND.FORMAT_DOS_860

.. attribute:: MENUCOMMAND.FORMAT_DOS_861

.. attribute:: MENUCOMMAND.FORMAT_DOS_862

.. attribute:: MENUCOMMAND.FORMAT_DOS_863

.. attribute:: MENUCOMMAND.FORMAT_DOS_865

.. attribute:: MENUCOMMAND.FORMAT_DOS_866

.. attribute:: MENUCOMMAND.FORMAT_DOS_869

.. attribute:: MENUCOMMAND.FORMAT_ENCODE

.. attribute:: MENUCOMMAND.FORMAT_ENCODE_END

.. attribute:: MENUCOMMAND.FORMAT_EUC_KR

.. attribute:: MENUCOMMAND.FORMAT_GB2312

.. attribute:: MENUCOMMAND.FORMAT_ISO_8859_1

.. attribute:: MENUCOMMAND.FORMAT_ISO_8859_10

.. attribute:: MENUCOMMAND.FORMAT_ISO_8859_11

.. attribute:: MENUCOMMAND.FORMAT_ISO_8859_13

.. attribute:: MENUCOMMAND.FORMAT_ISO_8859_14

.. attribute:: MENUCOMMAND.FORMAT_ISO_8859_15

.. attribute:: MENUCOMMAND.FORMAT_ISO_8859_16

.. attribute:: MENUCOMMAND.FORMAT_ISO_8859_2

.. attribute:: MENUCOMMAND.FORMAT_ISO_8859_3

.. attribute:: MENUCOMMAND.FORMAT_ISO_8859_4

.. attribute:: MENUCOMMAND.FORMAT_ISO_8859_5

.. attribute:: MENUCOMMAND.FORMAT_ISO_8859_6

.. attribute:: MENUCOMMAND.FORMAT_ISO_8859_7

.. attribute:: MENUCOMMAND.FORMAT_ISO_8859_8

.. attribute:: MENUCOMMAND.FORMAT_ISO_8859_9

.. attribute:: MENUCOMMAND.FORMAT_KOI8R_CYRILLIC

.. attribute:: MENUCOMMAND.FORMAT_KOI8U_CYRILLIC

.. attribute:: MENUCOMMAND.FORMAT_KOREAN_WIN

.. attribute:: MENUCOMMAND.FORMAT_MAC_CYRILLIC

.. attribute:: MENUCOMMAND.FORMAT_SHIFT_JIS

.. attribute:: MENUCOMMAND.FORMAT_TIS_620

.. attribute:: MENUCOMMAND.FORMAT_TODOS

.. attribute:: MENUCOMMAND.FORMAT_TOMAC

.. attribute:: MENUCOMMAND.FORMAT_TOUNIX

.. attribute:: MENUCOMMAND.FORMAT_UCS_2BE

.. attribute:: MENUCOMMAND.FORMAT_UCS_2LE

.. attribute:: MENUCOMMAND.FORMAT_UTF_8

.. attribute:: MENUCOMMAND.FORMAT_WIN_1250

.. attribute:: MENUCOMMAND.FORMAT_WIN_1251

.. attribute:: MENUCOMMAND.FORMAT_WIN_1252

.. attribute:: MENUCOMMAND.FORMAT_WIN_1253

.. attribute:: MENUCOMMAND.FORMAT_WIN_1254

.. attribute:: MENUCOMMAND.FORMAT_WIN_1255

.. attribute:: MENUCOMMAND.FORMAT_WIN_1256

.. attribute:: MENUCOMMAND.FORMAT_WIN_1257

.. attribute:: MENUCOMMAND.FORMAT_WIN_1258

.. attribute:: MENUCOMMAND.FORUM

.. attribute:: MENUCOMMAND.HELP

.. attribute:: MENUCOMMAND.HOMESWEETHOME

.. attribute:: MENUCOMMAND.LANGSTYLE_CONFIG_DLG

.. attribute:: MENUCOMMAND.LANG_ADA

.. attribute:: MENUCOMMAND.LANG_ASCII

.. attribute:: MENUCOMMAND.LANG_ASM

.. attribute:: MENUCOMMAND.LANG_ASP

.. attribute:: MENUCOMMAND.LANG_AU3

.. attribute:: MENUCOMMAND.LANG_BASH

.. attribute:: MENUCOMMAND.LANG_BATCH

.. attribute:: MENUCOMMAND.LANG_C

.. attribute:: MENUCOMMAND.LANG_CAML

.. attribute:: MENUCOMMAND.LANG_CMAKE

.. attribute:: MENUCOMMAND.LANG_COBOL

.. attribute:: MENUCOMMAND.LANG_CPP

.. attribute:: MENUCOMMAND.LANG_CS

.. attribute:: MENUCOMMAND.LANG_CSS

.. attribute:: MENUCOMMAND.LANG_D

.. attribute:: MENUCOMMAND.LANG_DIFF

.. attribute:: MENUCOMMAND.LANG_EXTERNAL

.. attribute:: MENUCOMMAND.LANG_EXTERNAL_LIMIT

.. attribute:: MENUCOMMAND.LANG_FLASH

.. attribute:: MENUCOMMAND.LANG_FORTRAN

.. attribute:: MENUCOMMAND.LANG_GUI4CLI

.. attribute:: MENUCOMMAND.LANG_HASKELL

.. attribute:: MENUCOMMAND.LANG_HTML

.. attribute:: MENUCOMMAND.LANG_INI

.. attribute:: MENUCOMMAND.LANG_INNO

.. attribute:: MENUCOMMAND.LANG_JAVA

.. attribute:: MENUCOMMAND.LANG_JS

.. attribute:: MENUCOMMAND.LANG_JSP

.. attribute:: MENUCOMMAND.LANG_KIX

.. attribute:: MENUCOMMAND.LANG_LISP

.. attribute:: MENUCOMMAND.LANG_LUA

.. attribute:: MENUCOMMAND.LANG_MAKEFILE

.. attribute:: MENUCOMMAND.LANG_MATLAB

.. attribute:: MENUCOMMAND.LANG_NSIS

.. attribute:: MENUCOMMAND.LANG_OBJC

.. attribute:: MENUCOMMAND.LANG_PASCAL

.. attribute:: MENUCOMMAND.LANG_PERL

.. attribute:: MENUCOMMAND.LANG_PHP

.. attribute:: MENUCOMMAND.LANG_POWERSHELL

.. attribute:: MENUCOMMAND.LANG_PROPS

.. attribute:: MENUCOMMAND.LANG_PS

.. attribute:: MENUCOMMAND.LANG_PYTHON

.. attribute:: MENUCOMMAND.LANG_R

.. attribute:: MENUCOMMAND.LANG_RC

.. attribute:: MENUCOMMAND.LANG_RUBY

.. attribute:: MENUCOMMAND.LANG_SCHEME

.. attribute:: MENUCOMMAND.LANG_SMALLTALK

.. attribute:: MENUCOMMAND.LANG_SQL

.. attribute:: MENUCOMMAND.LANG_TCL

.. attribute:: MENUCOMMAND.LANG_TEX

.. attribute:: MENUCOMMAND.LANG_TEXT

.. attribute:: MENUCOMMAND.LANG_USER

.. attribute:: MENUCOMMAND.LANG_USER_LIMIT

.. attribute:: MENUCOMMAND.LANG_VB

.. attribute:: MENUCOMMAND.LANG_VERILOG

.. attribute:: MENUCOMMAND.LANG_VHDL

.. attribute:: MENUCOMMAND.LANG_XML

.. attribute:: MENUCOMMAND.LANG_YAML

.. attribute:: MENUCOMMAND.MACRO_PLAYBACKRECORDEDMACRO

.. attribute:: MENUCOMMAND.MACRO_RUNMULTIMACRODLG

.. attribute:: MENUCOMMAND.MACRO_SAVECURRENTMACRO

.. attribute:: MENUCOMMAND.MACRO_STARTRECORDINGMACRO

.. attribute:: MENUCOMMAND.MACRO_STOPRECORDINGMACRO

.. attribute:: MENUCOMMAND.ONLINEHELP

.. attribute:: MENUCOMMAND.OPEN_ALL_RECENT_FILE

.. attribute:: MENUCOMMAND.PLUGINSHOME

.. attribute:: MENUCOMMAND.PROJECTPAGE

.. attribute:: MENUCOMMAND.SEARCH_CLEARALLMARKS

.. attribute:: MENUCOMMAND.SEARCH_CLEAR_BOOKMARKS

.. attribute:: MENUCOMMAND.SEARCH_COPYMARKEDLINES

.. attribute:: MENUCOMMAND.SEARCH_CUTMARKEDLINES

.. attribute:: MENUCOMMAND.SEARCH_DELETEMARKEDLINES

.. attribute:: MENUCOMMAND.SEARCH_FIND

.. attribute:: MENUCOMMAND.SEARCH_FINDINCREMENT

.. attribute:: MENUCOMMAND.SEARCH_FINDINFILES

.. attribute:: MENUCOMMAND.SEARCH_FINDNEXT

.. attribute:: MENUCOMMAND.SEARCH_FINDPREV

.. attribute:: MENUCOMMAND.SEARCH_GONEXTMARKER1

.. attribute:: MENUCOMMAND.SEARCH_GONEXTMARKER2

.. attribute:: MENUCOMMAND.SEARCH_GONEXTMARKER3

.. attribute:: MENUCOMMAND.SEARCH_GONEXTMARKER4

.. attribute:: MENUCOMMAND.SEARCH_GONEXTMARKER5

.. attribute:: MENUCOMMAND.SEARCH_GONEXTMARKER_DEF

.. attribute:: MENUCOMMAND.SEARCH_GOPREVMARKER1

.. attribute:: MENUCOMMAND.SEARCH_GOPREVMARKER2

.. attribute:: MENUCOMMAND.SEARCH_GOPREVMARKER3

.. attribute:: MENUCOMMAND.SEARCH_GOPREVMARKER4

.. attribute:: MENUCOMMAND.SEARCH_GOPREVMARKER5

.. attribute:: MENUCOMMAND.SEARCH_GOPREVMARKER_DEF

.. attribute:: MENUCOMMAND.SEARCH_GOTOLINE

.. attribute:: MENUCOMMAND.SEARCH_GOTOMATCHINGBRACE

.. attribute:: MENUCOMMAND.SEARCH_GOTONEXTFOUND

.. attribute:: MENUCOMMAND.SEARCH_GOTOPREVFOUND

.. attribute:: MENUCOMMAND.SEARCH_MARKALLEXT1

.. attribute:: MENUCOMMAND.SEARCH_MARKALLEXT2

.. attribute:: MENUCOMMAND.SEARCH_MARKALLEXT3

.. attribute:: MENUCOMMAND.SEARCH_MARKALLEXT4

.. attribute:: MENUCOMMAND.SEARCH_MARKALLEXT5

.. attribute:: MENUCOMMAND.SEARCH_NEXT_BOOKMARK

.. attribute:: MENUCOMMAND.SEARCH_PASTEMARKEDLINES

.. attribute:: MENUCOMMAND.SEARCH_PREV_BOOKMARK

.. attribute:: MENUCOMMAND.SEARCH_REPLACE

.. attribute:: MENUCOMMAND.SEARCH_SETANDFINDNEXT

.. attribute:: MENUCOMMAND.SEARCH_SETANDFINDPREV

.. attribute:: MENUCOMMAND.SEARCH_TOGGLE_BOOKMARK

.. attribute:: MENUCOMMAND.SEARCH_UNMARKALLEXT1

.. attribute:: MENUCOMMAND.SEARCH_UNMARKALLEXT2

.. attribute:: MENUCOMMAND.SEARCH_UNMARKALLEXT3

.. attribute:: MENUCOMMAND.SEARCH_UNMARKALLEXT4

.. attribute:: MENUCOMMAND.SEARCH_UNMARKALLEXT5

.. attribute:: MENUCOMMAND.SEARCH_VOLATILE_FINDNEXT

.. attribute:: MENUCOMMAND.SEARCH_VOLATILE_FINDPREV

.. attribute:: MENUCOMMAND.SETTING_AUTOCNBCHAR

.. attribute:: MENUCOMMAND.SETTING_EDGE_SIZE

.. attribute:: MENUCOMMAND.SETTING_HISTORY_SIZE

.. attribute:: MENUCOMMAND.SETTING_IMPORTPLUGIN

.. attribute:: MENUCOMMAND.SETTING_IMPORTSTYLETHEMS

.. attribute:: MENUCOMMAND.SETTING_PREFERECE

.. attribute:: MENUCOMMAND.SETTING_REMEMBER_LAST_SESSION

.. attribute:: MENUCOMMAND.SETTING_SHORTCUT_MAPPER

.. attribute:: MENUCOMMAND.SETTING_TAB_REPLCESPACE

.. attribute:: MENUCOMMAND.SETTING_TAB_SIZE

.. attribute:: MENUCOMMAND.SETTING_TRAYICON

.. attribute:: MENUCOMMAND.SYSTRAYPOPUP_ACTIVATE

.. attribute:: MENUCOMMAND.SYSTRAYPOPUP_CLOSE

.. attribute:: MENUCOMMAND.SYSTRAYPOPUP_NEWDOC

.. attribute:: MENUCOMMAND.SYSTRAYPOPUP_NEW_AND_PASTE

.. attribute:: MENUCOMMAND.SYSTRAYPOPUP_OPENFILE

.. attribute:: MENUCOMMAND.UPDATE_NPP

.. attribute:: MENUCOMMAND.VIEW_ALL_CHARACTERS

.. attribute:: MENUCOMMAND.VIEW_ALWAYSONTOP

.. attribute:: MENUCOMMAND.VIEW_CLONE_TO_ANOTHER_VIEW

.. attribute:: MENUCOMMAND.VIEW_CURLINE_HILITING

.. attribute:: MENUCOMMAND.VIEW_DOCCHANGEMARGIN

.. attribute:: MENUCOMMAND.VIEW_DRAWTABBAR_CLOSEBOTTUN

.. attribute:: MENUCOMMAND.VIEW_DRAWTABBAR_DBCLK2CLOSE

.. attribute:: MENUCOMMAND.VIEW_DRAWTABBAR_INACIVETAB

.. attribute:: MENUCOMMAND.VIEW_DRAWTABBAR_MULTILINE

.. attribute:: MENUCOMMAND.VIEW_DRAWTABBAR_TOPBAR

.. attribute:: MENUCOMMAND.VIEW_DRAWTABBAR_VERTICAL

.. attribute:: MENUCOMMAND.VIEW_EDGEBACKGROUND

.. attribute:: MENUCOMMAND.VIEW_EDGELINE

.. attribute:: MENUCOMMAND.VIEW_EDGENONE

.. attribute:: MENUCOMMAND.VIEW_EOL

.. attribute:: MENUCOMMAND.VIEW_FOLD

.. attribute:: MENUCOMMAND.VIEW_FOLDERMAGIN

.. attribute:: MENUCOMMAND.VIEW_FOLDERMAGIN_ARROW

.. attribute:: MENUCOMMAND.VIEW_FOLDERMAGIN_BOX

.. attribute:: MENUCOMMAND.VIEW_FOLDERMAGIN_CIRCLE

.. attribute:: MENUCOMMAND.VIEW_FOLDERMAGIN_SIMPLE

.. attribute:: MENUCOMMAND.VIEW_FOLD_1

.. attribute:: MENUCOMMAND.VIEW_FOLD_2

.. attribute:: MENUCOMMAND.VIEW_FOLD_3

.. attribute:: MENUCOMMAND.VIEW_FOLD_4

.. attribute:: MENUCOMMAND.VIEW_FOLD_5

.. attribute:: MENUCOMMAND.VIEW_FOLD_6

.. attribute:: MENUCOMMAND.VIEW_FOLD_7

.. attribute:: MENUCOMMAND.VIEW_FOLD_8

.. attribute:: MENUCOMMAND.VIEW_FOLD_CURRENT

.. attribute:: MENUCOMMAND.VIEW_FULLSCREENTOGGLE

.. attribute:: MENUCOMMAND.VIEW_GOTO_ANOTHER_VIEW

.. attribute:: MENUCOMMAND.VIEW_GOTO_NEW_INSTANCE

.. attribute:: MENUCOMMAND.VIEW_HIDELINES

.. attribute:: MENUCOMMAND.VIEW_INDENT_GUIDE

.. attribute:: MENUCOMMAND.VIEW_LINENUMBER

.. attribute:: MENUCOMMAND.VIEW_LOAD_IN_NEW_INSTANCE

.. attribute:: MENUCOMMAND.VIEW_LOCKTABBAR

.. attribute:: MENUCOMMAND.VIEW_POSTIT

.. attribute:: MENUCOMMAND.VIEW_REDUCETABBAR

.. attribute:: MENUCOMMAND.VIEW_REFRESHTABAR

.. attribute:: MENUCOMMAND.VIEW_SWITCHTO_OTHER_VIEW

.. attribute:: MENUCOMMAND.VIEW_SYMBOLMARGIN

.. attribute:: MENUCOMMAND.VIEW_SYNSCROLLH

.. attribute:: MENUCOMMAND.VIEW_SYNSCROLLV

.. attribute:: MENUCOMMAND.VIEW_TAB_SPACE

.. attribute:: MENUCOMMAND.VIEW_TOGGLE_FOLDALL

.. attribute:: MENUCOMMAND.VIEW_TOGGLE_UNFOLDALL

.. attribute:: MENUCOMMAND.VIEW_TOOLBAR_ENLARGE

.. attribute:: MENUCOMMAND.VIEW_TOOLBAR_REDUCE

.. attribute:: MENUCOMMAND.VIEW_TOOLBAR_STANDARD

.. attribute:: MENUCOMMAND.VIEW_UNFOLD

.. attribute:: MENUCOMMAND.VIEW_UNFOLD_1

.. attribute:: MENUCOMMAND.VIEW_UNFOLD_2

.. attribute:: MENUCOMMAND.VIEW_UNFOLD_3

.. attribute:: MENUCOMMAND.VIEW_UNFOLD_4

.. attribute:: MENUCOMMAND.VIEW_UNFOLD_5

.. attribute:: MENUCOMMAND.VIEW_UNFOLD_6

.. attribute:: MENUCOMMAND.VIEW_UNFOLD_7

.. attribute:: MENUCOMMAND.VIEW_UNFOLD_8

.. attribute:: MENUCOMMAND.VIEW_UNFOLD_CURRENT

.. attribute:: MENUCOMMAND.VIEW_USER_DLG

.. attribute:: MENUCOMMAND.VIEW_WRAP

.. attribute:: MENUCOMMAND.VIEW_WRAP_SYMBOL

.. attribute:: MENUCOMMAND.VIEW_ZOOMIN

.. attribute:: MENUCOMMAND.VIEW_ZOOMOUT

.. attribute:: MENUCOMMAND.VIEW_ZOOMRESTORE

.. attribute:: MENUCOMMAND.WIKIFAQ


.. The rest of this file is autogenerated from Scintilla. To edit change CreateWrapper.py /* ++Autogenerated ----- */


ANNOTATIONVISIBLE
-----------------

.. _ANNOTATIONVISIBLE:
.. class:: ANNOTATIONVISIBLE

.. attribute:: ANNOTATIONVISIBLE.HIDDEN

.. attribute:: ANNOTATIONVISIBLE.STANDARD

.. attribute:: ANNOTATIONVISIBLE.BOXED

AUTOMATICFOLD
-------------

.. _AUTOMATICFOLD:
.. class:: AUTOMATICFOLD

.. attribute:: AUTOMATICFOLD.SHOW

.. attribute:: AUTOMATICFOLD.CLICK

.. attribute:: AUTOMATICFOLD.CHANGE

CARETPOLICY
-----------

.. _CARETPOLICY:
.. class:: CARETPOLICY

.. attribute:: CARETPOLICY.SLOP

.. attribute:: CARETPOLICY.STRICT

.. attribute:: CARETPOLICY.JUMPS

.. attribute:: CARETPOLICY.EVEN

CARETSTICKY
-----------

.. _CARETSTICKY:
.. class:: CARETSTICKY

.. attribute:: CARETSTICKY.OFF

.. attribute:: CARETSTICKY.ON

.. attribute:: CARETSTICKY.WHITESPACE

CARETSTYLE
----------

.. _CARETSTYLE:
.. class:: CARETSTYLE

.. attribute:: CARETSTYLE.INVISIBLE

.. attribute:: CARETSTYLE.LINE

.. attribute:: CARETSTYLE.BLOCK

CASEINSENSITIVEBEHAVIOUR
------------------------

.. _CASEINSENSITIVEBEHAVIOUR:
.. class:: CASEINSENSITIVEBEHAVIOUR

.. attribute:: CASEINSENSITIVEBEHAVIOUR.RESPECTCASE

.. attribute:: CASEINSENSITIVEBEHAVIOUR.IGNORECASE

CASEVISIBLE
-----------

.. _CASEVISIBLE:
.. class:: CASEVISIBLE

.. attribute:: CASEVISIBLE.MIXED

.. attribute:: CASEVISIBLE.UPPER

.. attribute:: CASEVISIBLE.LOWER

CHARACTERSET
------------

.. _CHARACTERSET:
.. class:: CHARACTERSET

.. attribute:: CHARACTERSET.ANSI

.. attribute:: CHARACTERSET.DEFAULT

.. attribute:: CHARACTERSET.BALTIC

.. attribute:: CHARACTERSET.CHINESEBIG5

.. attribute:: CHARACTERSET.EASTEUROPE

.. attribute:: CHARACTERSET.GB2312

.. attribute:: CHARACTERSET.GREEK

.. attribute:: CHARACTERSET.HANGUL

.. attribute:: CHARACTERSET.MAC

.. attribute:: CHARACTERSET.OEM

.. attribute:: CHARACTERSET.RUSSIAN

.. attribute:: CHARACTERSET.CYRILLIC

.. attribute:: CHARACTERSET.SHIFTJIS

.. attribute:: CHARACTERSET.SYMBOL

.. attribute:: CHARACTERSET.TURKISH

.. attribute:: CHARACTERSET.JOHAB

.. attribute:: CHARACTERSET.HEBREW

.. attribute:: CHARACTERSET.ARABIC

.. attribute:: CHARACTERSET.VIETNAMESE

.. attribute:: CHARACTERSET.THAI

.. attribute:: CHARACTERSET.8859_15

CURSORSHAPE
-----------

.. _CURSORSHAPE:
.. class:: CURSORSHAPE

.. attribute:: CURSORSHAPE.NORMAL

.. attribute:: CURSORSHAPE.ARROW

.. attribute:: CURSORSHAPE.WAIT

.. attribute:: CURSORSHAPE.REVERSEARROW

EDGEVISUALSTYLE
---------------

.. _EDGEVISUALSTYLE:
.. class:: EDGEVISUALSTYLE

.. attribute:: EDGEVISUALSTYLE.NONE

.. attribute:: EDGEVISUALSTYLE.LINE

.. attribute:: EDGEVISUALSTYLE.BACKGROUND

ENDOFLINE
---------

.. _ENDOFLINE:
.. class:: ENDOFLINE

.. attribute:: ENDOFLINE.CRLF

.. attribute:: ENDOFLINE.CR

.. attribute:: ENDOFLINE.LF

FINDOPTION
----------

.. _FINDOPTION:
.. class:: FINDOPTION

.. attribute:: FINDOPTION.WHOLEWORD

.. attribute:: FINDOPTION.MATCHCASE

.. attribute:: FINDOPTION.WORDSTART

.. attribute:: FINDOPTION.REGEXP

.. attribute:: FINDOPTION.POSIX

FOLDACTION
----------

.. _FOLDACTION:
.. class:: FOLDACTION

.. attribute:: FOLDACTION._CONTRACT

.. attribute:: FOLDACTION._EXPAND

.. attribute:: FOLDACTION._TOGGLE

FOLDFLAG
--------

.. _FOLDFLAG:
.. class:: FOLDFLAG

.. attribute:: FOLDFLAG.LINEBEFORE_EXPANDED

.. attribute:: FOLDFLAG.LINEBEFORE_CONTRACTED

.. attribute:: FOLDFLAG.LINEAFTER_EXPANDED

.. attribute:: FOLDFLAG.LINEAFTER_CONTRACTED

.. attribute:: FOLDFLAG.LEVELNUMBERS

FOLDLEVEL
---------

.. _FOLDLEVEL:
.. class:: FOLDLEVEL

.. attribute:: FOLDLEVEL.BASE

.. attribute:: FOLDLEVEL.WHITEFLAG

.. attribute:: FOLDLEVEL.HEADERFLAG

.. attribute:: FOLDLEVEL.NUMBERMASK

FONTQUALITY
-----------

.. _FONTQUALITY:
.. class:: FONTQUALITY

.. attribute:: FONTQUALITY.QUALITY_MASK

.. attribute:: FONTQUALITY.QUALITY_DEFAULT

.. attribute:: FONTQUALITY.QUALITY_NON_ANTIALIASED

.. attribute:: FONTQUALITY.QUALITY_ANTIALIASED

.. attribute:: FONTQUALITY.QUALITY_LCD_OPTIMIZED

FONTWEIGHT
----------

.. _FONTWEIGHT:
.. class:: FONTWEIGHT

.. attribute:: FONTWEIGHT.NORMAL

.. attribute:: FONTWEIGHT.SEMIBOLD

.. attribute:: FONTWEIGHT.BOLD

INDENTVIEW
----------

.. _INDENTVIEW:
.. class:: INDENTVIEW

.. attribute:: INDENTVIEW.NONE

.. attribute:: INDENTVIEW.REAL

.. attribute:: INDENTVIEW.LOOKFORWARD

.. attribute:: INDENTVIEW.LOOKBOTH

INDICATORSTYLE
--------------

.. _INDICATORSTYLE:
.. class:: INDICATORSTYLE

.. attribute:: INDICATORSTYLE.PLAIN

.. attribute:: INDICATORSTYLE.SQUIGGLE

.. attribute:: INDICATORSTYLE.TT

.. attribute:: INDICATORSTYLE.DIAGONAL

.. attribute:: INDICATORSTYLE.STRIKE

.. attribute:: INDICATORSTYLE.HIDDEN

.. attribute:: INDICATORSTYLE.BOX

.. attribute:: INDICATORSTYLE.ROUNDBOX

.. attribute:: INDICATORSTYLE.STRAIGHTBOX

.. attribute:: INDICATORSTYLE.DASH

.. attribute:: INDICATORSTYLE.DOTS

.. attribute:: INDICATORSTYLE.SQUIGGLELOW

.. attribute:: INDICATORSTYLE.DOTBOX

.. attribute:: INDICATORSTYLE.SQUIGGLEPIXMAP

.. attribute:: INDICATORSTYLE.COMPOSITIONTHICK

.. attribute:: INDICATORSTYLE.MAX

.. attribute:: INDICATORSTYLE.CONTAINER

KEYMOD
------

.. _KEYMOD:
.. class:: KEYMOD

.. attribute:: KEYMOD.NORM

.. attribute:: KEYMOD.SHIFT

.. attribute:: KEYMOD.CTRL

.. attribute:: KEYMOD.ALT

.. attribute:: KEYMOD.SUPER

.. attribute:: KEYMOD.META

KEYS
----

.. _KEYS:
.. class:: KEYS

.. attribute:: KEYS.DOWN

.. attribute:: KEYS.UP

.. attribute:: KEYS.LEFT

.. attribute:: KEYS.RIGHT

.. attribute:: KEYS.HOME

.. attribute:: KEYS.END

.. attribute:: KEYS.PRIOR

.. attribute:: KEYS.NEXT

.. attribute:: KEYS.DELETE

.. attribute:: KEYS.INSERT

.. attribute:: KEYS.ESCAPE

.. attribute:: KEYS.BACK

.. attribute:: KEYS.TAB

.. attribute:: KEYS.RETURN

.. attribute:: KEYS.ADD

.. attribute:: KEYS.SUBTRACT

.. attribute:: KEYS.DIVIDE

.. attribute:: KEYS.WIN

.. attribute:: KEYS.RWIN

.. attribute:: KEYS.MENU

LEXER
-----

.. _LEXER:
.. class:: LEXER

.. attribute:: LEXER.CONTAINER

.. attribute:: LEXER.NULL

.. attribute:: LEXER.PYTHON

.. attribute:: LEXER.CPP

.. attribute:: LEXER.HTML

.. attribute:: LEXER.XML

.. attribute:: LEXER.PERL

.. attribute:: LEXER.SQL

.. attribute:: LEXER.VB

.. attribute:: LEXER.PROPERTIES

.. attribute:: LEXER.ERRORLIST

.. attribute:: LEXER.MAKEFILE

.. attribute:: LEXER.BATCH

.. attribute:: LEXER.XCODE

.. attribute:: LEXER.LATEX

.. attribute:: LEXER.LUA

.. attribute:: LEXER.DIFF

.. attribute:: LEXER.CONF

.. attribute:: LEXER.PASCAL

.. attribute:: LEXER.AVE

.. attribute:: LEXER.ADA

.. attribute:: LEXER.LISP

.. attribute:: LEXER.RUBY

.. attribute:: LEXER.EIFFEL

.. attribute:: LEXER.EIFFELKW

.. attribute:: LEXER.TCL

.. attribute:: LEXER.NNCRONTAB

.. attribute:: LEXER.BULLANT

.. attribute:: LEXER.VBSCRIPT

.. attribute:: LEXER.BAAN

.. attribute:: LEXER.MATLAB

.. attribute:: LEXER.SCRIPTOL

.. attribute:: LEXER.ASM

.. attribute:: LEXER.CPPNOCASE

.. attribute:: LEXER.FORTRAN

.. attribute:: LEXER.F77

.. attribute:: LEXER.CSS

.. attribute:: LEXER.POV

.. attribute:: LEXER.LOUT

.. attribute:: LEXER.ESCRIPT

.. attribute:: LEXER.PS

.. attribute:: LEXER.NSIS

.. attribute:: LEXER.MMIXAL

.. attribute:: LEXER.CLW

.. attribute:: LEXER.CLWNOCASE

.. attribute:: LEXER.LOT

.. attribute:: LEXER.YAML

.. attribute:: LEXER.TEX

.. attribute:: LEXER.METAPOST

.. attribute:: LEXER.POWERBASIC

.. attribute:: LEXER.FORTH

.. attribute:: LEXER.ERLANG

.. attribute:: LEXER.OCTAVE

.. attribute:: LEXER.MSSQL

.. attribute:: LEXER.VERILOG

.. attribute:: LEXER.KIX

.. attribute:: LEXER.GUI4CLI

.. attribute:: LEXER.SPECMAN

.. attribute:: LEXER.AU3

.. attribute:: LEXER.APDL

.. attribute:: LEXER.BASH

.. attribute:: LEXER.ASN1

.. attribute:: LEXER.VHDL

.. attribute:: LEXER.CAML

.. attribute:: LEXER.BLITZBASIC

.. attribute:: LEXER.PUREBASIC

.. attribute:: LEXER.HASKELL

.. attribute:: LEXER.PHPSCRIPT

.. attribute:: LEXER.TADS3

.. attribute:: LEXER.REBOL

.. attribute:: LEXER.SMALLTALK

.. attribute:: LEXER.FLAGSHIP

.. attribute:: LEXER.CSOUND

.. attribute:: LEXER.FREEBASIC

.. attribute:: LEXER.INNOSETUP

.. attribute:: LEXER.OPAL

.. attribute:: LEXER.SPICE

.. attribute:: LEXER.D

.. attribute:: LEXER.CMAKE

.. attribute:: LEXER.GAP

.. attribute:: LEXER.PLM

.. attribute:: LEXER.PROGRESS

.. attribute:: LEXER.ABAQUS

.. attribute:: LEXER.ASYMPTOTE

.. attribute:: LEXER.R

.. attribute:: LEXER.MAGIK

.. attribute:: LEXER.POWERSHELL

.. attribute:: LEXER.MYSQL

.. attribute:: LEXER.PO

.. attribute:: LEXER.TAL

.. attribute:: LEXER.COBOL

.. attribute:: LEXER.TACL

.. attribute:: LEXER.SORCUS

.. attribute:: LEXER.POWERPRO

.. attribute:: LEXER.NIMROD

.. attribute:: LEXER.SML

.. attribute:: LEXER.MARKDOWN

.. attribute:: LEXER.TXT2TAGS

.. attribute:: LEXER.A68K

.. attribute:: LEXER.MODULA

.. attribute:: LEXER.COFFEESCRIPT

.. attribute:: LEXER.TCMD

.. attribute:: LEXER.AVS

.. attribute:: LEXER.ECL

.. attribute:: LEXER.OSCRIPT

.. attribute:: LEXER.VISUALPROLOG

.. attribute:: LEXER.LITERATEHASKELL

.. attribute:: LEXER.STTXT

.. attribute:: LEXER.KVIRC

.. attribute:: LEXER.RUST

.. attribute:: LEXER.DMAP

.. attribute:: LEXER.AUTOMATIC

LINECACHE
---------

.. _LINECACHE:
.. class:: LINECACHE

.. attribute:: LINECACHE.NONE

.. attribute:: LINECACHE.CARET

.. attribute:: LINECACHE.PAGE

.. attribute:: LINECACHE.DOCUMENT

LINEENDTYPE
-----------

.. _LINEENDTYPE:
.. class:: LINEENDTYPE

.. attribute:: LINEENDTYPE.DEFAULT

.. attribute:: LINEENDTYPE.UNICODE

MARGINOPTION
------------

.. _MARGINOPTION:
.. class:: MARGINOPTION

.. attribute:: MARGINOPTION.NONE

.. attribute:: MARGINOPTION.SUBLINESELECT

MARGINTYPE
----------

.. _MARGINTYPE:
.. class:: MARGINTYPE

.. attribute:: MARGINTYPE.SYMBOL

.. attribute:: MARGINTYPE.NUMBER

.. attribute:: MARGINTYPE.BACK

.. attribute:: MARGINTYPE.FORE

.. attribute:: MARGINTYPE.TEXT

.. attribute:: MARGINTYPE.RTEXT

MARKEROUTLINE
-------------

.. _MARKEROUTLINE:
.. class:: MARKEROUTLINE

.. attribute:: MARKEROUTLINE.FOLDEREND

.. attribute:: MARKEROUTLINE.FOLDEROPENMID

.. attribute:: MARKEROUTLINE.FOLDERMIDTAIL

.. attribute:: MARKEROUTLINE.FOLDERTAIL

.. attribute:: MARKEROUTLINE.FOLDERSUB

.. attribute:: MARKEROUTLINE.FOLDER

.. attribute:: MARKEROUTLINE.FOLDEROPEN

MARKERSYMBOL
------------

.. _MARKERSYMBOL:
.. class:: MARKERSYMBOL

.. attribute:: MARKERSYMBOL.CIRCLE

.. attribute:: MARKERSYMBOL.ROUNDRECT

.. attribute:: MARKERSYMBOL.ARROW

.. attribute:: MARKERSYMBOL.SMALLRECT

.. attribute:: MARKERSYMBOL.SHORTARROW

.. attribute:: MARKERSYMBOL.EMPTY

.. attribute:: MARKERSYMBOL.ARROWDOWN

.. attribute:: MARKERSYMBOL.MINUS

.. attribute:: MARKERSYMBOL.PLUS

.. attribute:: MARKERSYMBOL.VLINE

.. attribute:: MARKERSYMBOL.LCORNER

.. attribute:: MARKERSYMBOL.TCORNER

.. attribute:: MARKERSYMBOL.BOXPLUS

.. attribute:: MARKERSYMBOL.BOXPLUSCONNECTED

.. attribute:: MARKERSYMBOL.BOXMINUS

.. attribute:: MARKERSYMBOL.BOXMINUSCONNECTED

.. attribute:: MARKERSYMBOL.LCORNERCURVE

.. attribute:: MARKERSYMBOL.TCORNERCURVE

.. attribute:: MARKERSYMBOL.CIRCLEPLUS

.. attribute:: MARKERSYMBOL.CIRCLEPLUSCONNECTED

.. attribute:: MARKERSYMBOL.CIRCLEMINUS

.. attribute:: MARKERSYMBOL.CIRCLEMINUSCONNECTED

.. attribute:: MARKERSYMBOL.BACKGROUND

.. attribute:: MARKERSYMBOL.DOTDOTDOT

.. attribute:: MARKERSYMBOL.ARROWS

.. attribute:: MARKERSYMBOL.PIXMAP

.. attribute:: MARKERSYMBOL.FULLRECT

.. attribute:: MARKERSYMBOL.LEFTRECT

.. attribute:: MARKERSYMBOL.AVAILABLE

.. attribute:: MARKERSYMBOL.UNDERLINE

.. attribute:: MARKERSYMBOL.RGBAIMAGE

.. attribute:: MARKERSYMBOL.BOOKMARK

.. attribute:: MARKERSYMBOL.CHARACTER

MODIFICATIONFLAGS
-----------------

.. _MODIFICATIONFLAGS:
.. class:: MODIFICATIONFLAGS

.. attribute:: MODIFICATIONFLAGS.INSERTTEXT

.. attribute:: MODIFICATIONFLAGS.DELETETEXT

.. attribute:: MODIFICATIONFLAGS.CHANGESTYLE

.. attribute:: MODIFICATIONFLAGS.CHANGEFOLD

.. attribute:: MODIFICATIONFLAGS.USER

.. attribute:: MODIFICATIONFLAGS.UNDO

.. attribute:: MODIFICATIONFLAGS.REDO

.. attribute:: MODIFICATIONFLAGS.MULTISTEPUNDOREDO

.. attribute:: MODIFICATIONFLAGS.LASTSTEPINUNDOREDO

.. attribute:: MODIFICATIONFLAGS.CHANGEMARKER

.. attribute:: MODIFICATIONFLAGS.BEFOREINSERT

.. attribute:: MODIFICATIONFLAGS.BEFOREDELETE

.. attribute:: MODIFICATIONFLAGS.MULTILINEUNDOREDO

.. attribute:: MODIFICATIONFLAGS.STARTACTION

.. attribute:: MODIFICATIONFLAGS.CHANGEINDICATOR

.. attribute:: MODIFICATIONFLAGS.CHANGELINESTATE

.. attribute:: MODIFICATIONFLAGS.CHANGEMARGIN

.. attribute:: MODIFICATIONFLAGS.CHANGEANNOTATION

.. attribute:: MODIFICATIONFLAGS.CONTAINER

.. attribute:: MODIFICATIONFLAGS.LEXERSTATE

.. attribute:: MODIFICATIONFLAGS.MODEVENTMASKALL

MULTIPASTE
----------

.. _MULTIPASTE:
.. class:: MULTIPASTE

.. attribute:: MULTIPASTE.ONCE

.. attribute:: MULTIPASTE.EACH

ORDERING
--------

.. _ORDERING:
.. class:: ORDERING

.. attribute:: ORDERING.PRESORTED

.. attribute:: ORDERING.PERFORMSORT

.. attribute:: ORDERING.CUSTOM

PRINTOPTION
-----------

.. _PRINTOPTION:
.. class:: PRINTOPTION

.. attribute:: PRINTOPTION.NORMAL

.. attribute:: PRINTOPTION.INVERTLIGHT

.. attribute:: PRINTOPTION.BLACKONWHITE

.. attribute:: PRINTOPTION.COLOURONWHITE

.. attribute:: PRINTOPTION.COLOURONWHITEDEFAULTBG

SELECTIONMODE
-------------

.. _SELECTIONMODE:
.. class:: SELECTIONMODE

.. attribute:: SELECTIONMODE.STREAM

.. attribute:: SELECTIONMODE.RECTANGLE

.. attribute:: SELECTIONMODE.LINES

.. attribute:: SELECTIONMODE.THIN

STATUS
------

.. _STATUS:
.. class:: STATUS

.. attribute:: STATUS.OK

.. attribute:: STATUS.FAILURE

.. attribute:: STATUS.BADALLOC

STYLESCOMMON
------------

.. _STYLESCOMMON:
.. class:: STYLESCOMMON

.. attribute:: STYLESCOMMON.DEFAULT

.. attribute:: STYLESCOMMON.LINENUMBER

.. attribute:: STYLESCOMMON.BRACELIGHT

.. attribute:: STYLESCOMMON.BRACEBAD

.. attribute:: STYLESCOMMON.CONTROLCHAR

.. attribute:: STYLESCOMMON.INDENTGUIDE

.. attribute:: STYLESCOMMON.CALLTIP

.. attribute:: STYLESCOMMON.LASTPREDEFINED

.. attribute:: STYLESCOMMON.MAX

TYPEPROPERTY
------------

.. _TYPEPROPERTY:
.. class:: TYPEPROPERTY

.. attribute:: TYPEPROPERTY.BOOLEAN

.. attribute:: TYPEPROPERTY.INTEGER

.. attribute:: TYPEPROPERTY.STRING

UPDATE
------

.. _UPDATE:
.. class:: UPDATE

.. attribute:: UPDATE.CONTENT

.. attribute:: UPDATE.SELECTION

.. attribute:: UPDATE.V_SCROLL

.. attribute:: UPDATE.H_SCROLL

VIRTUALSPACE
------------

.. _VIRTUALSPACE:
.. class:: VIRTUALSPACE

.. attribute:: VIRTUALSPACE.NONE

.. attribute:: VIRTUALSPACE.RECTANGULARSELECTION

.. attribute:: VIRTUALSPACE.USERACCESSIBLE

WHITESPACE
----------

.. _WHITESPACE:
.. class:: WHITESPACE

.. attribute:: WHITESPACE.INVISIBLE

.. attribute:: WHITESPACE.VISIBLEALWAYS

.. attribute:: WHITESPACE.VISIBLEAFTERINDENT

WRAP
----

.. _WRAP:
.. class:: WRAP

.. attribute:: WRAP.NONE

.. attribute:: WRAP.WORD

.. attribute:: WRAP.CHAR

WRAPINDENTMODE
--------------

.. _WRAPINDENTMODE:
.. class:: WRAPINDENTMODE

.. attribute:: WRAPINDENTMODE.FIXED

.. attribute:: WRAPINDENTMODE.SAME

.. attribute:: WRAPINDENTMODE.INDENT

WRAPVISUALFLAG
--------------

.. _WRAPVISUALFLAG:
.. class:: WRAPVISUALFLAG

.. attribute:: WRAPVISUALFLAG.NONE

.. attribute:: WRAPVISUALFLAG.END

.. attribute:: WRAPVISUALFLAG.START

.. attribute:: WRAPVISUALFLAG.MARGIN

WRAPVISUALLOCATION
------------------

.. _WRAPVISUALLOCATION:
.. class:: WRAPVISUALLOCATION

.. attribute:: WRAPVISUALLOCATION.DEFAULT

.. attribute:: WRAPVISUALLOCATION.END_BY_TEXT

.. attribute:: WRAPVISUALLOCATION.START_BY_TEXT

.. End of autogeneration /* --Autogenerated ----- */


.. _Scintilla: http://www.scintilla.org/
