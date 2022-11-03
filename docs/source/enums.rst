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

   Arguments contains: ``modifiers``, ``position``, ``margin``

.. attribute:: SCINTILLANOTIFICATION.MARGINRIGHTCLICK

   Arguments contains: ``modifiers``, ``position``, ``margin``

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

   This enum contains all the messages defined in Scintilla - e.g. SCI_APPENDTEXT.  For a complete list, see the `Scintilla <https://www.scintilla.org/ScintillaDoc.html>`_ documentation.


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

.. attribute:: MENUCOMMAND.EDIT_AUTOCOMPLETE_PATH

.. attribute:: MENUCOMMAND.EDIT_BEGINENDSELECT

.. attribute:: MENUCOMMAND.EDIT_BLANKLINEABOVECURRENT

.. attribute:: MENUCOMMAND.EDIT_BLANKLINEBELOWCURRENT

.. attribute:: MENUCOMMAND.EDIT_BLOCK_COMMENT

.. attribute:: MENUCOMMAND.EDIT_BLOCK_COMMENT_SET

.. attribute:: MENUCOMMAND.EDIT_BLOCK_UNCOMMENT

.. attribute:: MENUCOMMAND.EDIT_CHANGESEARCHENGINE

.. attribute:: MENUCOMMAND.EDIT_CHAR_PANEL

.. attribute:: MENUCOMMAND.EDIT_CLEARREADONLY

.. attribute:: MENUCOMMAND.EDIT_CLIPBOARDHISTORY_PANEL

.. attribute:: MENUCOMMAND.EDIT_COLUMNMODE

.. attribute:: MENUCOMMAND.EDIT_COLUMNMODETIP

.. attribute:: MENUCOMMAND.EDIT_COPY

.. attribute:: MENUCOMMAND.EDIT_COPY_BINARY

.. attribute:: MENUCOMMAND.EDIT_CURRENTDIRTOCLIP

.. attribute:: MENUCOMMAND.EDIT_CUT

.. attribute:: MENUCOMMAND.EDIT_CUT_BINARY

.. attribute:: MENUCOMMAND.EDIT_DELETE

.. attribute:: MENUCOMMAND.EDIT_DUP_LINE

.. attribute:: MENUCOMMAND.EDIT_EOL2WS

.. attribute:: MENUCOMMAND.EDIT_FILENAMETOCLIP

.. attribute:: MENUCOMMAND.EDIT_FULLPATHTOCLIP

.. attribute:: MENUCOMMAND.EDIT_FUNCCALLTIP

.. attribute:: MENUCOMMAND.EDIT_INS_TAB

.. attribute:: MENUCOMMAND.EDIT_INVERTCASE

.. attribute:: MENUCOMMAND.EDIT_JOIN_LINES

.. attribute:: MENUCOMMAND.EDIT_LINE_DOWN

.. attribute:: MENUCOMMAND.EDIT_LINE_UP

.. attribute:: MENUCOMMAND.EDIT_LOWERCASE

.. attribute:: MENUCOMMAND.EDIT_LTR

.. attribute:: MENUCOMMAND.EDIT_OPENASFILE

.. attribute:: MENUCOMMAND.EDIT_OPENINFOLDER

.. attribute:: MENUCOMMAND.EDIT_PASTE

.. attribute:: MENUCOMMAND.EDIT_PASTE_AS_HTML

.. attribute:: MENUCOMMAND.EDIT_PASTE_AS_RTF

.. attribute:: MENUCOMMAND.EDIT_PASTE_BINARY

.. attribute:: MENUCOMMAND.EDIT_PROPERCASE_BLEND

.. attribute:: MENUCOMMAND.EDIT_PROPERCASE_FORCE

.. attribute:: MENUCOMMAND.EDIT_RANDOMCASE

.. attribute:: MENUCOMMAND.EDIT_REDO

.. attribute:: MENUCOMMAND.EDIT_REMOVEEMPTYLINES

.. attribute:: MENUCOMMAND.EDIT_REMOVEEMPTYLINESWITHBLANK

.. attribute:: MENUCOMMAND.EDIT_RMV_TAB

.. attribute:: MENUCOMMAND.EDIT_RTL

.. attribute:: MENUCOMMAND.EDIT_SEARCHONINTERNET

.. attribute:: MENUCOMMAND.EDIT_SELECTALL

.. attribute:: MENUCOMMAND.EDIT_SENTENCECASE_BLEND

.. attribute:: MENUCOMMAND.EDIT_SENTENCECASE_FORCE

.. attribute:: MENUCOMMAND.EDIT_SETREADONLY

.. attribute:: MENUCOMMAND.EDIT_SORTLINES_DECIMALCOMMA_ASCENDING

.. attribute:: MENUCOMMAND.EDIT_SORTLINES_DECIMALCOMMA_DESCENDING

.. attribute:: MENUCOMMAND.EDIT_SORTLINES_DECIMALDOT_ASCENDING

.. attribute:: MENUCOMMAND.EDIT_SORTLINES_DECIMALDOT_DESCENDING

.. attribute:: MENUCOMMAND.EDIT_SORTLINES_INTEGER_ASCENDING

.. attribute:: MENUCOMMAND.EDIT_SORTLINES_INTEGER_DESCENDING

.. attribute:: MENUCOMMAND.EDIT_SORTLINES_LEXICOGRAPHIC_ASCENDING

.. attribute:: MENUCOMMAND.EDIT_SORTLINES_LEXICOGRAPHIC_DESCENDING

.. attribute:: MENUCOMMAND.EDIT_SPLIT_LINES

.. attribute:: MENUCOMMAND.EDIT_STREAM_COMMENT

.. attribute:: MENUCOMMAND.EDIT_STREAM_UNCOMMENT

.. attribute:: MENUCOMMAND.EDIT_SW2TAB_ALL

.. attribute:: MENUCOMMAND.EDIT_SW2TAB_LEADING

