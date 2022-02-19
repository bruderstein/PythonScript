# -*- coding: utf-8 -*-
import unittest
import re
from Npp import *

class ReplaceStartEndTestCase(unittest.TestCase):
    def setUp(self):
        notepad.new()
        notepad.runMenuCommand("Encoding", "Encode in UTF-8")
        editor.write(u'Abc123DEF4567 ghi8910\r\nAbc123\r\n')

    def tearDown(self):
        editor.setSavePoint()
        notepad.close()

    def test_replace_limit_3(self):
        editor.rereplace(r'([A-Z]{3})', 'TEST', re.IGNORECASE, 0, 0, 3)
        text = editor.getText()
        self.assertEqual(text, u'TEST123TEST4567 TEST8910\r\nAbc123\r\n')

    def test_replace_limit_2(self):
        editor.rereplace(r'([A-Z]{3})', 'TEST', re.IGNORECASE, 0, 0, 2)
        text = editor.getText()
        self.assertEqual(text, u'TEST123TEST4567 ghi8910\r\nAbc123\r\n')


    def test_replace_literal_count_1(self):
        editor.replace(r'Abc', 'TEST', re.IGNORECASE, 0, 0, 1)
        text = editor.getText()
        self.assertEqual(text, u'TEST123DEF4567 ghi8910\r\nAbc123\r\n')

suite = unittest.TestLoader().loadTestsFromTestCase(ReplaceStartEndTestCase)
