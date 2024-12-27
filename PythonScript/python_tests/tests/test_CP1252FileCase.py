# -*- coding: windows-1252 -*-
import unittest
from Npp import notepad, editor

class CP1252FileCase(unittest.TestCase):
    def setUp(self):
        notepad.new()

    def tearDown(self):
        editor.setSavePoint()
        notepad.close()

    def test_simple_text_output(self):
        editor.write(u'test123äöü')
        text = editor.getText()
        self.assertEqual(text, u'test123äöü')

suite = unittest.TestLoader().loadTestsFromTestCase(CP1252FileCase)
