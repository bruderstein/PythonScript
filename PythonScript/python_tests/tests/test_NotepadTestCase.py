# -*- coding: utf-8 -*-

import unittest
from Npp import notepad, BUFFERENCODING, LANGTYPE

class NotepadTestCase(unittest.TestCase):
    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_setEncoding(self):
        notepad.new()
        notepad.setEncoding(BUFFERENCODING.UTF8)
        encoding = notepad.getEncoding()
        self.assertEqual(encoding, BUFFERENCODING.UTF8)
        notepad.setEncoding(BUFFERENCODING.ANSI)
        encoding = notepad.getEncoding()
        self.assertEqual(encoding, BUFFERENCODING.ANSI)
        notepad.close()

    def test_setLangType(self):
        notepad.new()
        buffer1 = notepad.getCurrentBufferID()
        notepad.setLangType(LANGTYPE.C)

        notepad.new()
        buffer2 = notepad.getCurrentBufferID()
        notepad.setLangType(LANGTYPE.PHP)

        notepad.activateBufferID(buffer1)
        newBuffer1Lang = notepad.getLangType()
        notepad.activateBufferID(buffer2)
        newBuffer2Lang = notepad.getLangType()

        notepad.close()
        notepad.activateBufferID(buffer1)
        notepad.close()

        self.assertEqual(newBuffer1Lang, LANGTYPE.C)
        self.assertEqual(newBuffer2Lang, LANGTYPE.PHP)



suite = unittest.TestLoader().loadTestsFromTestCase(NotepadTestCase)
