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






class ReplaceAnsiPythonFunctionTestCase(unittest.TestCase):
    def setUp(self):
        global counter
        counter = 0
        notepad.new()
        notepad.runMenuCommand("Encoding", "Convert to ANSI")
        editor.write(u'abc123 def5432 gh98\r\näbc123 üef5432 öh98\r\n'.encode('windows-1252'))

    def tearDown(self):
        editor.setSavePoint()
        notepad.close()

    def test_replace_function(self):
        editor.rereplace(r'([a-z]+)([0-9]+)'.encode('windows-1252'), group2_with_counter)
        text = editor.getText()
        self.assertEqual(text, u'1231 54322 983\r\nä1234 ü54325 ö986\r\n'.encode('windows-1252'))

    def test_unicode_replace_function(self):
        editor.rereplace(ur'([a-zäöü]+)([0-9]+)', group1_with_counter)
        text = editor.getText()
        self.assertEqual(text, u'abc1 def2 gh3\r\näbc4 üef5 öh6\r\n'.encode('windows-1252'))

    def groups_check(self, m):
        global counter
        counter += 1
        groups_data_correct = { 1 : ('abc', '123'),
                            2 : ('def', '5432'),
                            3 : ('gh', '98'),
                            4 : (u'äbc'.encode('windows-1252'), '123'),
                            5 : (u'üef'.encode('windows-1252'), '5432'),
                            6 : (u'öh'.encode('windows-1252'), '98') }
        self.assertEqual(m.groups(), groups_data_correct[counter])
        return counter

    def test_groups(self):
        editor.rereplace(ur'([a-zäöü]+)([0-9]+)', lambda m: self.groups_check(m))
        text = editor.getText()
        self.assertEqual(text, '1 2 3\r\n4 5 6\r\n')


    def test_groups_with_named_groups(self):
        editor.rereplace(ur'(?<letters>[a-zäöü]+)(?<numbers>[0-9]+)', lambda m: self.groups_check(m))
        text = editor.getText()
        self.assertEqual(text, '1 2 3\r\n4 5 6\r\n')

    def test_named_groups(self):
        editor.rereplace(ur'(?<letters>[a-zäöü]+)(?<numbers>[0-9]+)', lambda m: m.group('letters'))
        text = editor.getText()
        self.assertEqual(text, u'abc def gh\r\näbc üef öh\r\n'.encode('windows-1252'))

    def test_named_groups_2(self):
        editor.rereplace(ur'(?<letters>[a-zäöü]+)(?<numbers>[0-9]+)', lambda m: m.group('numbers'))
        text = editor.getText()
        self.assertEqual(text, '123 5432 98\r\n123 5432 98\r\n')


    def group_tuples_check(self, m):
        global counter
        counter += 1
        groups_data_correct = { 1 : ('123', 'abc', '123'),
                            2 : ('5432', 'def', '5432'),
                            3 : ('98', 'gh', '98'),
                            4 : ('123', '\xe4bc', '123'),
                            5 : ('5432', u'üef'.encode('windows-1252'), '5432'),
                            6 : ('98', u'öh'.encode('windows-1252'), '98') }
        self.assertEqual(m.group(2, 'letters', 'numbers'), groups_data_correct[counter])
        return counter

    def test_group_tuples(self):
        editor.rereplace(ur'(?<letters>[a-zäöü]+)(?<numbers>[0-9]+)', lambda m: self.group_tuples_check(m))
        text = editor.getText()
        self.assertEqual(text, '1 2 3\r\n4 5 6\r\n')


suite = unittest.TestLoader().loadTestsFromTestCase(ReplaceAnsiPythonFunctionTestCase)
