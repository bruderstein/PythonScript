# -*- coding: utf-8 -*-
import unittest
from Npp import notepad, editor

class ReplaceAnsiTestCase(unittest.TestCase):
    def setUp(self):
        notepad.new()
        notepad.runMenuCommand("Encoding", "ANSI")
        editor.write(u'Here is some text\r\nWith some umlauts XäXüXö\r\n'.encode('windows-1252'));

    def tearDown(self):
        editor.setSavePoint()
        notepad.close()

    def test_simple_replace(self):
        editor.rereplace(r'some\s([a-z]+)', 'TEST');
        text = editor.getText()
        self.assertEqual(text, u'Here is TEST\r\nWith TEST XäXüXö\r\n'.encode('windows-1252'));

    def test_ansi_replace(self):
        editor.rereplace(u'X[äö]'.encode('windows-1252'), 'YY');
        text = editor.getText()
        self.assertEqual(text, u'Here is some text\r\nWith some umlauts YYXüYY\r\n'.encode('windows-1252'));

    def test_unicode_replace(self):
        editor.rereplace(u'X[äö]', 'PP');
        text = editor.getText()
        self.assertEqual(text, u'Here is some text\r\nWith some umlauts PPXüPP\r\n'.encode('windows-1252'));

    def test_replace_with_unicode(self):
        editor.rereplace('Here|With', u'XäöüY')
        text = editor.getText()
        self.assertEqual(text, u'XäöüY is some text\r\nXäöüY some umlauts XäXüXö\r\n'.encode('windows-1252'))

suite = unittest.TestLoader().loadTestsFromTestCase(ReplaceAnsiTestCase)
