# -*- coding: utf-8 -*-
import unittest
from Npp import notepad, editor

counter = 0

def group2_with_counter(m):
    global counter
    counter += 1
    replacement = '{}{}'.format(m.group(2), counter)
    return replacement

def group1_with_counter(m):
    global counter
    counter += 1
    replacement =  '{}{}'.format(m.group(1), counter)
    return replacement






class ReplaceUTF8PythonFunctionTestCase(unittest.TestCase):
    def setUp(self):
        global counter
        counter = 0
        notepad.new()
        notepad.runMenuCommand("Encoding", "Encode in UTF-8")
        editor.write('abc123 def5432 gh98\r\näbc123 üef5432 öh98\r\n')

    def tearDown(self):
        editor.setSavePoint()
        notepad.close()

    def test_replace_function(self):
        editor.rereplace(r'([a-z]+)([0-9]+)', group2_with_counter)
        text = editor.getText()
        self.assertEqual(text, '1231 54322 983\r\nä1234 ü54325 ö986\r\n')

    def test_utf8_replace_function(self):
        editor.rereplace(r'([a-zäöü]+)([0-9]+)', group1_with_counter)
        text = editor.getText()
        self.assertEqual(text, 'abc1 def2 gh3\r\näbc4 üef5 öh6\r\n')

    def groups_check(self, m):
        global counter
        counter += 1
        groups_data_correct = { 1 : ('abc', '123'),
                            2 : ('def', '5432'),
                            3 : ('gh', '98'),
                            4 : ('äbc', '123'),
                            5 : ('üef', '5432'),
                            6 : ('öh', '98') }
        self.assertEqual(m.groups(), groups_data_correct[counter])
        return counter

    def test_groups(self):
        editor.rereplace(r'([a-zäöü]+)([0-9]+)', lambda m: self.groups_check(m))
        text = editor.getText()
        self.assertEqual(text, '1 2 3\r\n4 5 6\r\n')


    def test_groups_with_named_groups(self):
        editor.rereplace(r'(?<letters>[a-zäöü]+)(?<numbers>[0-9]+)', lambda m: self.groups_check(m))
        text = editor.getText()
        self.assertEqual(text, '1 2 3\r\n4 5 6\r\n')

    def test_named_groups(self):
        editor.rereplace(r'(?<letters>[a-zäöü]+)(?<numbers>[0-9]+)', lambda m: m.group('letters'))
        text = editor.getText()
        self.assertEqual(text, 'abc def gh\r\näbc üef öh\r\n')

    def test_named_groups_2(self):
        editor.rereplace(r'(?<letters>[a-zäöü]+)(?<numbers>[0-9]+)', lambda m: m.group('numbers'))
        text = editor.getText()
        self.assertEqual(text, '123 5432 98\r\n123 5432 98\r\n')


    def group_tuples_check(self, m):
        global counter
        counter += 1
        groups_data_correct = { 1 : ('123', 'abc', '123'),
                            2 : ('5432', 'def', '5432'),
                            3 : ('98', 'gh', '98'),
                            4 : ('123', 'äbc', '123'),
                            5 : ('5432', 'üef', '5432'),
                            6 : ('98', 'öh', '98') }
        self.assertEqual(m.group(2, 'letters', 'numbers'), groups_data_correct[counter])
        return counter

    def test_group_tuples(self):
        editor.rereplace(r'(?<letters>[a-zäöü]+)(?<numbers>[0-9]+)', lambda m: self.group_tuples_check(m))
        text = editor.getText()
        self.assertEqual(text, '1 2 3\r\n4 5 6\r\n')

    def test_expand(self):
        editor.rereplace(r'([a-z]+)([0-9]+)', lambda m: m.expand(r'N\2L\1'))
        text = editor.getText()
        self.assertEqual(text, 'N123Labc N5432Ldef N98Lgh\r\näN123Lbc üN5432Lef öN98Lh\r\n')


    def start_check(self, m):
        global counter
        counter += 1
        start1_data_correct = [-1, 0, 7, 15, 23, 31, 40]
        start2_data_correct = [-1, 3, 10, 17, 25, 33, 41]
        self.assertEqual(m.start(1), start1_data_correct[counter])
        self.assertEqual(m.start(2), start2_data_correct[counter])
        self.assertEqual(m.start(), start1_data_correct[counter])
        return counter

    def test_start(self):
        editor.rereplace(r'([a-z]+)([0-9]+)', lambda m: self.start_check(m))
        text = editor.getText()
        self.assertEqual(text, '1 2 3\r\nä4 ü5 ö6\r\n')

    def end_check(self, m):
        global counter
        counter += 1
        end1_data_correct = [-1, 3, 10, 17, 25, 33, 41]
        end2_data_correct = [-1, 6, 14, 19, 28, 37, 43]
        self.assertEqual(m.end(1), end1_data_correct[counter])
        self.assertEqual(m.end(2), end2_data_correct[counter])
        self.assertEqual(m.end(), end2_data_correct[counter])
        return counter

    def test_end(self):
        editor.rereplace(r'([a-z]+)([0-9]+)', lambda m: self.end_check(m))
        text = editor.getText()
        self.assertEqual(text, '1 2 3\r\nä4 ü5 ö6\r\n')

    def span_check(self, m):
        global counter
        counter += 1
        start1_data_correct = [-1, 0, 7, 15, 23, 31, 40]
        start2_data_correct = [-1, 3, 10, 17, 25, 33, 41]
        end1_data_correct = [-1, 3, 10, 17, 25, 33, 41]
        end2_data_correct = [-1, 6, 14, 19, 28, 37, 43]
        self.assertEqual(m.span(1), (start1_data_correct[counter], end1_data_correct[counter]))
        self.assertEqual(m.span(2), (start2_data_correct[counter], end2_data_correct[counter]))
        self.assertEqual(m.span(), (start1_data_correct[counter], end2_data_correct[counter]))
        return counter

    def test_span(self):
        editor.rereplace(r'([a-z]+)([0-9]+)', lambda m: self.span_check(m))
        text = editor.getText()
        self.assertEqual(text, '1 2 3\r\nä4 ü5 ö6\r\n')

    def test_not_supported_groupdict(self):
        with self.assertRaisesRegexp(RuntimeError,  r"not supported under Notepad\+\+"):
            editor.rereplace(r'([a-z]+)', lambda m: m.groupdict()[0])


    def test_not_supported_re(self):
        with self.assertRaisesRegexp(RuntimeError,  r"not supported under Notepad\+\+"):
            editor.rereplace(r'([a-z]+)', lambda m: m.re)

    def test_not_supported_pos(self):
        with self.assertRaisesRegexp(RuntimeError,  r"not supported under Notepad\+\+"):
            editor.rereplace(r'([a-z]+)', lambda m: m.pos)

    def test_not_supported_endpos(self):
        with self.assertRaisesRegexp(RuntimeError,  r"not supported under Notepad\+\+"):
            editor.rereplace(r'([a-z]+)', lambda m: m.endpos)


    def test_group_index_invalid(self):
        with self.assertRaisesRegexp(IndexError, "no such group"):
            editor.rereplace(r'([a-z]+)([0-9]+)', lambda m: m.group(3))

    def test_group_name_invalid(self):
        with self.assertRaisesRegexp(IndexError, "no such group"):
            editor.rereplace(r'(?<letters>[a-z]+)(?<numbers>[0-9]+)', lambda m: m.group('somethingelse'))

suite = unittest.TestLoader().loadTestsFromTestCase(ReplaceUTF8PythonFunctionTestCase)
