# -*- coding: utf-8 -*-
import unittest
import time

from Npp import (
    editor, notepad, SCINTILLANOTIFICATION, FINDOPTION, KEYMOD,
    SCINTILLAMESSAGE, LEXER, MODIFICATIONFLAGS, LANGTYPE, Cell
    )


class ScintillaWrapperTestCase(unittest.TestCase):
    def setUp(self):
        # Make doubly sure we've got no lingering callbacks waiting
        notepad.clearCallbacks()
        editor.clearCallbacks()
        notepad.new()
        self.callbackCalled = False


    def tearDown(self):
        notepad.clearCallbacks()
        editor.clearCallbacks()
        editor.setSavePoint()
        notepad.close()

    def poll_for_callback(self, timeout = 0.5, interval = 0.1):
        while self.callbackCalled == False and timeout > 0:
            time.sleep(interval)
            timeout -= interval

        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_void_textrange(self):
        notepad.setLangType(LANGTYPE.XML)
        editor.write('<test attrib="unit" />')
        time.sleep(0.1)  # let the lexer do it's thang. The getStyledText appears to fail occassionally, returns a bunch of zeroes for the styles
                         # I'm assuming it's a timing bug, as it "normally" works. Added the sleep to see if it makes a difference.
        result = editor.getStyledText(0, 19)
        self.assertEqual(('<test attrib="unit"', [1, 1, 1, 1, 1, 8, 3, 3, 3, 3, 3, 3, 8, 6, 6, 6, 6, 6, 6]), result)

    def callback_scintillawrapper_int_void_textrange(self, args):
        result = editor.getStyledText(0, 19)
        self.assertEqual(('<test attrib="unit"', [1, 1, 1, 1, 1, 8, 3, 3, 3, 3, 3, 3, 8, 6, 6, 6, 6, 6, 6]), result)
        self.callbackCalled = True

    def test_scintillawrapper_int_void_textrange_in_callback(self):
        notepad.setLangType(LANGTYPE.XML)
        editor.write('<test attrib="unit" />')
        time.sleep(0.1)  # lexing and threading don't seem to mix
        editor.callback(lambda args: self.callback_scintillawrapper_int_void_textrange(args), [SCINTILLANOTIFICATION.SAVEPOINTREACHED])
        editor.setSavePoint()
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_int_string(self):
        editor.write('ABC This is a some ABC text we are going to search')
        time.sleep(0.1) # let N++ mess with the target
        editor.setSearchFlags(0)
        editor.setTarget(5, 25)
        result = editor.searchInTarget('ABC')
        self.assertEqual(result, 19)

    def callback_scintillawrapper_int_int_string(self, args):
        time.sleep(0.1) # let N++ mess with the target
        editor.setTarget(5, 25)
        self.assertEqual(editor.getTargetStart(), 5)
        self.assertEqual(editor.getTargetEnd(), 25)
        result = editor.searchInTarget('ABC')
        self.assertEqual(result, 19)
        self.callbackCalled = True

    def test_scintillawrapper_int_int_string_in_callback(self):
        editor.write('ABC This is a some ABC text we are going to search')
        editor.callback(lambda args: self.callback_scintillawrapper_int_int_string(args), [SCINTILLANOTIFICATION.SAVEPOINTREACHED])
        editor.setSavePoint();
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_int_cells(self):
        notepad.setLangType(LANGTYPE.TXT)
        editor.addStyledText(Cell('Test', [1,2,3,4]))
        result = editor.getStyledText(0, 4)
        self.assertEqual(result, ('Test',[1,2,3,4]))


    def callback_scintillawrapper_void_int_cells(self, args):
        notepad.setLangType(LANGTYPE.TXT)
        editor.addStyledText(Cell('Test', [1,2,3,4]))
        result = editor.getStyledText(0, 4)
        self.assertEqual(result, ('Test',[1,2,3,4]))
        self.callbackCalled = True

    def test_scintillawrapper_void_int_cells_in_callback(self):
        editor.write('Hi')
        editor.setText('')
        editor.callback(lambda args: self.callback_scintillawrapper_void_int_cells(args), [SCINTILLANOTIFICATION.SAVEPOINTREACHED])
        editor.setSavePoint()
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

	# TODO: NPP BUG - Crash
    @unittest.skipUnless(notepad.getVersion() > (7,5,8), "NPP BUG STILL EXISTS")
    def test_scintillawrapper_void_void_int(self):
        # we'll grab the doc point of the current document, then create another scintilla, set it to the document, write text in it,
        # then grab the text from the main 'editor' Scintilla, which should be what we added in
        docPointer = editor.getDocPointer()
        notepad.outputDebugString('creating hidden scintilla\n')
        hiddenScintilla = notepad.createScintilla()
        notepad.outputDebugString('setting doc pointer in hidden scintilla\n');
        hiddenScintilla.setDocPointer(docPointer)
        notepad.outputDebugString('complete - set doc pointer in hidden scintilla\n');
        hiddenScintilla.write('hello world, from the other side')
        text = editor.getText()
        notepad.outputDebugString('about to destroy scintilla\n');
        notepad.destroyScintilla(hiddenScintilla)
        notepad.outputDebugString('destroyed scintilla\n');
        self.assertEqual(text, 'hello world, from the other side')

    def callback_scintillawrapper_void_void_int(self, args):
        editor.clearCallbacks()
        editor.clearAll()
        self.test_scintillawrapper_void_void_int()
        self.callbackCalled = True

    def test_scintillawrapper_void_void_int_in_callback(self):
        notepad.outputDebugString('test_scintillawrapper_void_void_int_in_callback')
        editor.write('test');
        editor.callback(lambda args: self.callback_scintillawrapper_void_void_int(args), [SCINTILLANOTIFICATION.SAVEPOINTREACHED])
        editor.setSavePoint()
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_int_void(self):
        editor.write('one\r\ntwo\r\nthree\r\n')
        before = editor.getCurLine()
        editor.gotoLine(1)
        after = editor.getCurLine()
        self.assertEqual(after, 'two\r\n')
        self.assertNotEqual(before, after)

    def callback_scintillawrapper_void_int_void(self, args):
        editor.write('one\r\ntwo\r\nthree\r\n')
        before = editor.getCurLine()
        editor.gotoLine(1)
        after = editor.getCurLine()
        self.assertEqual(after, 'two\r\n')
        self.assertNotEqual(before, after)
        self.callbackCalled = True

    def test_scintillawrapper_void_int_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_int_void(args), [SCINTILLANOTIFICATION.SAVEPOINTREACHED])
        editor.setSavePoint()
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_position_string(self):
        editor.write('ABCDEF')
        editor.insertText(3, 'GHI')
        content = editor.getText()
        self.assertEquals('ABCGHIDEF', content)

    def callback_scintillawrapper_void_position_string(self, args):
        editor.write('ABCDEF')
        editor.insertText(3, 'GHI')
        content = editor.getText()
        self.assertEquals('ABCGHIDEF', content)
        self.callbackCalled = True

    def test_scintillawrapper_void_position_string_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_position_string(args), [SCINTILLANOTIFICATION.SAVEPOINTREACHED])
        editor.setSavePoint()
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_int_colour(self):
        currentColour = editor.styleGetFore(1)
        editor.styleSetFore(1, (218,219,220))
        newColour = editor.styleGetFore(1)
        # reset the style back to the original (assuming it works!)
        editor.styleSetFore(1, currentColour)
        self.assertEquals(newColour, (218,219,220))
        self.assertNotEquals(currentColour, newColour)


    def callback_scintillawrapper_void_int_colour(self, args):
        self.test_scintillawrapper_void_int_colour()
        self.callbackCalled = True

    def test_scintillawrapper_void_int_colour_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_int_colour(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test")
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_position_position(self):
        # Actually, in Scintilla source, changeLexerState doesn't officially return anything,
        # other than an "always return 0", but Scintilla.iface has it as int, so we'll check it's a 0.
        result = editor.changeLexerState(3,7)
        self.assertEqual(result, 0)
        # nothing else to check, just makes sure the call doesn't throw an error/deadlock

    def callback_scintillawrapper_int_position_position(self, args):
        self.test_scintillawrapper_int_position_position()
        self.callbackCalled = True

    def test_scintillawrapper_int_position_position_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_position_position(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write('0123456789')
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_int_int(self):
        editor.write('line 1\r\nline 2\r\nline 3\r\nline 4\r\n')
        handle = editor.markerAdd(2, 1)
        self.assertNotEqual(handle, -1)  # markerAdd() returns -1 if the line number is invalid or out of memory
        editor.insertText(0, 'inserted line 1\r\n')
        newLineNumber = editor.markerLineFromHandle(handle)
        self.assertEqual(newLineNumber, 3)

    def callback_scintillawrapper_int_int_int(self, args):
        editor.clearCallbacks()
        self.test_scintillawrapper_int_int_int()
        self.callbackCalled = True

    def test_scintillawrapper_int_int_int_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_int_int(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test")
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_keymod_int(self):
        editor.assignCmdKey(77 | ((KEYMOD.ALT | KEYMOD.CTRL) << 16), SCINTILLAMESSAGE.SCI_COPYALLOWLINE)
        # nothing to test

    def callback_scintillawrapper_void_keymod_int(self, args):
        editor.assignCmdKey(77 | ((KEYMOD.ALT | KEYMOD.CTRL) << 16), SCINTILLAMESSAGE.SCI_COPYALLOWLINE)
        self.callbackCalled = True

    def test_scintillawrapper_void_keymod_int_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_keymod_int(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test")
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_position_void_void(self):
        editor.write("1234")
        position = editor.getCurrentPos()
        self.assertEqual(position, 4)

    def callback_scintillawrapper_position_void_void(self, args):
        if args["modificationType"] & MODIFICATIONFLAGS.INSERTTEXT == 0:   # Ignore anything that isn't INSERTTEXT
            return
        editor.clearCallbacks()
        editor.write("1234")
        position = editor.getCurrentPos()
        self.assertEqual(position, 8)  # test1234|
        self.callbackCalled = True

    def test_scintillawrapper_position_void_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_position_void_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test")
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_string_stringresult(self):
        editor.setProperty('pythonscript.unittest', 'test1234')
        propertyValue = editor.getProperty('pythonscript.unittest')
        self.assertEqual(propertyValue, 'test1234')

    def callback_scintillawrapper_int_string_stringresult(self, args):
        editor.setProperty('pythonscript.unittest', 'test1234')
        propertyValue = editor.getProperty('pythonscript.unittest')
        self.assertEqual(propertyValue, 'test1234')
        self.callbackCalled = True

    def test_scintillawrapper_int_string_stringresult_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_string_stringresult(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_position_bool(self):
        editor.write('abcdef ghijkl mnopqrst')
        wordStart = editor.wordStartPosition(10, False)
        self.assertEqual(wordStart, 7)


    def callback_scintillawrapper_int_position_bool(self, args):
        editor.clearCallbacks()
        editor.setText('abcdef ghijkl mnopqrst')
        wordStart = editor.wordStartPosition(10, False)
        self.assertEqual(wordStart, 7)
        self.callbackCalled = True

    def test_scintillawrapper_int_position_bool_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_position_bool(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_bool_void(self):
        editor.setUndoCollection(False)
        editor.emptyUndoBuffer()   # recommended when disabling undo collection
        editor.write('test_void_bool_void')
        editor.setUndoCollection(True)
        editor.undo()
        afterUndoWithUndoDisabled = editor.getText()

        # Now check undo is back on again
        editor.write('extra')
        beforeUndo = editor.getText()
        editor.undo()
        afterUndo = editor.getText()

        self.assertEqual(afterUndoWithUndoDisabled, 'test_void_bool_void')
        self.assertEqual(beforeUndo, 'test_void_bool_voidextra')
        self.assertEqual(afterUndo, 'test_void_bool_void')

    def callback_scintillawrapper_void_bool_void(self, args):
        editor.clearCallbacks()
        editor.setText('')
        self.test_scintillawrapper_void_bool_void()
        self.callbackCalled = True

    def test_scintillawrapper_void_bool_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_bool_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_int_string(self):
        editor.setText('abc123')
        editor.gotoPos(3)
        editor.addText('def')
        resultText = editor.getText()
        self.assertEqual(resultText, 'abcdef123')

    def callback_scintillawrapper_void_int_string(self, args):
        originalFontName = editor.styleGetFont(1)
        editor.styleSetFont(1, 'Comic Sans')  # what else?
        newFontName = editor.styleGetFont(1)
        editor.styleSetFont(1, originalFontName)
        self.assertEqual(newFontName, 'Comic Sans')
        # Check it wasn't already comic sans
        self.assertNotEqual(originalFontName, newFontName)
        self.callbackCalled = True

    def test_scintillawrapper_void_int_string_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_int_string(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_void_void(self):
        editor.setText('12345')
        length = editor.getLength()
        self.assertEqual(length, 5)

    def callback_scintillawrapper_int_void_void(self, args):
        length = editor.getLength()
        self.assertEqual(length, 5)
        self.callbackCalled = True

    def test_scintillawrapper_int_void_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_void_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("12345");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_position_int(self):
        editor.setText('Hello world')
        editor.setLexer(LEXER.CONTAINER)
        editor.startStyling(0, 31)
        editor.setStyling(5, 29)
        styledText = editor.getStyledText(0, 5)
        editor.setLexer(LEXER.NULL)
        self.assertEqual(styledText, ('Hello', [29, 29, 29, 29, 29]))


    def callback_scintillawrapper_void_position_int(self, args):
        editor.clearCallbacks()
        self.test_scintillawrapper_void_position_int()
        self.callbackCalled = True

    def test_scintillawrapper_void_position_int_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_position_int(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_string_void(self):
        editor.setProperty('pythonscript.unittest', '422')
        result = editor.getPropertyInt('pythonscript.unittest', 0)
        self.assertEqual(result, 422)

    def callback_scintillawrapper_int_string_void(self, args):
        self.test_scintillawrapper_int_string_void()
        self.callbackCalled = True

    def test_scintillawrapper_int_string_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_string_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_position_int_int(self):
        editor.write('Hello World');
        x = editor.pointXFromPosition(6) # X position of the W
        y = editor.pointYFromPosition(6)

        position = editor.positionFromPoint(x, y)
        self.assertEqual(position, 6)

    def callback_scintillawrapper_position_int_int(self, args):
        x = editor.pointXFromPosition(6) # X position of the W
        y = editor.pointYFromPosition(6)

        position = editor.positionFromPoint(x, y)
        self.assertEqual(position, 6)
        self.callbackCalled = True

    def test_scintillawrapper_position_int_int_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_position_int_int(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("Hello World")
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_colour_void_void(self):
        originalColour = editor.getCaretFore()
        editor.setCaretFore((120, 130, 150))
        newColour = editor.getCaretFore()
        editor.setCaretFore(originalColour)
        self.assertEqual(newColour, (120, 130, 150))


    def callback_scintillawrapper_colour_void_void(self, args):
        self.test_scintillawrapper_colour_void_void()
        self.callbackCalled = True

    def test_scintillawrapper_colour_void_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_colour_void_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_bool_void_void(self):
        editor.setUndoCollection(True)
        shouldBeTrue = editor.getUndoCollection()
        editor.setUndoCollection(False)
        shouldBeFalse = editor.getUndoCollection()
        editor.setUndoCollection(True)
        self.assertEqual(shouldBeTrue, True)
        self.assertEqual(shouldBeFalse, False)



    def callback_scintillawrapper_bool_void_void(self, args):
        self.test_scintillawrapper_bool_void_void()
        self.callbackCalled = True

    def test_scintillawrapper_bool_void_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_bool_void_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    # There's a few of these tests for int_position_void, because they are key methods,
    # and there's some odd return types (chars etc)
    def test_scintillawrapper_int_position_void_getCharAt(self):
        editor.write('Hello World\r\nCheese\r\nCheddar')
        endOfHello = editor.getCharAt(4)

        self.assertEqual(endOfHello, ord('o'))

    def test_scintillawrapper_int_position_void_getColumn(self):
        editor.write('Hello World\r\nCheese\r\nCheddar')
        columnOfE = editor.getColumn(15)  # the first 'e' in Cheese
        self.assertEqual(columnOfE, 2)

    def test_scintillawrapper_int_position_void_lineFromPosition(self):
        editor.write('Hello World\r\nCheese\r\nCheddar')
        lineOfCheddar = editor.lineFromPosition(24)
        self.assertEqual(lineOfCheddar, 2)

    def callback_scintillawrapper_int_position_void(self, args):
        editor.clearCallbacks()
        editor.setText('')
        self.test_scintillawrapper_int_position_void_getColumn()
        editor.setText('')
        self.test_scintillawrapper_int_position_void_getCharAt()
        editor.setText('')
        self.test_scintillawrapper_int_position_void_lineFromPosition()
        self.callbackCalled = True

    def test_scintillawrapper_int_position_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_position_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        # self.poll_for_callback()
        self.poll_for_callback()


    def test_scintillawrapper_void_string_string(self):
        editor.setProperty('pythonscript.unittest', 'void_string_string')
        value = editor.getProperty('pythonscript.unittest')
        editor.setProperty('pythonscript.unittest', '')   # clear the property out again
        self.assertEqual(value, 'void_string_string')

    def callback_scintillawrapper_void_string_string(self, args):
        self.test_scintillawrapper_void_string_string()
        self.callbackCalled = True

    def test_scintillawrapper_void_string_string_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_string_string(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_position_position(self):
        editor.write('abc123 def456')
        editor.copyRange(4, 10)
        editor.paste()
        text = editor.getText()
        self.assertEqual(text, 'abc123 def45623 def')

    def callback_scintillawrapper_void_position_position(self, args):
        editor.clearCallbacks()
        editor.setText('')
        self.test_scintillawrapper_void_position_position()
        self.callbackCalled = True

    def test_scintillawrapper_void_position_position_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_position_position(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_bool_colour(self):
        editor.setSelFore(True, (150,150,150))
        editor.setSelFore(False, (0,0,0))
        # nothing to test, we just check the command runs (there is no getSelFore())

    def callback_scintillawrapper_void_bool_colour(self, args):
        self.test_scintillawrapper_void_bool_colour()
        self.callbackCalled = True

    def test_scintillawrapper_void_bool_colour_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_bool_colour(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_position_position_void(self):
        editor.write(u'äü')
        result1 = editor.positionBefore(2)
        result2 = editor.positionBefore(4)
        # both chars are 2 bytes long, before(2) should 0, before(4) should be 2
        self.assertEqual(result1, 0)
        self.assertEqual(result2, 2)

    def callback_scintillawrapper_position_position_void(self, args):
        result1 = editor.positionBefore(2)
        result2 = editor.positionBefore(4)
        # both chars are 2 bytes long, before(2) should 0, before(4) should be 2
        self.assertEqual(result1, 0)
        self.assertEqual(result2, 2)
        self.callbackCalled = True

    def test_scintillawrapper_position_position_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_position_position_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write(u'äü')
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_keymod_void(self):
        key = ((KEYMOD.SHIFT | KEYMOD.ALT | KEYMOD.ALT) << 16) | ord('P')
        editor.assignCmdKey(key, SCINTILLAMESSAGE.SCI_COPYALLOWLINE)
        editor.clearCmdKey(key)

    def callback_scintillawrapper_void_keymod_void(self, args):
        self.test_scintillawrapper_void_keymod_void()
        self.callbackCalled = True

    def test_scintillawrapper_void_keymod_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_keymod_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_void_void(self):
        editor.write('Hello')
        editor.write(' world')
        before = editor.getText()
        editor.clearAll()
        after = editor.getText()
        self.assertEqual(before, 'Hello world')
        self.assertEqual(after, '')

    def callback_scintillawrapper_void_void_void(self, args):
        editor.clearCallbacks()
        editor.setText('')
        self.test_scintillawrapper_void_void_void()
        self.callbackCalled = True

    def test_scintillawrapper_void_void_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_void_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_int_stringresult_getCurLine(self):
        editor.write('One\r\nTwo\r\nThree\r\n')
        editor.gotoPos(6)
        line = editor.getCurLine()
        self.assertEqual(line, 'Two\r\n')

    def test_scintillawrapper_int_int_stringresult_getLine(self):
        editor.write('One\r\nTwo\r\nThree')
        lineTwo = editor.getLine(1)
        lineThree = editor.getLine(2)
        self.assertEqual(lineTwo, 'Two\r\n')
        self.assertEqual(lineThree, 'Three')

    def test_scintillawrapper_int_int_stringresult_styleGetFont(self):
        originalFontName = editor.styleGetFont(31)
        editor.styleSetFont(31, 'Comic Sans')
        fontName = editor.styleGetFont(31)
        editor.styleSetFont(31, originalFontName)
        self.assertEqual(fontName, 'Comic Sans')
        self.assertNotEqual(originalFontName, fontName)  # we assume no-one uses Comic Sans, but just in case...

    def callback_scintillawrapper_int_int_stringresult_styleGetFont(self, args):
        self.test_scintillawrapper_int_int_stringresult_styleGetFont()
        self.callbackCalled = True

    def test_scintillawrapper_int_int_stringresult_in_callback_styleGetFont(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_int_stringresult_styleGetFont(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)


    def callback_scintillawrapper_int_int_stringresult_getLine(self, args):
        lineTwo = editor.getLine(1)
        lineThree = editor.getLine(2)
        self.assertEqual(lineTwo, 'Two\r\n')
        self.assertEqual(lineThree, 'Three')
        self.callbackCalled = True

    def test_scintillawrapper_int_int_stringresult_in_callback_getLine(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_int_stringresult_getLine(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("One\r\nTwo\r\nThree");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def callback_scintillawrapper_int_int_stringresult_getCurLine(self, args):
        editor.gotoPos(6)
        line = editor.getCurLine()
        self.assertEqual(line, 'Two\r\n')
        self.callbackCalled = True

    def test_scintillawrapper_int_int_stringresult_in_callback_getCurLine(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_int_stringresult_getLine(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("One\r\nTwo\r\nThree");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)


    def test_scintillawrapper_void_colour_void(self):
        originalColour = editor.getCaretFore()
        editor.setCaretFore((255,0,0))
        newColour = editor.getCaretFore()
        editor.setCaretFore(originalColour)
        self.assertEqual(newColour, (255, 0, 0))


    def callback_scintillawrapper_void_colour_void(self, args):
        self.test_scintillawrapper_void_colour_void()
        self.callbackCalled = True

    def test_scintillawrapper_void_colour_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_colour_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_int_bool(self):
        original = editor.styleGetBold(31)
        editor.styleSetBold(31, True)
        shouldBeTrue = editor.styleGetBold(31)
        editor.styleSetBold(31, False)
        shouldBeFalse = editor.styleGetBold(31)
        editor.styleSetBold(31, original)
        self.assertEqual(shouldBeTrue, True)
        self.assertEqual(shouldBeFalse, False)


    def callback_scintillawrapper_void_int_bool(self, args):
        self.test_scintillawrapper_void_int_bool()
        self.callbackCalled = True

    def test_scintillawrapper_void_int_bool_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_int_bool(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_int_void(self):
        editor.write('One\r\nTwo\r\nThree\r\nFour')

        handle = editor.markerAdd(3, 4)  # Add marker 4 at line 3

        beforeMoveMarker = editor.markerGet(3)
        editor.insertText(0, 'Add a line\r\n')
        afterMove3 = editor.markerGet(3)
        afterMove4 = editor.markerGet(4)
        lineNumber = editor.markerLineFromHandle(handle)

        self.assertEqual(beforeMoveMarker & 0x10, 0x10)  # marker 4 is 0x10, and there could be other markers on the line
        self.assertEqual(afterMove3 & 0x10, 0)           # after the insert, line 3 should not contain the marker
        self.assertEqual(afterMove4 & 0x10, 0x10)        # it should be on line 4
        self.assertEqual(lineNumber, 4)                  # The lineNumber obtained from the handle, should also be 4


    def callback_scintillawrapper_int_int_void(self, args):
        editor.clearCallbacks()
        editor.clearAll()
        self.test_scintillawrapper_int_int_void()
        self.callbackCalled = True

    def test_scintillawrapper_int_int_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_int_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_position_void(self):
        editor.write('Hello world 12356')
        editor.gotoPos(6)
        afterGotoPos = editor.getCurrentPos()
        editor.setCurrentPos(10)
        afterSetCurrentPos = editor.getCurrentPos()
        self.assertEqual(afterGotoPos, 6)
        self.assertEqual(afterSetCurrentPos, 10)


    def callback_scintillawrapper_void_position_void(self, args):
        editor.clearCallbacks()
        self.test_scintillawrapper_void_position_void()
        self.callbackCalled = True

    def test_scintillawrapper_void_position_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_position_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_position_int_void(self):
        editor.write('Hello\r\n\t    World')
        indentPosition = editor.getLineIndentPosition(1)
        self.assertEqual(indentPosition, 12)

    def callback_scintillawrapper_position_int_void(self, args):
        indentPosition = editor.getLineIndentPosition(1)
        self.assertEqual(indentPosition, 12)
        self.callbackCalled = True

    def test_scintillawrapper_position_int_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_position_int_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write('Hello\r\n\t    World')
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_position_int_findtext(self):
        editor.write('ac123 def456\r\nghi11111 xl45')
        result = editor.findText(FINDOPTION.REGEXP, 0, 30, '[a-z]{3}[0-9]{3}')
        self.assertEqual(result, (6, 12))

    def callback_scintillawrapper_position_int_findtext(self, args):
        result = editor.findText(FINDOPTION.REGEXP, 0, 30, '[a-z]{3}[0-9]{3}')
        self.assertEqual(result, (6, 12))
        self.callbackCalled = True

    def test_scintillawrapper_position_int_findtext_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_position_int_findtext(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write('ac123 def456\r\nghi11111 xl45')
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_colour_int_void(self):
        original = editor.styleGetFore(4)
        editor.styleSetFore(4, (191, 195, 198))
        replaced = editor.styleGetFore(4)
        editor.styleSetFore(4, original)
        self.assertEqual(replaced, (191, 195, 198))

    def callback_scintillawrapper_colour_int_void(self, args):
        self.test_scintillawrapper_colour_int_void()
        self.callbackCalled = True

    def test_scintillawrapper_colour_int_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_colour_int_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_bool_int_void(self):
        _ = editor.getMarginSensitiveN(1)
        editor.setMarginSensitiveN(1, True)
        shouldBeTrue = editor.getMarginSensitiveN(1)
        editor.setMarginSensitiveN(1, False)
        shouldBeFalse = editor.getMarginSensitiveN(1)
        self.assertEqual(shouldBeTrue, True)
        self.assertEqual(shouldBeFalse, False)

    def callback_scintillawrapper_bool_int_void(self, args):
        self.test_scintillawrapper_bool_int_void()
        self.callbackCalled = True

    def test_scintillawrapper_bool_int_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_bool_int_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_void_string_lexerLanguage(self):
        _ = editor.getLexerLanguage()
        editor.setLexerLanguage('python')
        python = editor.getLexerLanguage()
        self.assertEqual(python, 'python')

    def callback_scintillawrapper_void_void_string_lexerLanguage(self, args):
        self.test_scintillawrapper_void_void_string_lexerLanguage()
        self.callbackCalled = True

    def test_scintillawrapper_void_void_string_in_callback_lexerLanguage(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_void_string_lexerLanguage(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_void_string(self):
        originalWordChars = editor.getWordChars()
        editor.setWordChars('abcdefghijklmnop')
        changedWordChars = editor.getWordChars()
        editor.setWordChars(originalWordChars)
        for ch in changedWordChars:
            self.assertIn(ch, 'abcdefghijklmnop')
        self.assertEqual(len(changedWordChars), len('abcdefghijklmnop'))

    def callback_scintillawrapper_void_void_string(self, args):
        self.test_scintillawrapper_void_void_string()
        self.callbackCalled = True

    def test_scintillawrapper_void_void_string_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_void_string(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_void_stringresult(self):
        editor.write('abc123 def456 ghi789')
        editor.setSel(7, 13)
        text = editor.getSelText()
        self.assertEqual(text, 'def456')


    def callback_scintillawrapper_int_void_stringresult(self, args):
        editor.clearCallbacks()
        editor.setSel(7, 13)
        text = editor.getSelText()
        self.assertEqual(text, 'def456')
        self.callbackCalled = True

    def test_scintillawrapper_int_void_stringresult_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_void_stringresult(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write('abc123 def456 ghi789')
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_void_position(self):
        # This test is actually identical to position_int_int
        # That test is for positionFromPoint(), this is for pointXFromPosition()
        # I'm leaving this in, so we've definitely got a covering test for int_void_position
        editor.write('Hello World');
        x = editor.pointXFromPosition(6) # X position of the W
        y = editor.pointYFromPosition(6)
        position = editor.positionFromPoint(x, y)
        self.assertEqual(position, 6)

    def callback_scintillawrapper_int_void_position(self, args):
        x = editor.pointXFromPosition(6) # X position of the W
        y = editor.pointYFromPosition(6)

        position = editor.positionFromPoint(x, y)
        self.assertEqual(position, 6)
        self.callbackCalled = True

    def test_scintillawrapper_int_void_position_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_void_position(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("Hello World")
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_int_position(self):
        editor.write('abc123 def456 ghi789 jk12 lmnop3456')
        editor.setSel(7, 13)
        editor.addSelection(14, 20)
        editor.setSelectionNCaret(1, 16)
        editor.setSelectionNAnchor(1, 18)
        # Note: Do not attempt to call setSelectionNCaret on a selection that doesn't exist - it causes a crash.
        # Always call addSelection first
        caret = editor.getSelectionNCaret(1)
        anchor = editor.getSelectionNAnchor(1)

        self.assertEqual(caret, 16)
        self.assertEqual(anchor, 18)


    def callback_scintillawrapper_void_int_position(self, args):
        editor.clearCallbacks()
        self.test_scintillawrapper_void_int_position()
        self.callbackCalled = True

    def test_scintillawrapper_void_int_position_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_int_position(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_int_int(self):
        editor.write('One\r\nTwo\r\nThree\r\nFour')
        editor.markerAddSet(2, 0x11) # marker 4 (0x10) and 0 (0x01) on line 2
        markersBeforeMove = editor.markerGet(2)
        editor.insertText(0, 'Inserted\r\n')
        markersAfterMove = editor.markerGet(2)
        markersAfterMoveLine3 = editor.markerGet(3)

        self.assertEqual(markersBeforeMove, 0x11)
        self.assertEqual(markersAfterMove, 0)
        self.assertEqual(markersAfterMoveLine3, 0x11)

    def callback_scintillawrapper_void_int_int(self, args):
        editor.clearCallbacks()
        self.test_scintillawrapper_void_int_int()
        self.callbackCalled = True

    def test_scintillawrapper_void_int_int_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_int_int(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        self.poll_for_callback()
        self.assertEqual(self.callbackCalled, True)


    def test_annotationSetText_as_text(self):
        editor.write('One\r\nTwo\r\nThree')
        editor.annotationSetText(1, 'This is line two')
        text = editor.annotationGetText(1)
        self.assertEqual(text, 'This is line two')

    def test_annotationSetText_clear(self):
        editor.write('One\r\nTwo\r\nThree')
        editor.annotationSetText(1, 'This is line two')
        before = editor.annotationGetText(1)
        editor.annotationSetText(1, None)
        after = editor.annotationGetText(1)
        self.assertEqual(before, 'This is line two')
        self.assertEqual(after, '')

    def test_getRangePointer(self):
        editor.write('Hello world')
        rangeText = editor.getRangePointer(3, 5)
        self.assertEqual(rangeText, 'lo wo')

    def test_getCharacterPointer(self):
        editor.write('Hello world char pointer')
        text = editor.getCharacterPointer()
        self.assertEqual(text, 'Hello world char pointer')

    def test_deleteLine_with_contents(self):
        editor.write('Line 1\r\nLine 2\r\nLine 3\r\n')
        editor.deleteLine(1)
        text = editor.getText();
        self.assertEqual(text, 'Line 1\r\nLine 3\r\n')

    def test_deleteLine_middle_no_contents(self):
        editor.write('Line 1\r\n\r\nLine 3\r\n')
        editor.deleteLine(1)
        text = editor.getText();
        self.assertEqual(text, 'Line 1\r\nLine 3\r\n')

    def test_deleteLine_end_no_contents(self):
        editor.write('Line 1\r\nLine 2\r\n\r\n')
        editor.deleteLine(2)
        text = editor.getText();
        self.assertEqual(text, 'Line 1\r\nLine 2\r\n')

    def test_deleteLine_end_no_eol(self):
        editor.write('Line 1\r\nLine 2\r\nLine 3')
        editor.deleteLine(2)
        text = editor.getText();
        self.assertEqual(text, 'Line 1\r\nLine 2\r\n')

    def test_deleteLine_start_no_contents(self):
        editor.write('\r\nLine 2\r\nLine 3\r\n')
        editor.deleteLine(0)
        text = editor.getText();
        self.assertEqual(text, 'Line 2\r\nLine 3\r\n')


suite = unittest.TestLoader().loadTestsFromTestCase(ScintillaWrapperTestCase)