.. attribute:: MENUCOMMAND.EDIT_TAB2SW

.. attribute:: MENUCOMMAND.EDIT_TRANSPOSE_LINE

.. attribute:: MENUCOMMAND.EDIT_TRIMALL

.. attribute:: MENUCOMMAND.EDIT_TRIMLINEHEAD

.. attribute:: MENUCOMMAND.EDIT_TRIMTRAILING

.. attribute:: MENUCOMMAND.EDIT_TRIM_BOTH

.. attribute:: MENUCOMMAND.EDIT_UNDO

.. attribute:: MENUCOMMAND.EDIT_UPPERCASE

.. attribute:: MENUCOMMAND.EXPORT_FUNC_LIST_AND_QUIT

.. attribute:: MENUCOMMAND.FILEMENU_EXISTCMDPOSITION

.. attribute:: MENUCOMMAND.FILEMENU_LASTONE

.. attribute:: MENUCOMMAND.FILESWITCHER_FILESCLOSE

.. attribute:: MENUCOMMAND.FILESWITCHER_FILESCLOSEOTHERS

.. attribute:: MENUCOMMAND.FILE_CLOSE

.. attribute:: MENUCOMMAND.FILE_CLOSEALL

.. attribute:: MENUCOMMAND.FILE_CLOSEALL_BUT_CURRENT

.. attribute:: MENUCOMMAND.FILE_CLOSEALL_TOLEFT

.. attribute:: MENUCOMMAND.FILE_CLOSEALL_TORIGHT

.. attribute:: MENUCOMMAND.FILE_DELETE

.. attribute:: MENUCOMMAND.FILE_EXIT

.. attribute:: MENUCOMMAND.FILE_LOADSESSION

.. attribute:: MENUCOMMAND.FILE_NEW

.. attribute:: MENUCOMMAND.FILE_OPEN

.. attribute:: MENUCOMMAND.FILE_OPENFOLDERASWORSPACE

.. attribute:: MENUCOMMAND.FILE_OPEN_CMD

.. attribute:: MENUCOMMAND.FILE_OPEN_DEFAULT_VIEWER

.. attribute:: MENUCOMMAND.FILE_OPEN_FOLDER

.. attribute:: MENUCOMMAND.FILE_PRINT

.. attribute:: MENUCOMMAND.FILE_PRINTNOW

.. attribute:: MENUCOMMAND.FILE_RELOAD

.. attribute:: MENUCOMMAND.FILE_RENAME

.. attribute:: MENUCOMMAND.FILE_RESTORELASTCLOSEDFILE

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

.. attribute:: MENUCOMMAND.FORMAT_ISO_8859_13

.. attribute:: MENUCOMMAND.FORMAT_ISO_8859_14

.. attribute:: MENUCOMMAND.FORMAT_ISO_8859_15

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

.. attribute:: MENUCOMMAND.LANG_ASN1

.. attribute:: MENUCOMMAND.LANG_ASP

.. attribute:: MENUCOMMAND.LANG_AU3

.. attribute:: MENUCOMMAND.LANG_AVS

.. attribute:: MENUCOMMAND.LANG_BAANC

.. attribute:: MENUCOMMAND.LANG_BASH

.. attribute:: MENUCOMMAND.LANG_BATCH

.. attribute:: MENUCOMMAND.LANG_BLITZBASIC

.. attribute:: MENUCOMMAND.LANG_C

.. attribute:: MENUCOMMAND.LANG_CAML

.. attribute:: MENUCOMMAND.LANG_CMAKE

.. attribute:: MENUCOMMAND.LANG_COBOL

.. attribute:: MENUCOMMAND.LANG_COFFEESCRIPT

.. attribute:: MENUCOMMAND.LANG_CPP

.. attribute:: MENUCOMMAND.LANG_CS

.. attribute:: MENUCOMMAND.LANG_CSOUND

.. attribute:: MENUCOMMAND.LANG_CSS

.. attribute:: MENUCOMMAND.LANG_D

.. attribute:: MENUCOMMAND.LANG_DIFF

.. attribute:: MENUCOMMAND.LANG_ERLANG

.. attribute:: MENUCOMMAND.LANG_ESCRIPT

.. attribute:: MENUCOMMAND.LANG_EXTERNAL

.. attribute:: MENUCOMMAND.LANG_EXTERNAL_LIMIT

.. attribute:: MENUCOMMAND.LANG_FLASH

.. attribute:: MENUCOMMAND.LANG_FORTH

.. attribute:: MENUCOMMAND.LANG_FORTRAN

.. attribute:: MENUCOMMAND.LANG_FORTRAN_77

.. attribute:: MENUCOMMAND.LANG_FREEBASIC

.. attribute:: MENUCOMMAND.LANG_GUI4CLI

.. attribute:: MENUCOMMAND.LANG_HASKELL

.. attribute:: MENUCOMMAND.LANG_HTML

.. attribute:: MENUCOMMAND.LANG_IHEX

.. attribute:: MENUCOMMAND.LANG_INI

.. attribute:: MENUCOMMAND.LANG_INNO

.. attribute:: MENUCOMMAND.LANG_JAVA

.. attribute:: MENUCOMMAND.LANG_JS

.. attribute:: MENUCOMMAND.LANG_JSON

.. attribute:: MENUCOMMAND.LANG_JSP

.. attribute:: MENUCOMMAND.LANG_KIX

.. attribute:: MENUCOMMAND.LANG_LATEX

.. attribute:: MENUCOMMAND.LANG_LISP

.. attribute:: MENUCOMMAND.LANG_LUA

.. attribute:: MENUCOMMAND.LANG_MAKEFILE

.. attribute:: MENUCOMMAND.LANG_MATLAB

.. attribute:: MENUCOMMAND.LANG_MMIXAL

.. attribute:: MENUCOMMAND.LANG_NIMROD

.. attribute:: MENUCOMMAND.LANG_NNCRONTAB

