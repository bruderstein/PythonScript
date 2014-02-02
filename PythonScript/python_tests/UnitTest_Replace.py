# -*- coding: utf-8 -*-
import unittest
from Npp import *

class ReplaceTestCase(unittest.TestCase):
	def setUp(self):
		notepad.new()
		notepad.setEncoding(BUFFERENCODING.UTF8)
		editor.write('Here is some text\r\nWith some umlauts XäXüXö\r\n');
		
	def tearDown(self):
		editor.setSavePoint()
		notepad.close()
		
	def test_simple_replace(self):
		editor.replace2(r'some\s([a-z]+)', 'TEST');
		text = editor.getText()
		self.assertEqual(text, 'Here is TEST\r\nWith TEST XäXüXö\r\n');
		
	def test_utf8_replace(self):
		editor.replace2(r'X[äö]', 'YY');
		text = editor.getText()
		self.assertEqual(text, 'Here is some text\r\nWith some umlauts YYXüYY\r\n');
		

suite = unittest.TestLoader().loadTestsFromTestCase(ReplaceTestCase)
