# -*- coding: utf-8 -*-
import unittest
from Npp import *

class ReplaceUTF8TestCase(unittest.TestCase):
    def setUp(self):
        notepad.new()
        notepad.runMenuCommand("Encoding", "Encode in UTF-8")
        editor.write('Here is some text\r\nWith some umlauts XäXüXö\r\n');

    def tearDown(self):
        editor.setSavePoint()
        notepad.close()

    def test_simple_replace(self):
        editor.rereplace(r'some\s([a-z]+)', 'TEST');
        text = editor.getText()
        self.assertEqual(text, 'Here is TEST\r\nWith TEST XäXüXö\r\n');

    def test_utf8_replace(self):
        editor.rereplace(r'X[äö]', 'YY');
        text = editor.getText()
        self.assertEqual(text, 'Here is some text\r\nWith some umlauts YYXüYY\r\n');

    def test_replace_condition(self):
        editor.rereplace('(Here)|(Xä)', '(?1Cheese)(?2Y)')
        text = editor.getText()
        self.assertEqual(text, 'Cheese is some text\r\nWith some umlauts YXüXö\r\n')


    def test_replace_with_unicode(self):
        editor.rereplace('Here|With', u'XäöüY')
        text = editor.getText()
        self.assertEqual(text, 'XäöüY is some text\r\nXäöüY some umlauts XäXüXö\r\n')

    def test_replace_with_unicode_using_standard_string(self):
        editor.rereplace('Here|With', 'XäöüY')  # note the standard replacement string here
        text = editor.getText()
        self.assertEqual(text, 'XäöüY is some text\r\nXäöüY some umlauts XäXüXö\r\n')

suite = unittest.TestLoader().loadTestsFromTestCase(ReplaceUTF8TestCase)
