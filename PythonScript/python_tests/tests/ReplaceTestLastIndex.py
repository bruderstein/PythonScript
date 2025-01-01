# -*- coding: utf-8 -*-
import unittest
from Npp import notepad, editor


class ReplaceLastIndex(unittest.TestCase):
    def setUp(self):
        global counter
        counter = 0
        notepad.new()
        notepad.runMenuCommand("Encoding", "Encode in UTF-8")

    def tearDown(self):
        editor.setSavePoint()
        notepad.close()

    def test_lastindex_normal(self):
        editor.write('abcX123')
        editor.rereplace('(abc)(X)([0-9]+)', lambda m: self.assertEqual(m.lastindex, 3))

    def test_lastindex_empty(self):
        editor.write('abcX')
        editor.rereplace('(abc)(X)([0-9]*)', lambda m: self.assertEqual(m.lastindex, 3))

    def test_lastindex_notmatched(self):
        editor.write('abcX')
        editor.rereplace('(abc)(X)([0-9]+)?', lambda m: self.assertEqual(m.lastindex, 2))

suite = unittest.TestLoader().loadTestsFromTestCase(ReplaceLastIndex)
