# -*- coding: utf-8 -*-
import unittest
from Npp import *
import re

class ReplacePlainTestCase(unittest.TestCase):
	def setUp(self):
		notepad.new()
		notepad.runMenuCommand("Encoding", "Encode in UTF-8")
		editor.write('Some text with ([abc]+) embedded regex\r\n');
		
	def tearDown(self):
		editor.setSavePoint()
		notepad.close()
		
	def test_plain_replace(self):
		editor.replace(r'([abc]+)', 'TEST');
		text = editor.getText()
		self.assertEqual(text, 'Some text with TEST embedded regex\r\n');


suite = unittest.TestLoader().loadTestsFromTestCase(ReplacePlainTestCase)