.. attribute:: MENUCOMMAND.LANG_NSIS

.. attribute:: MENUCOMMAND.LANG_OBJC

.. attribute:: MENUCOMMAND.LANG_OSCRIPT

.. attribute:: MENUCOMMAND.LANG_PASCAL

.. attribute:: MENUCOMMAND.LANG_PERL

.. attribute:: MENUCOMMAND.LANG_PHP

.. attribute:: MENUCOMMAND.LANG_POWERSHELL

.. attribute:: MENUCOMMAND.LANG_PROPS

.. attribute:: MENUCOMMAND.LANG_PS

.. attribute:: MENUCOMMAND.LANG_PUREBASIC

.. attribute:: MENUCOMMAND.LANG_PYTHON

.. attribute:: MENUCOMMAND.LANG_R

.. attribute:: MENUCOMMAND.LANG_RC

.. attribute:: MENUCOMMAND.LANG_REBOL

.. attribute:: MENUCOMMAND.LANG_REGISTRY

.. attribute:: MENUCOMMAND.LANG_RUBY

.. attribute:: MENUCOMMAND.LANG_RUST

.. attribute:: MENUCOMMAND.LANG_SCHEME

.. attribute:: MENUCOMMAND.LANG_SMALLTALK

.. attribute:: MENUCOMMAND.LANG_SPICE

.. attribute:: MENUCOMMAND.LANG_SQL

.. attribute:: MENUCOMMAND.LANG_SREC

.. attribute:: MENUCOMMAND.LANG_SWIFT

.. attribute:: MENUCOMMAND.LANG_TCL

.. attribute:: MENUCOMMAND.LANG_TEHEX

.. attribute:: MENUCOMMAND.LANG_TEX

.. attribute:: MENUCOMMAND.LANG_TEXT

.. attribute:: MENUCOMMAND.LANG_TXT2TAGS

.. attribute:: MENUCOMMAND.LANG_USER

.. attribute:: MENUCOMMAND.LANG_USER_DLG

.. attribute:: MENUCOMMAND.LANG_USER_LIMIT

.. attribute:: MENUCOMMAND.LANG_VB

.. attribute:: MENUCOMMAND.LANG_VERILOG

.. attribute:: MENUCOMMAND.LANG_VHDL

.. attribute:: MENUCOMMAND.LANG_VISUALPROLOG

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

.. attribute:: MENUCOMMAND.SEARCH_DELETEUNMARKEDLINES

.. attribute:: MENUCOMMAND.SEARCH_FIND

.. attribute:: MENUCOMMAND.SEARCH_FINDCHARINRANGE

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

.. attribute:: MENUCOMMAND.SEARCH_INVERSEMARKS

.. attribute:: MENUCOMMAND.SEARCH_MARK

.. attribute:: MENUCOMMAND.SEARCH_MARKALLEXT1

.. attribute:: MENUCOMMAND.SEARCH_MARKALLEXT2

.. attribute:: MENUCOMMAND.SEARCH_MARKALLEXT3

.. attribute:: MENUCOMMAND.SEARCH_MARKALLEXT4

.. attribute:: MENUCOMMAND.SEARCH_MARKALLEXT5

.. attribute:: MENUCOMMAND.SEARCH_NEXT_BOOKMARK

.. attribute:: MENUCOMMAND.SEARCH_PASTEMARKEDLINES

.. attribute:: MENUCOMMAND.SEARCH_PREV_BOOKMARK

.. attribute:: MENUCOMMAND.SEARCH_REPLACE

.. attribute:: MENUCOMMAND.SEARCH_SELECTMATCHINGBRACES

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

.. attribute:: MENUCOMMAND.SETTING_EDITCONTEXTMENU

.. attribute:: MENUCOMMAND.SETTING_IMPORTPLUGIN

.. attribute:: MENUCOMMAND.SETTING_IMPORTSTYLETHEMS

.. attribute:: MENUCOMMAND.SETTING_PREFERECE

.. attribute:: MENUCOMMAND.SETTING_REMEMBER_LAST_SESSION

.. attribute:: MENUCOMMAND.SETTING_SHORTCUT_MAPPER

.. attribute:: MENUCOMMAND.SETTING_SHORTCUT_MAPPER_MACRO

.. attribute:: MENUCOMMAND.SETTING_SHORTCUT_MAPPER_RUN

.. attribute:: MENUCOMMAND.SETTING_TRAYICON

.. attribute:: MENUCOMMAND.SYSTRAYPOPUP_ACTIVATE

.. attribute:: MENUCOMMAND.SYSTRAYPOPUP_CLOSE

.. attribute:: MENUCOMMAND.SYSTRAYPOPUP_NEWDOC

.. attribute:: MENUCOMMAND.SYSTRAYPOPUP_NEW_AND_PASTE

.. attribute:: MENUCOMMAND.SYSTRAYPOPUP_OPENFILE

.. attribute:: MENUCOMMAND.TOOL_MD5_GENERATE

.. attribute:: MENUCOMMAND.TOOL_MD5_GENERATEFROMFILE

.. attribute:: MENUCOMMAND.TOOL_MD5_GENERATEINTOCLIPBOARD

.. attribute:: MENUCOMMAND.UPDATE_NPP

.. attribute:: MENUCOMMAND.VIEW_ALL_CHARACTERS

.. attribute:: MENUCOMMAND.VIEW_ALWAYSONTOP

.. attribute:: MENUCOMMAND.VIEW_CLONE_TO_ANOTHER_VIEW

.. attribute:: MENUCOMMAND.VIEW_CURLINE_HILITING

.. attribute:: MENUCOMMAND.VIEW_DOCCHANGEMARGIN

.. attribute:: MENUCOMMAND.VIEW_DOC_MAP

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

.. attribute:: MENUCOMMAND.VIEW_FILEBROWSER

.. attribute:: MENUCOMMAND.VIEW_FILESWITCHER_PANEL

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

