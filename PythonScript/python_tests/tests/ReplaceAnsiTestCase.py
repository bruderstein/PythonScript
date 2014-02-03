# -*- coding: utf-8 -*-
import unittest
from Npp import *

class ReplaceAnsiTestCase(unittest.TestCase):
	def setUp(self):
		notepad.new()
		notepad.runMenuCommand("Encoding", "Encode in ANSI")
		editor.write(u'Here is some text\r\nWith some umlauts XäXüXö\r\n'.encode('windows-1252'));
		
	def tearDown(self):
		editor.setSavePoint()
		notepad.close()
		
	def test_simple_replace(self):
		editor.replace2(r'some\s([a-z]+)', 'TEST');
		text = editor.getText()
		self.assertEqual(text, u'Here is TEST\r\nWith TEST XäXüXö\r\n'.encode('windows-1252'));
		
	def test_ansi_replace(self):
		editor.replace2(u'X[äö]'.encode('windows-1252'), 'YY');
		text = editor.getText()
		self.assertEqual(text, u'Here is some text\r\nWith some umlauts YYXüYY\r\n'.encode('windows-1252'));

	def test_unicode_replace(self):
		editor.replace2(u'X[äö]', 'PP');
		text = editor.getText()
		self.assertEqual(text, u'Here is some text\r\nWith some umlauts PPXüPP\r\n'.encode('windows-1252'));	

suite = unittest.TestLoader().loadTestsFromTestCase(ReplaceAnsiTestCase)
