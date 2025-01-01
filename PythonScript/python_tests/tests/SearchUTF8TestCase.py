# -*- coding: utf-8 -*-
import unittest
import re
from Npp import notepad, editor

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
        self.assertEqual((m.start(), m.end(), m.group()), correct_groups[counter])


    def test_plain_search(self):
        editor.search('bc123', lambda m: self.check_plain_result(m))
        self.assertEqual(counter, 2)

    def check_regex_result(self, m):
        global counter
        counter += 1
        correct_groups = { 1 : (0, 6, 'abc123'), 2 : (7, 14, 'def5432'), 3 : (15, 19, 'gh98'),
                           4 : (21, 28, 'äbc123'), 5: (29, 37, 'üef5432'), 6 : (38, 43, 'öh98')
                         }
        self.assertEqual((m.start(), m.end(), m.group()), correct_groups[counter])

    def test_regex_search(self):
        editor.research('([äüöa-z]+)([0-9]+)', lambda m: self.check_regex_result(m))
        self.assertEqual(counter, 6)

    def test_plain_search_wrong_arg(self):
        with self.assertRaisesRegexp(TypeError, "callable"):
            editor.search('bc123', 'this is a string, i am doing it wrong')

    def test_regex_search_wrong_arg(self):
        with self.assertRaisesRegexp(TypeError, "callable"):
            editor.research('bc123', 'this is a string, i am doing it wrong')


    def set_called(self, m):
        self.called = True

    def test_search_casesensitive(self):
        self.called = False
        editor.research('BC123', self.set_called)
        self.assertEqual(self.called, False)

    def test_search_flags_caseinsensitive(self):
        self.called = False
        editor.research('BC123', self.set_called, re.IGNORECASE)
        self.assertEqual(self.called, True)

    def check_search_start(self, m):
        correct_match = (23, 28, 'bc123')
        self.called = True
        self.assertEqual((m.start(), m.end(), m.group()), correct_match)

    def test_search_start(self):
        self.called = False
        editor.research('([a-c]{2})123', lambda m: self.check_search_start(m), 0, 3)
        self.assertEqual(self.called, True)

    def test_search_start_end(self):
        self.called = False
        editor.research('([a-c]{2})123', self.set_called, 0, 3, 27)
        self.assertEqual(self.called, False)

    def check_search_start_end_count(self, m):
        self.call_count += 1
        correct_calls = { 1 : (7, 14, 'def5432'),
                          2 : (15, 19, 'gh98'),
                          3 : (21, 28, 'äbc123')
                         }
        self.assertEqual((m.start(), m.end(), m.group()), correct_calls[self.call_count])


    def test_search_start_end_count(self):
        # Search from 3 to 40, so we miss the first and the last entries, and limit it to a maximum of 3 entries
        # There would otherwise be four
        self.call_count = 0
        editor.research('([a-zäüö]+)([0-9]+)', lambda m: self.check_search_start_end_count(m), 0, 3, 40, 3)
        self.assertEqual(self.call_count, 3)

suite = unittest.TestLoader().loadTestsFromTestCase(SearchUTF8TestCase)