.. attribute:: MENUCOMMAND.VIEW_FUNC_LIST

.. attribute:: MENUCOMMAND.VIEW_GOTO_ANOTHER_VIEW

.. attribute:: MENUCOMMAND.VIEW_GOTO_NEW_INSTANCE

.. attribute:: MENUCOMMAND.VIEW_HIDELINES

.. attribute:: MENUCOMMAND.VIEW_INDENT_GUIDE

.. attribute:: MENUCOMMAND.VIEW_LINENUMBER

.. attribute:: MENUCOMMAND.VIEW_LOAD_IN_NEW_INSTANCE

.. attribute:: MENUCOMMAND.VIEW_LOCKTABBAR

.. attribute:: MENUCOMMAND.VIEW_LWALIGN

.. attribute:: MENUCOMMAND.VIEW_LWDEF

.. attribute:: MENUCOMMAND.VIEW_LWINDENT

.. attribute:: MENUCOMMAND.VIEW_MONITORING

.. attribute:: MENUCOMMAND.VIEW_POSTIT

.. attribute:: MENUCOMMAND.VIEW_PROJECT_PANEL_1

.. attribute:: MENUCOMMAND.VIEW_PROJECT_PANEL_2

.. attribute:: MENUCOMMAND.VIEW_PROJECT_PANEL_3

.. attribute:: MENUCOMMAND.VIEW_REDUCETABBAR

.. attribute:: MENUCOMMAND.VIEW_REFRESHTABAR

.. attribute:: MENUCOMMAND.VIEW_SUMMARY

.. attribute:: MENUCOMMAND.VIEW_SWITCHTO_OTHER_VIEW

.. attribute:: MENUCOMMAND.VIEW_SYMBOLMARGIN

.. attribute:: MENUCOMMAND.VIEW_SYNSCROLLH

.. attribute:: MENUCOMMAND.VIEW_SYNSCROLLV

.. attribute:: MENUCOMMAND.VIEW_TAB1

.. attribute:: MENUCOMMAND.VIEW_TAB2

.. attribute:: MENUCOMMAND.VIEW_TAB3

.. attribute:: MENUCOMMAND.VIEW_TAB4

.. attribute:: MENUCOMMAND.VIEW_TAB5

.. attribute:: MENUCOMMAND.VIEW_TAB6

.. attribute:: MENUCOMMAND.VIEW_TAB7

.. attribute:: MENUCOMMAND.VIEW_TAB8

.. attribute:: MENUCOMMAND.VIEW_TAB9

.. attribute:: MENUCOMMAND.VIEW_TAB_MOVEBACKWARD

.. attribute:: MENUCOMMAND.VIEW_TAB_MOVEFORWARD

.. attribute:: MENUCOMMAND.VIEW_TAB_NEXT

.. attribute:: MENUCOMMAND.VIEW_TAB_PREV

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

.. attribute:: MENUCOMMAND.VIEW_WRAP

.. attribute:: MENUCOMMAND.VIEW_WRAP_SYMBOL

.. attribute:: MENUCOMMAND.VIEW_ZOOMIN

.. attribute:: MENUCOMMAND.VIEW_ZOOMOUT

.. attribute:: MENUCOMMAND.VIEW_ZOOMRESTORE

.. attribute:: MENUCOMMAND.WIKIFAQ


LANGTYPE
--------

.. _LANGTYPE:
.. class:: LANGTYPE

.. attribute:: LANGTYPE.ADA

.. attribute:: LANGTYPE.ASM

.. attribute:: LANGTYPE.ASN1

.. attribute:: LANGTYPE.ASP

.. attribute:: LANGTYPE.AU3

.. attribute:: LANGTYPE.AVS

.. attribute:: LANGTYPE.BAANC

.. attribute:: LANGTYPE.BASH

.. attribute:: LANGTYPE.BATCH

.. attribute:: LANGTYPE.BLITZBASIC

.. attribute:: LANGTYPE.C

.. attribute:: LANGTYPE.CAML

.. attribute:: LANGTYPE.CMAKE

.. attribute:: LANGTYPE.COBOL

.. attribute:: LANGTYPE.COFFEESCRIPT

.. attribute:: LANGTYPE.CPP

.. attribute:: LANGTYPE.CS

.. attribute:: LANGTYPE.CSOUND

.. attribute:: LANGTYPE.CSS

.. attribute:: LANGTYPE.D

.. attribute:: LANGTYPE.DIFF

.. attribute:: LANGTYPE.ERLANG

.. attribute:: LANGTYPE.ESCRIPT

.. attribute:: LANGTYPE.FLASH

.. attribute:: LANGTYPE.FORTH

.. attribute:: LANGTYPE.FORTRAN

.. attribute:: LANGTYPE.FORTRAN_77

.. attribute:: LANGTYPE.FREEBASIC

.. attribute:: LANGTYPE.GUI4CLI

.. attribute:: LANGTYPE.HASKELL

.. attribute:: LANGTYPE.HTML

.. attribute:: LANGTYPE.IHEX

.. attribute:: LANGTYPE.INI

.. attribute:: LANGTYPE.INNO

.. attribute:: LANGTYPE.JAVA

.. attribute:: LANGTYPE.JAVASCRIPT

.. attribute:: LANGTYPE.JS

.. attribute:: LANGTYPE.JSON

.. attribute:: LANGTYPE.JSP

.. attribute:: LANGTYPE.KIX

.. attribute:: LANGTYPE.LATEX

.. attribute:: LANGTYPE.LISP

.. attribute:: LANGTYPE.LUA

.. attribute:: LANGTYPE.MAKEFILE

.. attribute:: LANGTYPE.MATLAB

.. attribute:: LANGTYPE.MMIXAL

.. attribute:: LANGTYPE.NIMROD

.. attribute:: LANGTYPE.NNCRONTAB

.. attribute:: LANGTYPE.NSIS

