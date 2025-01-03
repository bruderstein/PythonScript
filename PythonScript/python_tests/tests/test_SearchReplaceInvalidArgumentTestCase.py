# -*- coding: utf-8 -*-
import unittest
from Npp import notepad, editor

class SearchReplaceInvalidArgumentTestCase(unittest.TestCase):
    def setUp(self):
        notepad.new()

    def tearDown(self):
        notepad.close()

    def test_invalid_search_argument(self):
        for invalid_arg in ["", None]:
            for method in [editor.research, editor.search, editor.replace, editor.rereplace]:
                self.assertRaises(TypeError, method, invalid_arg, None)

    def test_invalid_replace_argument(self):
        for method in [editor.replace, editor.rereplace]:
            self.assertRaises(TypeError, method, "abc", None)


suite = unittest.TestLoader().loadTestsFromTestCase(SearchReplaceInvalidArgumentTestCase)
