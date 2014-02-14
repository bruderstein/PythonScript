import unittest
import time
from Npp import *

class ScintillaCallbackTestCase(unittest.TestCase):
    def setUp(self):
        pass

    def tearDown(self):
        editor.clearCallbacks()

    def updateui_callback(self, args):
        self.assertNotEqual(args['updated'], 0)
        self.assertNotEqual(args['updated'], None)
        self.called = True

    def test_callback_updateui(self):
        self.called = False
        editor.callback(lambda args: self.updateui_callback(args), [SCINTILLANOTIFICATION.UPDATEUI])
        notepad.new()
        editor.write('test')
        time.sleep(0.2) # wait for the event to have run
        editor.clearCallbacks()
        self.assertEqual(self.called, True)
        



suite = unittest.TestLoader().loadTestsFromTestCase(ScintillaCallbackTestCase)