.. attribute:: LANGTYPE.OBJC

.. attribute:: LANGTYPE.OSCRIPT

.. attribute:: LANGTYPE.PASCAL

.. attribute:: LANGTYPE.PERL

.. attribute:: LANGTYPE.PHP

.. attribute:: LANGTYPE.POWERSHELL

.. attribute:: LANGTYPE.PROPS

.. attribute:: LANGTYPE.PS

.. attribute:: LANGTYPE.PUREBASIC

.. attribute:: LANGTYPE.PYTHON

.. attribute:: LANGTYPE.R

.. attribute:: LANGTYPE.RC

.. attribute:: LANGTYPE.REBOL

.. attribute:: LANGTYPE.REGISTRY

.. attribute:: LANGTYPE.RUBY

.. attribute:: LANGTYPE.RUST

.. attribute:: LANGTYPE.SCHEME

.. attribute:: LANGTYPE.SEARCHRESULT

.. attribute:: LANGTYPE.SMALLTALK

.. attribute:: LANGTYPE.SPICE

.. attribute:: LANGTYPE.SQL

.. attribute:: LANGTYPE.SREC

.. attribute:: LANGTYPE.SWIFT

.. attribute:: LANGTYPE.TCL

.. attribute:: LANGTYPE.TEHEX

.. attribute:: LANGTYPE.TEX

.. attribute:: LANGTYPE.TXT

.. attribute:: LANGTYPE.TXT2TAGS

.. attribute:: LANGTYPE.USER

.. attribute:: LANGTYPE.VB

.. attribute:: LANGTYPE.VERILOG

.. attribute:: LANGTYPE.VHDL

.. attribute:: LANGTYPE.VISUALPROLOG

.. attribute:: LANGTYPE.XML

.. attribute:: LANGTYPE.YAML


WINVER
------

.. _WINVER:
.. class:: WINVER

.. attribute:: WINVER.UNKNOWN

.. attribute:: WINVER.WIN32S

.. attribute:: WINVER.95

.. attribute:: WINVER.98

.. attribute:: WINVER.ME

.. attribute:: WINVER.NT

.. attribute:: WINVER.W2K

.. attribute:: WINVER.XP

.. attribute:: WINVER.S2003

.. attribute:: WINVER.XPX64

.. attribute:: WINVER.VISTA

.. attribute:: WINVER.WIN7

.. attribute:: WINVER.WIN8

.. attribute:: WINVER.WIN81

.. attribute:: WINVER.WIN10


.. The rest of this file is autogenerated from Scintilla. To edit change CreateWrapper.py /* ++Autogenerated ----- */


ACCESSIBILITY
-------------

.. _ACCESSIBILITY:
.. class:: ACCESSIBILITY

.. attribute:: ACCESSIBILITY.DISABLED

.. attribute:: ACCESSIBILITY.ENABLED

ALPHA
-----

.. _ALPHA:
.. class:: ALPHA

.. attribute:: ALPHA.TRANSPARENT

.. attribute:: ALPHA.OPAQUE

.. attribute:: ALPHA.NOALPHA

ANNOTATIONVISIBLE
-----------------

.. _ANNOTATIONVISIBLE:
.. class:: ANNOTATIONVISIBLE

.. attribute:: ANNOTATIONVISIBLE.HIDDEN

.. attribute:: ANNOTATIONVISIBLE.STANDARD

.. attribute:: ANNOTATIONVISIBLE.BOXED

.. attribute:: ANNOTATIONVISIBLE.INDENTED

AUTOCOMPLETEOPTION
------------------

.. _AUTOCOMPLETEOPTION:
.. class:: AUTOCOMPLETEOPTION

.. attribute:: AUTOCOMPLETEOPTION.NORMAL

.. attribute:: AUTOCOMPLETEOPTION.FIXED_SIZE

AUTOMATICFOLD
-------------

.. _AUTOMATICFOLD:
.. class:: AUTOMATICFOLD

.. attribute:: AUTOMATICFOLD.NONE

.. attribute:: AUTOMATICFOLD.SHOW

.. attribute:: AUTOMATICFOLD.CLICK

.. attribute:: AUTOMATICFOLD.CHANGE

BIDIRECTIONAL
-------------

.. _BIDIRECTIONAL:
.. class:: BIDIRECTIONAL

.. attribute:: BIDIRECTIONAL.DISABLED

.. attribute:: BIDIRECTIONAL.L2R

.. attribute:: BIDIRECTIONAL.R2L

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

.. attribute:: CARETSTYLE.OVERSTRIKE_BAR

.. attribute:: CARETSTYLE.OVERSTRIKE_BLOCK

.. attribute:: CARETSTYLE.CURSES

.. attribute:: CARETSTYLE.INS_MASK

.. attribute:: CARETSTYLE.BLOCK_AFTER

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

.. attribute:: CASEVISIBLE.CAMEL

CHANGEHISTORYOPTION
-------------------

.. _CHANGEHISTORYOPTION:
.. class:: CHANGEHISTORYOPTION

.. attribute:: CHANGEHISTORYOPTION.DISABLED

.. attribute:: CHANGEHISTORYOPTION.ENABLED

.. attribute:: CHANGEHISTORYOPTION.MARKERS

.. attribute:: CHANGEHISTORYOPTION.INDICATORS

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

.. attribute:: CHARACTERSET.OEM866

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

CHARACTERSOURCE
---------------

.. _CHARACTERSOURCE:
.. class:: CHARACTERSOURCE

.. attribute:: CHARACTERSOURCE.DIRECT_INPUT

.. attribute:: CHARACTERSOURCE.TENTATIVE_INPUT

.. attribute:: CHARACTERSOURCE.IME_RESULT

COMPLETIONMETHODS
-----------------

.. _COMPLETIONMETHODS:
.. class:: COMPLETIONMETHODS

.. attribute:: COMPLETIONMETHODS.FILLUP

.. attribute:: COMPLETIONMETHODS.DOUBLECLICK

