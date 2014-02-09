# -*- coding: utf-8 -*-
import unittest
from Npp import *

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
        editor.replace2(r'([a-z]+)([0-9]+)', group2_with_counter)
        text = editor.getText()
        self.assertEqual(text, '1231 54322 983\r\nä1234 ü54325 ö986\r\n')
        
    def test_utf8_replace_function(self):
        editor.replace2(r'([a-zäöü]+)([0-9]+)', group1_with_counter)
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
        editor.replace2(r'([a-zäöü]+)([0-9]+)', lambda m: self.groups_check(m))
        text = editor.getText()
        self.assertEqual(text, '1 2 3\r\n4 5 6\r\n')


    def test_groups_with_named_groups(self):
        editor.replace2(r'(?<letters>[a-zäöü]+)(?<numbers>[0-9]+)', lambda m: self.groups_check(m))
        text = editor.getText()
        self.assertEqual(text, '1 2 3\r\n4 5 6\r\n')

    def test_named_groups(self):
        editor.replace2(r'(?<letters>[a-zäöü]+)(?<numbers>[0-9]+)', lambda m: m.group('letters'))
        text = editor.getText()
        self.assertEqual(text, 'abc def gh\r\näbc üef öh\r\n')

    def test_named_groups_2(self):
        editor.replace2(r'(?<letters>[a-zäöü]+)(?<numbers>[0-9]+)', lambda m: m.group('numbers'))
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
        editor.replace2(r'(?<letters>[a-zäöü]+)(?<numbers>[0-9]+)', lambda m: self.group_tuples_check(m))
        text = editor.getText()
        self.assertEqual(text, '1 2 3\r\n4 5 6\r\n')


suite = unittest.TestLoader().loadTestsFromTestCase(ReplaceUTF8PythonFunctionTestCase)
