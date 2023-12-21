# -*- coding: utf-8 -*-
import unittest
import time
import tempfile
import os
from Npp import *

globalCallbackCalled = False

def dummy_callback(args):
    global callbackCalled
    editor.write('Dummy Callback called in error')
    globalCallbackCalled = True

def global_poll_for_callback(timeout = 0.5, interval = 0.1):
    global globalCallbackCalled
    while globalCallbackCalled == False and timeout > 0:
        time.sleep(interval)
        timeout -= interval

class NotepadCallbackTestCase(unittest.TestCase):
    def setUp(self):
        notepad.new()
        self.callbackCalled = False
        self.files_to_delete = []

    def tearDown(self):
        notepad.clearCallbacks()
        editor.clearCallbacks()
        editor.setSavePoint()
        notepad.close()
        for file in self.files_to_delete:
            os.remove(file)

    def get_temp_filename(self):
        (file, filename) = tempfile.mkstemp()
        os.close(file)
        self.files_to_delete.append(filename)
        return filename

    def poll_for_callback(self, timeout = 0.5, interval = 0.1):
        while self.callbackCalled == False and timeout > 0:
            time.sleep(interval)
            timeout -= interval


    def callback_lang_changed(self, args):
        self.callbackCalled = True

    def test_notepad_callback(self):
        notepad.callback(lambda a: self.callback_lang_changed(a), [NOTIFICATION.LANGCHANGED])
        notepad.setLangType(LANGTYPE.PHP)
        self.poll_for_callback()
        self.assertTrue(self.callbackCalled)

    def callback_nested(self, args):
        langType = notepad.getLangType()
        if langType == LANGTYPE.PHP:             # first call
            notepad.setLangType(LANGTYPE.PYTHON)
        elif langType == LANGTYPE.PYTHON:        # second call
            notepad.setLangType(LANGTYPE.XML)
        elif langType == LANGTYPE.XML:
            self.callbackCalled = True

    def test_notepad_nested_callbacks(self):
        notepad.callback(lambda a: self.callback_nested(a), [NOTIFICATION.LANGCHANGED])
        notepad.setLangType(LANGTYPE.PHP)
        self.poll_for_callback()
        currentLangType = notepad.getLangType()
        self.assertTrue(self.callbackCalled)
        self.assertEqual(currentLangType, LANGTYPE.XML)  # LANGTYPE.XML is the third nested call

    def callback_register_2(self, args):
        self.callbackCalled = True

    def callback_register(self, args):
        notepad.callback(lambda a: self.callback_register_2(a), [NOTIFICATION.LANGCHANGED])
        notepad.setLangType(LANGTYPE.XML)


    def test_callback_register_in_callback(self):
        notepad.callback(lambda a: self.callback_register(a), [NOTIFICATION.LANGCHANGED])
        notepad.setLangType(LANGTYPE.PHP)

        self.poll_for_callback()
        self.assertTrue(self.callbackCalled)

    def callback_for_remove(self, args):
        self.callbackCalled = True

    def test_remove_all_callbacks(self):
        notepad.callback(dummy_callback, [NOTIFICATION.LANGCHANGED])
        notepad.clearCallbacks()
        global_poll_for_callback(0.3)  # don't poll for too long, as we're not expecting the callback
        self.assertFalse(globalCallbackCalled)

    def test_remove_callback_via_method(self):
        notepad.callback(lambda a: self.callback_for_remove(a), [NOTIFICATION.LANGCHANGED])
        notepad.callback(dummy_callback, [NOTIFICATION.LANGCHANGED])
        notepad.clearCallbacks(dummy_callback)
        notepad.setLangType(LANGTYPE.PHP)
        global_poll_for_callback(0.3)  # don't poll for too long, as we're not expecting the callback
        self.poll_for_callback()
        self.assertFalse(globalCallbackCalled)
        self.assertTrue(self.callbackCalled)   # the second callback should still have been called

    def test_remove_callback_via_notification(self):
        notepad.callback(lambda a: self.callback_for_remove(a), [NOTIFICATION.FILESAVED])
        notepad.callback(dummy_callback, [NOTIFICATION.LANGCHANGED])
        notepad.clearCallbacks([NOTIFICATION.LANGCHANGED])
        editor.write('clear list test')
        filename = self.get_temp_filename()

        notepad.saveAs(filename)
        notepad.setLangType(LANGTYPE.PHP)

        global_poll_for_callback(0.3)  # don't poll for too long, as we're not expecting the callback
        self.poll_for_callback()
        self.assertFalse(globalCallbackCalled)
        self.assertTrue(self.callbackCalled)   # the second callback (FILESAVED) should still have been called

    def test_remove_callback_via_method_and_notification(self):
        notepad.callback(lambda a: self.callback_for_remove(a), [NOTIFICATION.FILESAVED])
        notepad.callback(dummy_callback, [NOTIFICATION.LANGCHANGED])
        notepad.clearCallbacks(dummy_callback, [NOTIFICATION.LANGCHANGED])
        editor.write('root')

        filename = self.get_temp_filename()
        notepad.saveAs(filename)
        notepad.setLangType(LANGTYPE.PHP)
        global_poll_for_callback(0.3)  # don't poll for too long, as we're not expecting the callback
        self.poll_for_callback()
        self.assertFalse(globalCallbackCalled)
        self.assertTrue(self.callbackCalled)   # the second callback should still have been called


    def callback_editor(self, args):
        self.callbackCalled = True

    def callback_notepad_trigger_editor(self, args):
        editor.write('editor callback from  notepad callback')

    def test_editor_callback_from_notepad_callback(self):
        editor.callback(lambda a: self.callback_editor(a), [SCINTILLANOTIFICATION.MODIFIED])
        notepad.callback(lambda a: self.callback_notepad_trigger_editor(a), [NOTIFICATION.LANGCHANGED])
        notepad.setLangType(LANGTYPE.PHP)
        self.poll_for_callback()
        self.assertTrue(self.callbackCalled)

    def callback_notepad(self, args):
        self.callbackCalled = True

    def callback_editor_trigger_notepad(self, args):
        notepad.setLangType(LANGTYPE.PHP)

    def test_notepad_callback_from_editor_callback(self):
        notepad.callback(lambda a: self.callback_notepad(a), [NOTIFICATION.LANGCHANGED])
        editor.callback(lambda a: self.callback_editor_trigger_notepad(a), [SCINTILLANOTIFICATION.MODIFIED])

        editor.write('trigger editor callback')
        self.poll_for_callback()
        self.assertTrue(self.callbackCalled)


    def test_notepad_callback_with_disallowed_sync_method(self):
        """This checks calling a method in a n++ callback, that is not allowed in
        a synchronous Scintilla callback - specifically because N++ callbacks are synchronous, but
        allow all methods"""

        editor.write('File 1')
        notepad.saveAs(self.get_temp_filename())
        self.oldBufferID = notepad.getCurrentBufferID()
        notepad.new()
        editor.write('File 2')
        notepad.saveAs(self.get_temp_filename())
        editor.write('Change')
        notepad.callback(lambda a: self.callback_with_disallowed_sync_method(a), [NOTIFICATION.FILEBEFORESAVE])
        notepad.save()
        currentBufferID = notepad.getCurrentBufferID()
        # Close the second opened file, then both get closed neatly
        notepad.close()

        self.assertTrue(self.callbackCalled)
        self.assertEqual(self.oldBufferID, currentBufferID)


    def callback_with_disallowed_sync_method(self, args):
        notepad.activateBufferID(self.oldBufferID)
        self.callbackCalled = True


suite = unittest.TestLoader().loadTestsFromTestCase(NotepadCallbackTestCase)