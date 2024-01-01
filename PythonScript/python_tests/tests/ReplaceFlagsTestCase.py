# -*- coding: utf-8 -*-
import unittest
import re
from Npp import *

class ReplaceFlagsTestCase(unittest.TestCase):
    def setUp(self):
        notepad.new()
        notepad.runMenuCommand("Encoding", "Encode in UTF-8")
        editor.write(u'Abc123DEF4567 ghi8910\r\nAbc123\r\n')

    def tearDown(self):
        editor.setSavePoint()
        notepad.close()

    def test_replace_is_case_sensitive(self):
        editor.rereplace(r'([A-Z]{3})', 'TEST')
        text = editor.getText()
        self.assertEqual(text, u'Abc123TEST4567 ghi8910\r\nAbc123\r\n')

    def test_replace_case_insensitive(self):
        editor.rereplace(r'([A-Z]{3})', 'TEST', re.IGNORECASE)
        text = editor.getText()
        self.assertEqual(text, u'TEST123TEST4567 TEST8910\r\nTEST123\r\n')

    def test_replace_anchor_is_whole_file(self):
        editor.rereplace(r'^Abc', 'TEST', editor.WHOLEDOC)
        text = editor.getText()
        self.assertEqual(text, u'TEST123DEF4567 ghi8910\r\nAbc123\r\n')

    def test_replace_anchor_is_default_per_line(self):
        editor.rereplace(r'^Abc', 'TEST')
        text = editor.getText()
        self.assertEqual(text, u'TEST123DEF4567 ghi8910\r\nTEST123\r\n')


    def test_replace_literal_is_case_sensitive(self):
        editor.replace('ABC', 'TEST')
        text = editor.getText()
        self.assertEqual(text, u'Abc123DEF4567 ghi8910\r\nAbc123\r\n')

    def test_replace_literal_case_insensitive(self):
        editor.replace('ABC', 'TEST', re.IGNORECASE)
        text = editor.getText()
        self.assertEqual(text, u'TEST123DEF4567 ghi8910\r\nTEST123\r\n')

suite = unittest.TestLoader().loadTestsFromTestCase(ReplaceFlagsTestCase)
