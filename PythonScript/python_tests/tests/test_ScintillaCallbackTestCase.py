# -*- coding: utf-8 -*-
import unittest
import time
from Npp import notepad, editor, SCINTILLANOTIFICATION, MODIFICATIONFLAGS

globalCallbackCalled = False

def dummy_callback(args):
    global globalCallbackCalled
    editor.write('Dummy Callback called in error')
    globalCallbackCalled = True

def global_poll_for_callback(timeout = 0.5, interval = 0.1):
    global globalCallbackCalled
    while (not globalCallbackCalled) and (timeout > 0):
        time.sleep(interval)
        timeout -= interval

class ScintillaCallbackTestCase(unittest.TestCase):
    def setUp(self):
        notepad.new()
        self.callbackCalled = False
        self.callbackResults = {}

    def tearDown(self):
        notepad.clearCallbacks()
        editor.clearCallbacks()
        editor.setSavePoint()
        notepad.close()

    def updateui_callback(self, args):
        self.assertNotEqual(args['updated'], 0)
        self.assertNotEqual(args['updated'], None)
        self.callbackCalled = True


    def disabled_test_callback_updateui(self):
        editor.callback(lambda args: self.updateui_callback(args), [SCINTILLANOTIFICATION.UPDATEUI], )
        notepad.new()
        editor.write('test update ui')
        self.poll_for_callback()
        self.assertEqual(self.called, True)


    def modified_callback(self, args):
        pass

    def test_modified_callback(self):
        # Although the callback doesn't do anything, this checks that when lots of callbacks are generated in succession
        # we don't get any deadlocks.  This used to be a common problem.
        editor.callback(lambda a: self.modified_callback(a), [SCINTILLANOTIFICATION.MODIFIED])
        for _ in range(250):
            editor.write('test modified\n')


    def callback_nested(self, args):
        # Look, our very own version of callback hell!
        # Order is as follows:
        #   1. test_nested_callbacks writes 'root', triggers the callback.
        #   2. callback writes 'change1', triggers the callback again
        #   3. callback writes 'change2', triggers the callback
        #   4. callback sees 'change2', and sets the called flag to True.
        if args['modificationType'] & 1 == 0:   # ignore modifications that aren't SC_MOD_INSERTTEXT
            return

        if args['text'] == 'root':
            editor.write('change1')
        elif args['text'] == 'change1':
            editor.write('change2')
        elif args['text'] == 'change2':
            self.callbackCalled = True

    def test_nested_callbacks(self):
        editor.callback(lambda a: self.callback_nested(a), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write('root')
        self.poll_for_callback()
        text = editor.getText()
        self.assertEqual(self.callbackCalled, True)
        # Check the callbacks were called one after another in the right order
        self.assertEqual(text, "rootchange1change2")

    def callback_register_2(self, args):
        if args['modificationType'] & 1 == 0:   # ignore modifications that aren't SC_MOD_INSERTTEXT
            return
        if args['text'] == 'change1':
            editor.write('change3')
        elif args['text'] == 'change3':
            self.callbackCalled = True

    def callback_register_in_callback(self, args):
        # Order:
        #  1. test_register_callback_in_callback writes 'root', triggers the callback
        #  2. callback_register_in_callback sees 'root', registers a second callback, and writes 'change1'
        #  3.a. callback_register_in_callback sees 'change1', writes 'change2'
        #  3.b. callback_register_2 sees 'change1', writes 'change3'
        #  4. callback_register_in_callback sees 'change2' ignores
        #  5. callback_register_2 sees 'change2', ignores
        #  6. callback_register_in_callback sees 'change3' ignores
        #  7. callback_register_2 sees 'change3', sets callbackCalled = True

        if args['modificationType'] & 1 == 0:   # ignore modifications that aren't SC_MOD_INSERTTEXT
            return
        if args['text'] == 'root':
            editor.callback(lambda a: self.callback_register_2(a), [SCINTILLANOTIFICATION.MODIFIED])
            editor.write('change1')
        elif args['text'] == 'change1':
            editor.write('change2')

    def test_register_callback_in_callback(self):
        editor.callback(lambda a: self.callback_register_in_callback(a), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write('root')
        self.poll_for_callback()
        text = editor.getText()
        self.assertEqual(text, 'rootchange1change2change3')


    def callback_for_remove(self, args):
        self.callbackCalled = True

    def test_remove_all_callbacks(self):
        global globalCallbackCalled
        editor.callback(dummy_callback, [SCINTILLANOTIFICATION.MODIFIED])
        editor.clearCallbacks()
        editor.write('root')
        global_poll_for_callback(0.3)  # don't poll for too long, as we're not expecting the callback
        self.assertFalse(globalCallbackCalled)

    def test_remove_callback_via_method(self):
        global globalCallbackCalled
        editor.callback(lambda a: self.callback_for_remove(a), [SCINTILLANOTIFICATION.MODIFIED])
        editor.callback(dummy_callback, [SCINTILLANOTIFICATION.MODIFIED])
        editor.clearCallbacks(dummy_callback)
        editor.write('root')
        global_poll_for_callback(0.3)  # don't poll for too long, as we're not expecting the callback
        self.poll_for_callback()
        self.assertFalse(globalCallbackCalled)
        self.assertTrue(self.callbackCalled)   # the second callback should still have been called

    def test_remove_callback_via_notification(self):
        editor.callback(lambda a: self.callback_for_remove(a), [SCINTILLANOTIFICATION.UPDATEUI])
        editor.callback(dummy_callback, [SCINTILLANOTIFICATION.MODIFIED])
        editor.clearCallbacks([SCINTILLANOTIFICATION.MODIFIED])
        editor.write('root')
        global_poll_for_callback(0.3)  # don't poll for too long, as we're not expecting the callback
        self.poll_for_callback()
        self.assertFalse(globalCallbackCalled)
        self.assertTrue(self.callbackCalled)   # the second callback should still have been called

    def test_remove_callback_via_method_and_notification(self):
        editor.callback(lambda a: self.callback_for_remove(a), [SCINTILLANOTIFICATION.UPDATEUI])
        editor.callback(dummy_callback, [SCINTILLANOTIFICATION.MODIFIED])
        editor.clearCallbacks(dummy_callback, [SCINTILLANOTIFICATION.MODIFIED])
        editor.write('root')
        global_poll_for_callback(0.3)  # don't poll for too long, as we're not expecting the callback
        self.poll_for_callback()
        self.assertFalse(globalCallbackCalled)
        self.assertTrue(self.callbackCalled)   # the second callback should still have been called



    def test_sync_modified_callback(self):
        editor.write('start\r\n')
        editor.callbackSync(lambda a: self.callback_sync_modified(a), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write('change\r\n')
        calledDirectly = self.callbackCalled
        text = editor.getText()
        self.assertEqual(self.callbackResults['text'], 'start\r\nchange\r\n')
        self.assertEqual(self.callbackResults['modifiedText'], 'change\r\n')
        self.assertEqual(text, 'start\r\nchange\r\n')
        self.assertEqual(calledDirectly, True)

    def callback_sync_modified(self, args):
        if args['modificationType'] & 1 == 0:   # ignore modifications that aren't SC_MOD_INSERTTEXT
            return
        self.callbackResults['text'] = editor.getText()
        self.callbackResults['modifiedText'] = args['text']
        self.callbackCalled = True

    def test_sync_setsavepoint(self):
        editor.write('start\r\n')
        editor.callbackSync(lambda a: self.callback_sync_setsavepoint(a), [SCINTILLANOTIFICATION.SAVEPOINTREACHED])
        editor.setSavePoint()
        self.assertEqual(self.callbackResults['text'], 'start\r\nin change\r\n')

    def callback_sync_setsavepoint(self, args):
        editor.write('in change\r\n')
        self.callbackResults['text'] = editor.getText()

    def test_sync_disallowed_scintilla_method(self):
        editor.write('Hello world')
        editor.callbackSync(lambda a: self.callback_sync_disallowed_scintilla_method(a), [SCINTILLANOTIFICATION.SAVEPOINTREACHED])
        editor.setSavePoint()
        self.assertTrue(self.callbackCalled)


    def callback_sync_disallowed_scintilla_method(self, args):
        self.callbackCalled = True
        with self.assertRaisesRegex(RuntimeError, "not allowed in a synchronous"):
            _ = editor.findText(0, 0, editor.getLength(), 'Hello')

    def test_sync_disallowed_notepad_method(self):
        editor.write('Hello world')
        editor.callbackSync(lambda a: self.callback_sync_disallowed_notepad_method(a), [SCINTILLANOTIFICATION.SAVEPOINTREACHED])
        editor.setSavePoint()
        self.assertTrue(self.callbackCalled)

    def callback_sync_disallowed_notepad_method(self, args):
        self.callbackCalled = True
        with self.assertRaisesRegex(RuntimeError, "not allowed in a synchronous"):
            _ = notepad.createScintilla()

    def test_callback_with_replace(self):
        editor.callback(lambda a: self.callback_with_replace(a), [SCINTILLANOTIFICATION.MODIFIED])
        editor.write('hello world')
        self.poll_for_callback()
        text = editor.getText()
        self.assertEqual(text, 'hello CHEESE')

    def callback_with_replace(self, args):
        if args['modificationType'] & MODIFICATIONFLAGS.INSERTTEXT:
            editor.rereplace('wo[a-z]{3}', 'CHEESE')
            self.callbackCalled = True


    def poll_for_callback(self, timeout = 0.5, interval = 0.1):
        while (not self.callbackCalled) and (timeout > 0):
            time.sleep(interval)
            timeout -= interval


suite = unittest.TestLoader().loadTestsFromTestCase(ScintillaCallbackTestCase)
