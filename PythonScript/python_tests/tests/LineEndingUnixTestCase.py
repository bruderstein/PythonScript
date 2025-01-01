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
        editor.write('test123äöüß')
        text = editor.getText()
        self.assertEqual(text, 'test123äöüß')

suite = unittest.TestLoader().loadTestsFromTestCase(LineEndingUnixTestCase)
