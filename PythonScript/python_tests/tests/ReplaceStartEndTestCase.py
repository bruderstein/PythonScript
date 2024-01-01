# -*- coding: utf-8 -*-
import unittest
import re
from Npp import *

class ReplaceStartEndTestCase(unittest.TestCase):
    def setUp(self):
        notepad.new()
        notepad.runMenuCommand("Encoding", "UTF-8")
        editor.write(u'Abc123DEF4567 ghi8910\r\nAbc123\r\n')

    def tearDown(self):
        editor.setSavePoint()
        notepad.close()

    def test_replace_start_later(self):
        editor.rereplace(r'([A-Z]{3})', 'TEST', re.IGNORECASE, 3)
        text = editor.getText()
        self.assertEqual(text, u'Abc123TEST4567 TEST8910\r\nTEST123\r\n')

    def test_replace_start_later_end_sooner(self):
        editor.rereplace(r'([A-Z]{3})', 'TEST', re.IGNORECASE, 3, 18)
        text = editor.getText()
        self.assertEqual(text, u'Abc123TEST4567 TEST8910\r\nAbc123\r\n')


    def test_replace_literal_start_later(self):
        editor.rereplace(r'Abc', 'TEST', re.IGNORECASE, 3)
        text = editor.getText()
        self.assertEqual(text, u'Abc123DEF4567 ghi8910\r\nTEST123\r\n')

    def test_replace_literal_start_later_end_sooner_1(self):
        editor.replace('ABC', 'TEST', re.IGNORECASE, 3, 18)
        text = editor.getText()
        self.assertEqual(text, u'Abc123DEF4567 ghi8910\r\nAbc123\r\n')

    def test_replace_literal_start_later_end_sooner_2(self):
        editor.replace('123', 'TEST', re.IGNORECASE, 3, 18)
        text = editor.getText()
        self.assertEqual(text, u'AbcTESTDEF4567 ghi8910\r\nAbc123\r\n')

    def test_replace_literal_start_later_end_sooner_2(self):
        editor.replace('123', 'TEST', re.IGNORECASE, 4, 18)
        text = editor.getText()
        self.assertEqual(text, u'Abc123DEF4567 ghi8910\r\nAbc123\r\n')

suite = unittest.TestLoader().loadTestsFromTestCase(ReplaceStartEndTestCase)