.. attribute:: COMPLETIONMETHODS.TAB

.. attribute:: COMPLETIONMETHODS.NEWLINE

.. attribute:: COMPLETIONMETHODS.COMMAND

CURSORSHAPE
-----------

.. _CURSORSHAPE:
.. class:: CURSORSHAPE

.. attribute:: CURSORSHAPE.NORMAL

.. attribute:: CURSORSHAPE.ARROW

.. attribute:: CURSORSHAPE.WAIT

.. attribute:: CURSORSHAPE.REVERSEARROW

DOCUMENTOPTION
--------------

.. _DOCUMENTOPTION:
.. class:: DOCUMENTOPTION

.. attribute:: DOCUMENTOPTION.DEFAULT

.. attribute:: DOCUMENTOPTION.STYLES_NONE

.. attribute:: DOCUMENTOPTION.TEXT_LARGE

EOLANNOTATIONVISIBLE
--------------------

.. _EOLANNOTATIONVISIBLE:
.. class:: EOLANNOTATIONVISIBLE

.. attribute:: EOLANNOTATIONVISIBLE.HIDDEN

.. attribute:: EOLANNOTATIONVISIBLE.STANDARD

.. attribute:: EOLANNOTATIONVISIBLE.BOXED

.. attribute:: EOLANNOTATIONVISIBLE.STADIUM

.. attribute:: EOLANNOTATIONVISIBLE.FLAT_CIRCLE

.. attribute:: EOLANNOTATIONVISIBLE.ANGLE_CIRCLE

.. attribute:: EOLANNOTATIONVISIBLE.CIRCLE_FLAT

.. attribute:: EOLANNOTATIONVISIBLE.FLATS

.. attribute:: EOLANNOTATIONVISIBLE.ANGLE_FLAT

.. attribute:: EOLANNOTATIONVISIBLE.CIRCLE_ANGLE

.. attribute:: EOLANNOTATIONVISIBLE.FLAT_ANGLE

.. attribute:: EOLANNOTATIONVISIBLE.ANGLES

EDGEVISUALSTYLE
---------------

.. _EDGEVISUALSTYLE:
.. class:: EDGEVISUALSTYLE

.. attribute:: EDGEVISUALSTYLE.NONE

.. attribute:: EDGEVISUALSTYLE.LINE

.. attribute:: EDGEVISUALSTYLE.BACKGROUND

.. attribute:: EDGEVISUALSTYLE.MULTILINE

ELEMENT
-------

.. _ELEMENT:
.. class:: ELEMENT

.. attribute:: ELEMENT.LIST

.. attribute:: ELEMENT.LIST_BACK

.. attribute:: ELEMENT.LIST_SELECTED

.. attribute:: ELEMENT.LIST_SELECTED_BACK

.. attribute:: ELEMENT.SELECTION_TEXT

.. attribute:: ELEMENT.SELECTION_BACK

.. attribute:: ELEMENT.SELECTION_ADDITIONAL_TEXT

.. attribute:: ELEMENT.SELECTION_ADDITIONAL_BACK

.. attribute:: ELEMENT.SELECTION_SECONDARY_TEXT

.. attribute:: ELEMENT.SELECTION_SECONDARY_BACK

.. attribute:: ELEMENT.SELECTION_INACTIVE_TEXT

.. attribute:: ELEMENT.SELECTION_INACTIVE_BACK

.. attribute:: ELEMENT.CARET

.. attribute:: ELEMENT.CARET_ADDITIONAL

.. attribute:: ELEMENT.CARET_LINE_BACK

.. attribute:: ELEMENT.WHITE_SPACE

.. attribute:: ELEMENT.WHITE_SPACE_BACK

.. attribute:: ELEMENT.HOT_SPOT_ACTIVE

.. attribute:: ELEMENT.HOT_SPOT_ACTIVE_BACK

.. attribute:: ELEMENT.FOLD_LINE

.. attribute:: ELEMENT.HIDDEN_LINE

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

.. attribute:: FINDOPTION.NONE

.. attribute:: FINDOPTION.WHOLEWORD

.. attribute:: FINDOPTION.MATCHCASE

.. attribute:: FINDOPTION.WORDSTART

.. attribute:: FINDOPTION.REGEXP

.. attribute:: FINDOPTION.POSIX

.. attribute:: FINDOPTION.CXX11REGEX

FOCUSCHANGE
-----------

.. _FOCUSCHANGE:
.. class:: FOCUSCHANGE

.. attribute:: FOCUSCHANGE.CHANGE

.. attribute:: FOCUSCHANGE.SETFOCUS

.. attribute:: FOCUSCHANGE.KILLFOCUS

FOLDACTION
----------

.. _FOLDACTION:
.. class:: FOLDACTION

.. attribute:: FOLDACTION.CONTRACT

.. attribute:: FOLDACTION.EXPAND

.. attribute:: FOLDACTION.TOGGLE

.. attribute:: FOLDACTION.CONTRACT_EVERY_LEVEL

FOLDDISPLAYTEXTSTYLE
--------------------

.. _FOLDDISPLAYTEXTSTYLE:
.. class:: FOLDDISPLAYTEXTSTYLE

.. attribute:: FOLDDISPLAYTEXTSTYLE.HIDDEN

.. attribute:: FOLDDISPLAYTEXTSTYLE.STANDARD

.. attribute:: FOLDDISPLAYTEXTSTYLE.BOXED

FOLDFLAG
--------

.. _FOLDFLAG:
.. class:: FOLDFLAG

.. attribute:: FOLDFLAG.NONE

.. attribute:: FOLDFLAG.LINEBEFORE_EXPANDED

.. attribute:: FOLDFLAG.LINEBEFORE_CONTRACTED

.. attribute:: FOLDFLAG.LINEAFTER_EXPANDED

.. attribute:: FOLDFLAG.LINEAFTER_CONTRACTED

