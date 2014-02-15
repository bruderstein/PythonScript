import unittest
import time
from Npp import *

class ScintillaCallbackTestCase(unittest.TestCase):
    def setUp(self):
        notepad.new()

    def tearDown(self):
        notepad.clearCallbacks()
        editor.clearCallbacks()
        editor.setSavePoint()
        notepad.close()

    def updateui_callback(self, args):
        self.assertNotEqual(args['updated'], 0)
        self.assertNotEqual(args['updated'], None)
        self.called = True


    def test_callback_updateui(self):
        self.called = False
        editor.callback(lambda args: self.updateui_callback(args), [SCINTILLANOTIFICATION.UPDATEUI], )
        notepad.new()
        editor.write('test')
        time.sleep(0.2) # wait for the event to have run
        editor.clearCallbacks()
        self.assertEqual(self.called, True)
        

    def modified_callback(self, args):
        console.write('modified\n')

    def test_modified_callback(self):
        editor.callback(lambda a: self.modified_callback(a), [SCINTILLANOTIFICATION.MODIFIED])
        for _ in range(250):
            editor.write('test\n')


suite = unittest.TestLoader().loadTestsFromTestCase(ScintillaCallbackTestCase)