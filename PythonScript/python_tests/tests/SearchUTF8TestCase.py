# -*- coding: utf-8 -*-
import unittest
from Npp import *

counter = 0
    

class SearchUTF8TestCase(unittest.TestCase):
    def setUp(self):
        global counter
        counter = 0
        notepad.new()
        notepad.runMenuCommand("Encoding", "Encode in UTF-8")
        editor.write('abc123 def5432 gh98\r\näbc123 üef5432 öh98\r\n')
        
    def tearDown(self):
        editor.setSavePoint()
        notepad.close()
        

    def check_plain_result(self, m):
        global counter
        counter += 1
        correct_groups = { 1 : (1, 6, 'bc123'), 2 : (23, 28, 'bc123') }
        self.assertEquals((m.start(), m.end(), m.group()), correct_groups[counter])


    def test_plain_search(self):
        editor.search('bc123', lambda m: self.check_plain_result(m))
        self.assertEquals(counter, 2)

    def check_regex_result(self, m):
        global counter
        counter += 1
        correct_groups = { 1 : (0, 6, 'abc123'), 2 : (7, 14, 'def5432'), 3 : (15, 19, 'gh98'),
                           4 : (21, 28, 'äbc123'), 5: (29, 37, 'üef5432'), 6 : (38, 43, 'öh98')
                         }
        self.assertEquals((m.start(), m.end(), m.group()), correct_groups[counter])

    def test_regex_search(self):
        editor.research('([äüöa-z]+)([0-9]+)', lambda m: self.check_regex_result(m))
        self.assertEquals(counter, 6)

    def test_plain_search_wrong_arg(self):
        with self.assertRaisesRegexp(TypeError, "callable"):
            editor.search('bc123', 'this is a string, i am doing it wrong')

    def test_regex_search_wrong_arg(self):
        with self.assertRaisesRegexp(TypeError, "callable"):
            editor.research('bc123', 'this is a string, i am doing it wrong')
        

suite = unittest.TestLoader().loadTestsFromTestCase(SearchUTF8TestCase)