.. attribute:: FOLDFLAG.LEVELNUMBERS

.. attribute:: FOLDFLAG.LINESTATE

FOLDLEVEL
---------

.. _FOLDLEVEL:
.. class:: FOLDLEVEL

.. attribute:: FOLDLEVEL.NONE

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

IMEINTERACTION
--------------

.. _IMEINTERACTION:
.. class:: IMEINTERACTION

.. attribute:: IMEINTERACTION.WINDOWED

.. attribute:: IMEINTERACTION.INLINE

IDLESTYLING
-----------

.. _IDLESTYLING:
.. class:: IDLESTYLING

.. attribute:: IDLESTYLING.NONE

.. attribute:: IDLESTYLING.TOVISIBLE

.. attribute:: IDLESTYLING.AFTERVISIBLE

.. attribute:: IDLESTYLING.ALL

INDENTVIEW
----------

.. _INDENTVIEW:
.. class:: INDENTVIEW

.. attribute:: INDENTVIEW.NONE

.. attribute:: INDENTVIEW.REAL

.. attribute:: INDENTVIEW.LOOKFORWARD

.. attribute:: INDENTVIEW.LOOKBOTH

INDICFLAG
---------

.. _INDICFLAG:
.. class:: INDICFLAG

.. attribute:: INDICFLAG.NONE

.. attribute:: INDICFLAG.VALUEFORE

INDICVALUE
----------

.. _INDICVALUE:
.. class:: INDICVALUE

.. attribute:: INDICVALUE.BIT

.. attribute:: INDICVALUE.MASK

INDICATORNUMBERS
----------------

.. _INDICATORNUMBERS:
.. class:: INDICATORNUMBERS

.. attribute:: INDICATORNUMBERS.CONTAINER

.. attribute:: INDICATORNUMBERS.IME

.. attribute:: INDICATORNUMBERS.IME_MAX

.. attribute:: INDICATORNUMBERS.HISTORY_REVERTED_TO_ORIGIN_INSERTION

.. attribute:: INDICATORNUMBERS.HISTORY_REVERTED_TO_ORIGIN_DELETION

.. attribute:: INDICATORNUMBERS.HISTORY_SAVED_INSERTION

.. attribute:: INDICATORNUMBERS.HISTORY_SAVED_DELETION

.. attribute:: INDICATORNUMBERS.HISTORY_MODIFIED_INSERTION

.. attribute:: INDICATORNUMBERS.HISTORY_MODIFIED_DELETION

.. attribute:: INDICATORNUMBERS.HISTORY_REVERTED_TO_MODIFIED_INSERTION

.. attribute:: INDICATORNUMBERS.HISTORY_REVERTED_TO_MODIFIED_DELETION

.. attribute:: INDICATORNUMBERS.MAX

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

.. attribute:: INDICATORSTYLE.COMPOSITIONTHIN

.. attribute:: INDICATORSTYLE.FULLBOX

.. attribute:: INDICATORSTYLE.TEXTFORE

.. attribute:: INDICATORSTYLE.POINT

.. attribute:: INDICATORSTYLE.POINTCHARACTER

.. attribute:: INDICATORSTYLE.GRADIENT

.. attribute:: INDICATORSTYLE.GRADIENTCENTRE

.. attribute:: INDICATORSTYLE.POINT_TOP

.. attribute:: INDICATORSTYLE.CONTAINER

.. attribute:: INDICATORSTYLE.IME

.. attribute:: INDICATORSTYLE.IME_MAX

.. attribute:: INDICATORSTYLE.MAX

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

LAYER
-----

.. _LAYER:
.. class:: LAYER

.. attribute:: LAYER.BASE

.. attribute:: LAYER.UNDER_TEXT

.. attribute:: LAYER.OVER_TEXT

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

.. attribute:: LEXER.AS

.. attribute:: LEXER.DMIS

.. attribute:: LEXER.REGISTRY

.. attribute:: LEXER.BIBTEX

.. attribute:: LEXER.SREC

.. attribute:: LEXER.IHEX

.. attribute:: LEXER.TEHEX

.. attribute:: LEXER.JSON

.. attribute:: LEXER.EDIFACT

.. attribute:: LEXER.INDENT

.. attribute:: LEXER.MAXIMA

.. attribute:: LEXER.STATA

.. attribute:: LEXER.SAS

.. attribute:: LEXER.NIM

.. attribute:: LEXER.CIL

.. attribute:: LEXER.X12

.. attribute:: LEXER.DATAFLEX

.. attribute:: LEXER.HOLLYWOOD

.. attribute:: LEXER.RAKU

.. attribute:: LEXER.FSHARP

.. attribute:: LEXER.JULIA

.. attribute:: LEXER.ASCIIDOC

.. attribute:: LEXER.GDSCRIPT

.. attribute:: LEXER.AUTOMATIC

LINECACHE
---------

.. _LINECACHE:
.. class:: LINECACHE

.. attribute:: LINECACHE.NONE

.. attribute:: LINECACHE.CARET

.. attribute:: LINECACHE.PAGE

.. attribute:: LINECACHE.DOCUMENT

LINECHARACTERINDEXTYPE
----------------------

.. _LINECHARACTERINDEXTYPE:
.. class:: LINECHARACTERINDEXTYPE

.. attribute:: LINECHARACTERINDEXTYPE.NONE

.. attribute:: LINECHARACTERINDEXTYPE.UTF32

.. attribute:: LINECHARACTERINDEXTYPE.UTF16

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

.. attribute:: MARGINTYPE.COLOUR

MARKEROUTLINE
-------------

.. _MARKEROUTLINE:
.. class:: MARKEROUTLINE

.. attribute:: MARKEROUTLINE.HISTORY_REVERTED_TO_ORIGIN

.. attribute:: MARKEROUTLINE.HISTORY_SAVED

.. attribute:: MARKEROUTLINE.HISTORY_MODIFIED

