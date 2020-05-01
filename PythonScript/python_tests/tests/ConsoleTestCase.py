# -*- coding: utf-8 -*-
import unittest
from Npp import *

class ConsoleTestCase(unittest.TestCase):
    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_print_utf8_to_console(self):
        console.clear()
        console.write(u'\u00C4pfel')
        text = console.editor.getText()
        self.assertEqual(text, u'\u00C4pfel')
        console.clear()


suite = unittest.TestLoader().loadTestsFromTestCase(ConsoleTestCase)