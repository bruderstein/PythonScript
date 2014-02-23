# -*- coding: utf-8 -*-

import unittest
import os
import shlex
import tempfile
from Npp import *

class NotepadTestCase(unittest.TestCase):
    def setUp(self):
        self.files_to_delete = []

    def tearDown(self):
        for file in self.files_to_delete:
            os.remove(file)

    def check_file_contents(self, filename, expectedContents):
        f = open(filename, "rb")
        actualContents = f.read()
        f.close()
        self.assertEqual(actualContents, expectedContents)

    def get_temp_filename(self):
        (file, filename) = tempfile.mkstemp()
        os.close(file)
        self.files_to_delete.append(filename)
        return filename

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

    def test_save(self):
        filename = self.get_temp_filename()
        notepad.new(filename)
        # setEncoding doesn't work when the file has a name
        notepad.runMenuCommand('Encoding', 'Encode in ANSI')
        editor.write('Hello world - save')
        notepad.save()
        notepad.close()
        self.check_file_contents(filename, 'Hello world - save')

    def test_saveAs(self):
        notepad.new()
        notepad.setEncoding(BUFFERENCODING.ANSI)
        editor.write('Hello world - saveAs')
        filename = self.get_temp_filename()
        notepad.saveAs(filename)
        notepad.close()
        self.check_file_contents(filename, 'Hello world - saveAs')


    def test_saveAsCopy(self):
        notepad.new()
        notepad.setEncoding(BUFFERENCODING.ANSI)
        editor.write('Hello world - saveAsCopy')
        realFilename = self.get_temp_filename()
        copyFilename = self.get_temp_filename()
        notepad.saveAs(realFilename)
        notepad.saveAsCopy(copyFilename)
        editor.write('-OriginalChanged')
        notepad.save()
        notepad.close()
        self.check_file_contents(realFilename, 'Hello world - saveAsCopy-OriginalChanged')
        self.check_file_contents(copyFilename, 'Hello world - saveAsCopy')

    def test_open(self):
        filename = self.get_temp_filename()
        f = open(filename, "w")
        f.write('Test - open')
        f.close()
        notepad.open(filename)
        text = editor.getText()
        notepad.close()

        self.assertEqual(text, 'Test - open')
        

    def test_getCurrentView(self):
        notepad.new()
        editor.write('View 1')
        view1BufferID = notepad.getCurrentBufferID()
        notepad.new()
        view2BufferID = notepad.getCurrentBufferID()
        editor.write('View 2')
        notepad.runMenuCommand('Move/Clone current document', 'Move to other view')
        inView2 = notepad.getCurrentView()
        view2Text = editor.getText()
        notepad.activateBufferID(view1BufferID)
        inView1 = notepad.getCurrentView()
        view1Text = editor.getText()

        # tidy up - should really be it's own case with a tearDown - TODO
        notepad.activateBufferID(view2BufferID)
        editor.setSavePoint()
        notepad.close()
        notepad.activateBufferID(view1BufferID)
        editor.setSavePoint()
        notepad.close()
        
        self.assertEqual(inView2, 1)
        self.assertEqual(inView1, 0)
        self.assertEqual(view1Text, 'View 1')
        self.assertEqual(view2Text, 'View 2')


    def test_getCurrentLang(self):
        notepad.new()
        notepad.setLangType(LANGTYPE.PHP)
        lang = notepad.getCurrentLang()
        notepad.close()
        self.assertEqual(lang, LANGTYPE.PHP)

    def test_getFiles(self):
        # Grab the list of files already open
        beforeFiles = notepad.getFiles()
        notepad.new()
        editor.write('File 1')
        file1 = self.get_temp_filename()
        notepad.saveAs(file1)
        bufferID1 = notepad.getCurrentBufferID()
        index1 = notepad.getCurrentDocIndex(0)
        notepad.new()
        editor.write('File 2')
        file2 = self.get_temp_filename()
        notepad.saveAs(file2)
        bufferID2 = notepad.getCurrentBufferID()
        index2 = notepad.getCurrentDocIndex(0)

        files = notepad.getFiles()
        # remove the files that were open before we started
        for entry in beforeFiles:
            files.remove(entry)

        # python temp files are all lowercase, so make the filenames from getFiles lowercase too
        lowercaseFiles = []
        for entry in files:
            lowercaseFiles.append((entry[0].lower(), entry[1], entry[2], entry[3]))

        # clean up
        notepad.activateBufferID(bufferID1)
        editor.setSavePoint()
        notepad.close()
        notepad.activateBufferID(bufferID2)
        editor.setSavePoint()
        notepad.close()

        self.assertIn((file1, bufferID1, index1, 0), lowercaseFiles)
        self.assertIn((file2, bufferID2, index2, 0), lowercaseFiles)
        self.assertEqual(len(files), 2)


    def test_saveLoadSession(self):
        # Create and open two files
        file1 = self.get_temp_filename()
        file2 = self.get_temp_filename()
        notepad.open(file1)
        editor.write('File 1 session')
        notepad.save()
        notepad.open(file2)
        editor.write('File 2 session')
        notepad.save()
        
        # Save the session
        sessionFile = self.get_temp_filename()
        notepad.saveSession(sessionFile, [file1, file2])

        # Close the files 
        notepad.activateFile(file1)
        notepad.close()
        notepad.activateFile(file2)
        notepad.close()
        
        # Load the session back
        notepad.loadSession(sessionFile)

        # Check the files are there again
        notepad.activateFile(file1)
        file1Content = editor.getText()
        notepad.close()
        notepad.activateFile(file2)
        file2Content = editor.getText()
        notepad.close()

        self.assertEqual(file1Content, 'File 1 session')
        self.assertEqual(file2Content, 'File 2 session')

    def test_getSessionFiles(self):
        # Create and open two files
        file1 = self.get_temp_filename()
        file2 = self.get_temp_filename()
        notepad.open(file1)
        editor.write('File 1 session')
        notepad.save()
        notepad.open(file2)
        editor.write('File 2 session')
        notepad.save()
        
        # Save the session
        sessionFile = self.get_temp_filename()
        notepad.saveSession(sessionFile, [file1, file2])
        sessionFiles = notepad.getSessionFiles(sessionFile)
        
        notepad.activateFile(file1)
        notepad.close()
        notepad.activateFile(file2)
        notepad.close()

        self.assertEqual(sessionFiles, [file1, file2])

    def test_saveCurrentSession(self):
        
        # Create and open two files
        file1 = self.get_temp_filename()
        file2 = self.get_temp_filename()
        notepad.open(file1)
        editor.write('File 1 session')
        notepad.save()
        notepad.open(file2)
        editor.write('File 2 session')
        notepad.save()

        sessionFile = self.get_temp_filename()
        notepad.saveCurrentSession(sessionFile)

        notepad.activateFile(file1)
        notepad.close()
        notepad.activateFile(file2)
        notepad.close()

        sessionFiles = notepad.getSessionFiles(sessionFile)
        lowercaseSessionFiles = [file.lower() for file in sessionFiles]

        self.assertIn(file1, lowercaseSessionFiles)
        self.assertIn(file2, lowercaseSessionFiles)


    def test_reloadFile(self):
        notepad.new()
        editor.write('Reload test')
        filename = self.get_temp_filename()
        notepad.saveAs(filename)
        f = open(filename, "w")
        f.write('Updated outside')
        f.close()
        beforeReload = editor.getText()
        notepad.reloadFile(filename, False)
        afterReload = editor.getText()
        notepad.close()

        self.assertEqual(beforeReload, 'Reload test')
        self.assertEqual(afterReload, 'Updated outside')

    def test_getPluginConfigDir(self):
        dir = notepad.getPluginConfigDir()
        self.assertTrue(dir.lower().endswith('plugins\\config'))

    def test_nppCommandLineDir(self):
        dir = notepad.getNppDir()
        commandLine = notepad.getCommandLine()

        nppExe = shlex.split(commandLine)[0]
        nppDirOfExe = os.path.dirname(nppExe)

        self.assertEqual(dir, nppDirOfExe)


suite = unittest.TestLoader().loadTestsFromTestCase(NotepadTestCase)