.. attribute:: MARKEROUTLINE.HISTORY_REVERTED_TO_MODIFIED

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

.. attribute:: MARKERSYMBOL.VERTICALBOOKMARK

.. attribute:: MARKERSYMBOL.BAR

.. attribute:: MARKERSYMBOL.CHARACTER

MODIFICATIONFLAGS
-----------------

.. _MODIFICATIONFLAGS:
.. class:: MODIFICATIONFLAGS

.. attribute:: MODIFICATIONFLAGS.NONE

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

.. attribute:: MODIFICATIONFLAGS.INSERTCHECK

.. attribute:: MODIFICATIONFLAGS.CHANGETABSTOPS

.. attribute:: MODIFICATIONFLAGS.CHANGEEOLANNOTATION

.. attribute:: MODIFICATIONFLAGS.MODEVENTMASKALL

MULTIAUTOCOMPLETE
-----------------

.. _MULTIAUTOCOMPLETE:
.. class:: MULTIAUTOCOMPLETE

.. attribute:: MULTIAUTOCOMPLETE.ONCE

.. attribute:: MULTIAUTOCOMPLETE.EACH

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

PHASESDRAW
----------

.. _PHASESDRAW:
.. class:: PHASESDRAW

.. attribute:: PHASESDRAW.ONE

.. attribute:: PHASESDRAW.TWO

.. attribute:: PHASESDRAW.MULTIPLE

POPUP
-----

.. _POPUP:
.. class:: POPUP

.. attribute:: POPUP.NEVER

.. attribute:: POPUP.ALL

.. attribute:: POPUP.TEXT

PRINTOPTION
-----------

.. _PRINTOPTION:
.. class:: PRINTOPTION

.. attribute:: PRINTOPTION.NORMAL

.. attribute:: PRINTOPTION.INVERTLIGHT

.. attribute:: PRINTOPTION.BLACKONWHITE

.. attribute:: PRINTOPTION.COLOURONWHITE

.. attribute:: PRINTOPTION.COLOURONWHITEDEFAULTBG

.. attribute:: PRINTOPTION.SCREENCOLOURS

REPRESENTATIONAPPEARANCE
------------------------

.. _REPRESENTATIONAPPEARANCE:
.. class:: REPRESENTATIONAPPEARANCE

.. attribute:: REPRESENTATIONAPPEARANCE._PLAIN

.. attribute:: REPRESENTATIONAPPEARANCE._BLOB

.. attribute:: REPRESENTATIONAPPEARANCE._COLOUR

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

.. attribute:: STATUS.WARN_START

.. attribute:: STATUS.WARN_REGEX

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

.. attribute:: STYLESCOMMON.FOLDDISPLAYTEXT

.. attribute:: STYLESCOMMON.LASTPREDEFINED

.. attribute:: STYLESCOMMON.MAX

SUPPORTS
--------

.. _SUPPORTS:
.. class:: SUPPORTS

.. attribute:: SUPPORTS.LINE_DRAWS_FINAL

.. attribute:: SUPPORTS.PIXEL_DIVISIONS

.. attribute:: SUPPORTS.FRACTIONAL_STROKE_WIDTH

.. attribute:: SUPPORTS.TRANSLUCENT_STROKE

.. attribute:: SUPPORTS.PIXEL_MODIFICATION

.. attribute:: SUPPORTS.THREAD_SAFE_MEASURE_WIDTHS

TABDRAWMODE
-----------

.. _TABDRAWMODE:
.. class:: TABDRAWMODE

.. attribute:: TABDRAWMODE.LONGARROW

.. attribute:: TABDRAWMODE.STRIKEOUT

TECHNOLOGY
----------

.. _TECHNOLOGY:
.. class:: TECHNOLOGY

.. attribute:: TECHNOLOGY.DEFAULT

.. attribute:: TECHNOLOGY.DIRECTWRITE

.. attribute:: TECHNOLOGY.DIRECTWRITERETAIN

.. attribute:: TECHNOLOGY.DIRECTWRITEDC

TYPEPROPERTY
------------

.. _TYPEPROPERTY:
.. class:: TYPEPROPERTY

.. attribute:: TYPEPROPERTY.BOOLEAN

.. attribute:: TYPEPROPERTY.INTEGER

.. attribute:: TYPEPROPERTY.STRING

UNDOFLAGS
---------

.. _UNDOFLAGS:
.. class:: UNDOFLAGS

.. attribute:: UNDOFLAGS.NONE

.. attribute:: UNDOFLAGS.MAY_COALESCE

UPDATE
------

.. _UPDATE:
.. class:: UPDATE

.. attribute:: UPDATE.NONE

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

.. attribute:: VIRTUALSPACE.NOWRAPLINESTART

VISIBLEPOLICY
-------------

.. _VISIBLEPOLICY:
.. class:: VISIBLEPOLICY

.. attribute:: VISIBLEPOLICY.SLOP

.. attribute:: VISIBLEPOLICY.STRICT

WHITESPACE
----------

.. _WHITESPACE:
.. class:: WHITESPACE

.. attribute:: WHITESPACE.INVISIBLE

.. attribute:: WHITESPACE.VISIBLEALWAYS

.. attribute:: WHITESPACE.VISIBLEAFTERINDENT

.. attribute:: WHITESPACE.VISIBLEONLYININDENT

WRAP
----

.. _WRAP:
.. class:: WRAP

.. attribute:: WRAP.NONE

.. attribute:: WRAP.WORD

.. attribute:: WRAP.CHAR

.. attribute:: WRAP.WHITESPACE

WRAPINDENTMODE
--------------

.. _WRAPINDENTMODE:
.. class:: WRAPINDENTMODE

.. attribute:: WRAPINDENTMODE.FIXED

.. attribute:: WRAPINDENTMODE.SAME

.. attribute:: WRAPINDENTMODE.INDENT

.. attribute:: WRAPINDENTMODE.DEEPINDENT

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

