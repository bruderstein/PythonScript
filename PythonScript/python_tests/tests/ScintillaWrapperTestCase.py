# -*- coding: utf-8 -*-
import unittest
import time

from Npp import *


class ScintillaWrapperTestCase(unittest.TestCase):
    def setUp(self):
        notepad.new()
        self.callbackCalled = False


    def tearDown(self):
        notepad.clearCallbacks()
        editor.clearCallbacks()
        editor.setSavePoint()
        notepad.close()


    def test_scintillawrapper_int_void_textrange(self):
        notepad.setLangType(LANGTYPE.XML)
        editor.write('<test attrib="unit" />')
        result = editor.getStyledText(0, 19)
        self.assertEqual(('<test attrib="unit"', [1, 1, 1, 1, 1, 8, 3, 3, 3, 3, 3, 3, 8, 6, 6, 6, 6, 6, 6]), result)

    def callback_scintillawrapper_int_void_textrange(self, args):
        result = editor.getStyledText(0, 19)
        self.assertEqual(('<test attrib="unit"', [1, 1, 1, 1, 1, 8, 3, 3, 3, 3, 3, 3, 8, 6, 6, 6, 6, 6, 6]), result)
        self.callbackCalled = True

    def test_scintillawrapper_int_void_textrange_in_callback(self):
        notepad.setLangType(LANGTYPE.XML)
        editor.write('<test attrib="unit" />')
        editor.callback(lambda args: self.callback_scintillawrapper_int_void_textrange(args), [SCINTILLANOTIFICATION.SAVEPOINTREACHED])
        editor.setSavePoint()
        time.sleep(0.2)
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
        editor.setTarget(5, 25)
        editor.callback(lambda args: self.callback_scintillawrapper_int_int_string(args), [SCINTILLANOTIFICATION.SAVEPOINTREACHED])
        editor.setSavePoint();
        time.sleep(0.3) # we wait a bit longer, as the callback function has a sleep in it to let N++ mess with the target
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
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_void_int(self):
        pass
        # call one of the following: setMarginLeft, setMarginRight, setDocPointer, addRefDocument

    def callback_scintillawrapper_void_void_int(self, args):
        # call one of the following: setMarginLeft, setMarginRight, setDocPointer, addRefDocument
        self.callbackCalled = True

    def test_scintillawrapper_void_void_int_in_callback(self):
        editor.write('test');
        editor.callback(lambda args: self.callback_scintillawrapper_void_void_int(args), [SCINTILLANOTIFICATION.SAVEPOINTREACHED])
        editor.setSavePoint()
        time.sleep(0.1)
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
        time.sleep(0.1)
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
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_int_colour(self):
        pass
        # call one of the following: markerSetFore, markerSetBack, styleSetFore, styleSetBack

    def callback_scintillawrapper_void_int_colour(self, args):
        # call one of the following: markerSetFore, markerSetBack, styleSetFore, styleSetBack
        self.callbackCalled = True

    def test_scintillawrapper_void_int_colour_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_int_colour(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_position_position(self):
        pass
        # call one of the following: changeLexerState

    def callback_scintillawrapper_int_position_position(self, args):
        # call one of the following: changeLexerState
        self.callbackCalled = True

    def test_scintillawrapper_int_position_position_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_position_position(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_int_int(self):
        pass
        # call one of the following: markerAdd, markerNext, markerPrevious, getLastChild

    def callback_scintillawrapper_int_int_int(self, args):
        # call one of the following: markerAdd, markerNext, markerPrevious, getLastChild
        self.callbackCalled = True

    def test_scintillawrapper_int_int_int_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_int_int(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_keymod_int(self):
        pass
        # call one of the following: assignCmdKey

    def callback_scintillawrapper_void_keymod_int(self, args):
        # call one of the following: assignCmdKey
        self.callbackCalled = True

    def test_scintillawrapper_void_keymod_int_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_keymod_int(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_position_void_void(self):
        pass
        # call one of the following: getCurrentPos, getAnchor, getEndStyled, autoCPosStart

    def callback_scintillawrapper_position_void_void(self, args):
        # call one of the following: getCurrentPos, getAnchor, getEndStyled, autoCPosStart
        self.callbackCalled = True

    def test_scintillawrapper_position_void_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_position_void_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_string_stringresult(self):
        pass
        # call one of the following: encodedFromUTF8, getProperty, getPropertyExpanded, describeProperty

    def callback_scintillawrapper_int_string_stringresult(self, args):
        # call one of the following: encodedFromUTF8, getProperty, getPropertyExpanded, describeProperty
        self.callbackCalled = True

    def test_scintillawrapper_int_string_stringresult_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_string_stringresult(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_position_bool(self):
        pass
        # call one of the following: wordStartPosition, wordEndPosition

    def callback_scintillawrapper_int_position_bool(self, args):
        # call one of the following: wordStartPosition, wordEndPosition
        self.callbackCalled = True

    def test_scintillawrapper_int_position_bool_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_position_bool(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_bool_void(self):
        pass
        # call one of the following: setUndoCollection, setBufferedDraw, setUsePalette, setSelEOLFilled

    def callback_scintillawrapper_void_bool_void(self, args):
        # call one of the following: setUndoCollection, setBufferedDraw, setUsePalette, setSelEOLFilled
        self.callbackCalled = True

    def test_scintillawrapper_void_bool_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_bool_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_int_string(self):
        pass
        # call one of the following: addText, markerDefinePixmap, styleSetFont, setStylingEx

    def callback_scintillawrapper_void_int_string(self, args):
        # call one of the following: addText, markerDefinePixmap, styleSetFont, setStylingEx
        self.callbackCalled = True

    def test_scintillawrapper_void_int_string_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_int_string(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_void_void(self):
        pass
        # call one of the following: getLength, getViewWS, getEOLMode, getTabWidth

    def callback_scintillawrapper_int_void_void(self, args):
        # call one of the following: getLength, getViewWS, getEOLMode, getTabWidth
        self.callbackCalled = True

    def test_scintillawrapper_int_void_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_void_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_position_int(self):
        pass
        # call one of the following: startStyling

    def callback_scintillawrapper_void_position_int(self, args):
        # call one of the following: startStyling
        self.callbackCalled = True

    def test_scintillawrapper_void_position_int_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_position_int(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_string_void(self):
        pass
        # call one of the following: getPropertyInt, propertyType

    def callback_scintillawrapper_int_string_void(self, args):
        # call one of the following: getPropertyInt, propertyType
        self.callbackCalled = True

    def test_scintillawrapper_int_string_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_string_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_position_int_int(self):
        pass
        # call one of the following: positionFromPoint, positionFromPointClose, charPositionFromPoint, charPositionFromPointClose

    def callback_scintillawrapper_position_int_int(self, args):
        # call one of the following: positionFromPoint, positionFromPointClose, charPositionFromPoint, charPositionFromPointClose
        self.callbackCalled = True

    def test_scintillawrapper_position_int_int_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_position_int_int(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_colour_void_void(self):
        pass
        # call one of the following: getCaretLineBack, getCaretFore, getEdgeColour, getHotspotActiveFore

    def callback_scintillawrapper_colour_void_void(self, args):
        # call one of the following: getCaretLineBack, getCaretFore, getEdgeColour, getHotspotActiveFore
        self.callbackCalled = True

    def test_scintillawrapper_colour_void_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_colour_void_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_bool_void_void(self):
        pass
        # call one of the following: canRedo, getUndoCollection, getBufferedDraw, getSelEOLFilled

    def callback_scintillawrapper_bool_void_void(self, args):
        # call one of the following: canRedo, getUndoCollection, getBufferedDraw, getSelEOLFilled
        self.callbackCalled = True

    def test_scintillawrapper_bool_void_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_bool_void_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_position_void(self):
        pass
        # call one of the following: getCharAt, getStyleAt, getColumn, lineFromPosition

    def callback_scintillawrapper_int_position_void(self, args):
        # call one of the following: getCharAt, getStyleAt, getColumn, lineFromPosition
        self.callbackCalled = True

    def test_scintillawrapper_int_position_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_position_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_string_string(self):
        pass
        # call one of the following: setProperty

    def callback_scintillawrapper_void_string_string(self, args):
        # call one of the following: setProperty
        self.callbackCalled = True

    def test_scintillawrapper_void_string_string_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_string_string(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_position_position(self):
        pass
        # call one of the following: setSel, braceHighlight, copyRange, colourise

    def callback_scintillawrapper_void_position_position(self, args):
        # call one of the following: setSel, braceHighlight, copyRange, colourise
        self.callbackCalled = True

    def test_scintillawrapper_void_position_position_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_position_position(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_bool_colour(self):
        pass
        # call one of the following: setSelFore, setSelBack, setWhitespaceFore, setWhitespaceBack

    def callback_scintillawrapper_void_bool_colour(self, args):
        # call one of the following: setSelFore, setSelBack, setWhitespaceFore, setWhitespaceBack
        self.callbackCalled = True

    def test_scintillawrapper_void_bool_colour_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_bool_colour(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_position_position_void(self):
        pass
        # call one of the following: braceMatch, positionBefore, positionAfter

    def callback_scintillawrapper_position_position_void(self, args):
        # call one of the following: braceMatch, positionBefore, positionAfter
        self.callbackCalled = True

    def test_scintillawrapper_position_position_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_position_position_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_keymod_void(self):
        pass
        # call one of the following: clearCmdKey

    def callback_scintillawrapper_void_keymod_void(self, args):
        # call one of the following: clearCmdKey
        self.callbackCalled = True

    def test_scintillawrapper_void_keymod_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_keymod_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_void_void(self):
        pass
        # call one of the following: clearAll, clearDocumentStyle, redo, selectAll

    def callback_scintillawrapper_void_void_void(self, args):
        # call one of the following: clearAll, clearDocumentStyle, redo, selectAll
        self.callbackCalled = True

    def test_scintillawrapper_void_void_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_void_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_int_stringresult(self):
        pass
        # call one of the following: getCurLine, styleGetFont, getLine, getText

    def callback_scintillawrapper_int_int_stringresult(self, args):
        # call one of the following: getCurLine, styleGetFont, getLine, getText
        self.callbackCalled = True

    def test_scintillawrapper_int_int_stringresult_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_int_stringresult(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_colour_void(self):
        pass
        # call one of the following: setCaretFore, setCaretLineBack, callTipSetBack, callTipSetFore

    def callback_scintillawrapper_void_colour_void(self, args):
        # call one of the following: setCaretFore, setCaretLineBack, callTipSetBack, callTipSetFore
        self.callbackCalled = True

    def test_scintillawrapper_void_colour_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_colour_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_int_bool(self):
        pass
        # call one of the following: setMarginSensitiveN, styleSetBold, styleSetItalic, styleSetEOLFilled

    def callback_scintillawrapper_void_int_bool(self, args):
        # call one of the following: setMarginSensitiveN, styleSetBold, styleSetItalic, styleSetEOLFilled
        self.callbackCalled = True

    def test_scintillawrapper_void_int_bool_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_int_bool(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_int_void(self):
        pass
        # call one of the following: markerLineFromHandle, markerGet, getMarginTypeN, getMarginWidthN

    def callback_scintillawrapper_int_int_void(self, args):
        # call one of the following: markerLineFromHandle, markerGet, getMarginTypeN, getMarginWidthN
        self.callbackCalled = True

    def test_scintillawrapper_int_int_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_int_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_position_void(self):
        pass
        # call one of the following: gotoPos, setAnchor, setCurrentPos, setSelectionStart

    def callback_scintillawrapper_void_position_void(self, args):
        # call one of the following: gotoPos, setAnchor, setCurrentPos, setSelectionStart
        self.callbackCalled = True

    def test_scintillawrapper_void_position_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_position_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_position_int_void(self):
        pass
        # call one of the following: getLineIndentPosition, positionFromLine, getLineSelStartPosition, getLineSelEndPosition

    def callback_scintillawrapper_position_int_void(self, args):
        # call one of the following: getLineIndentPosition, positionFromLine, getLineSelStartPosition, getLineSelEndPosition
        self.callbackCalled = True

    def test_scintillawrapper_position_int_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_position_int_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_position_int_findtext(self):
        pass
        # call one of the following: findText

    def callback_scintillawrapper_position_int_findtext(self, args):
        # call one of the following: findText
        self.callbackCalled = True

    def test_scintillawrapper_position_int_findtext_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_position_int_findtext(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_colour_int_void(self):
        pass
        # call one of the following: styleGetFore, styleGetBack, indicGetFore

    def callback_scintillawrapper_colour_int_void(self, args):
        # call one of the following: styleGetFore, styleGetBack, indicGetFore
        self.callbackCalled = True

    def test_scintillawrapper_colour_int_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_colour_int_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_bool_int_void(self):
        pass
        # call one of the following: getMarginSensitiveN, styleGetBold, styleGetItalic, styleGetEOLFilled

    def callback_scintillawrapper_bool_int_void(self, args):
        # call one of the following: getMarginSensitiveN, styleGetBold, styleGetItalic, styleGetEOLFilled
        self.callbackCalled = True

    def test_scintillawrapper_bool_int_void_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_bool_int_void(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_void_string(self):
        pass
        # call one of the following: setWordChars, autoCStops, autoCSelect, autoCSetFillUps

    def callback_scintillawrapper_void_void_string(self, args):
        # call one of the following: setWordChars, autoCStops, autoCSelect, autoCSetFillUps
        self.callbackCalled = True

    def test_scintillawrapper_void_void_string_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_void_string(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_void_stringresult(self):
        pass
        # call one of the following: getSelText, autoCGetCurrentText, targetAsUTF8, getLexerLanguage

    def callback_scintillawrapper_int_void_stringresult(self, args):
        # call one of the following: getSelText, autoCGetCurrentText, targetAsUTF8, getLexerLanguage
        self.callbackCalled = True

    def test_scintillawrapper_int_void_stringresult_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_void_stringresult(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_int_void_position(self):
        pass
        # call one of the following: pointXFromPosition, pointYFromPosition

    def callback_scintillawrapper_int_void_position(self, args):
        # call one of the following: pointXFromPosition, pointYFromPosition
        self.callbackCalled = True

    def test_scintillawrapper_int_void_position_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_int_void_position(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_int_position(self):
        pass
        # call one of the following: setSelectionNCaret, setSelectionNAnchor, setSelectionNStart, setSelectionNEnd

    def callback_scintillawrapper_void_int_position(self, args):
        # call one of the following: setSelectionNCaret, setSelectionNAnchor, setSelectionNStart, setSelectionNEnd
        self.callbackCalled = True

    def test_scintillawrapper_void_int_position_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_int_position(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)

    def test_scintillawrapper_void_int_int(self):
        pass
        # call one of the following: setStyling, markerDefine, markerDelete, markerAddSet

    def callback_scintillawrapper_void_int_int(self, args):
        # call one of the following: setStyling, markerDefine, markerDelete, markerAddSet
        self.callbackCalled = True

    def test_scintillawrapper_void_int_int_in_callback(self):
        editor.callback(lambda args: self.callback_scintillawrapper_void_int_int(args), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write("test");
        time.sleep(0.1)
        self.assertEqual(self.callbackCalled, True)



suite = unittest.TestLoader().loadTestsFromTestCase(ScintillaWrapperTestCase)
