import unittest
from Npp import *

class NotepadTestCase(unittest.TestCase):
    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_setEncoding(self):
        notepad.new()
        notepad.setEncoding(BUFFERENCODING.UTF8)
        encoding = notepad.getEncoding()
        self.assertEqual(encoding, BUFFERENCODING.UTF8)
        notepad.setEncoding(BUFFERENCODING.ANSI)
        encoding = notepad.getEncoding()
        self.assertEqual(encoding, BUFFERENCODING.ANSI)
        notepad.close()


suite = unittest.TestLoader().loadTestsFromTestCase(NotepadTestCase)