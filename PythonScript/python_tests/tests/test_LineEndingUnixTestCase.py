# -*- coding: utf-8 -*-
import unittest
from Npp import notepad, editor

class LineEndingUnixTestCase(unittest.TestCase):
    def setUp(self):
        notepad.new()

    def tearDown(self):
        editor.setSavePoint()
        notepad.close()

    def test_simple_text_output(self):
        editor.write(u'test123äöü')
        text = editor.getText()
        self.assertEqual(text, u'test123äöü')

suite = unittest.TestLoader().loadTestsFromTestCase(LineEndingUnixTestCase)
