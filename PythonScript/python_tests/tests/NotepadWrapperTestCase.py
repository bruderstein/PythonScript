# -*- coding: utf-8 -*-

import unittest
import os
import shlex
import tempfile
import ctypes, ctypes.wintypes
import xml.etree.ElementTree as et
import time
from threading import Timer
import subprocess

from Npp import *

EnumWindowsProc = ctypes.WINFUNCTYPE(ctypes.c_bool,
                                     ctypes.wintypes.HWND,
                                     ctypes.wintypes.LPARAM)

NPP_HANDLE = ctypes.windll.user32.FindWindowW(u'Notepad++', None)
SCI_HANDLE = ctypes.windll.user32.FindWindowExW(NPP_HANDLE, None, u"Scintilla", None)

class ArgumentError(Exception):
    pass

class NotepadTestCase(unittest.TestCase):
    def setUp(self):
        self.files_to_delete = []


    def tearDown(self):
        for file in self.files_to_delete:
            os.remove(file)

# helper functions


    def normalise_filename(self, filename):
        buf = ctypes.create_unicode_buffer(260)
        GetLongPathName = ctypes.windll.kernel32.GetLongPathNameW
        GetLongPathName(filename.decode('windows-1252'), buf, 260)
        return buf.value.lower()


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


    def _get_config_directory(self):
        plugin_config_dir = notepad.getPluginConfigDir()
        return os.path.abspath(os.path.join(plugin_config_dir, '..\\..'))


    def _get_active_styler_xml(self):
        npp_config_file = os.path.join(self._get_config_directory(), r'config.xml')
        xml_doc = et.parse(npp_config_file)
        return xml_doc.find('GUIConfigs/GUIConfig[@name="stylerTheme"]').get('path')


    def _get_current_lang_xml(self):
        npp_config_file = os.path.join(self._get_config_directory(), r'nativeLang.xml')
        if os.path.exists(npp_config_file):
            xml_doc = et.parse(npp_config_file)
            return xml_doc.find('Native-Langue').get('filename')
        return None

    def _get_disable_update_xml(self):
        npp_config_file = os.path.join(self._get_config_directory(), r'config.xml')
        xml_doc = et.parse(npp_config_file)
        return xml_doc.find('GUIConfigs/GUIConfig[@name="noUpdate"]').text


    def _invalid_parameter_passed(self, method, *args):
        ''' checks if invalid parameters passed trigger the
            ArgumentError exception by checking if the
            exception messages contains the text
            "did not match C++ signature" '''
        try:
            method(*args)
        except Exception as e:
            if 'did not match C++ signature' in e.message:
                raise ArgumentError
            elif 'invalid aka unknown bufferID provided' in e.message:
                raise ArgumentError
            else:
                raise Exception('args:{}  -  message:{}'.format(args, e.message))


    def __test_invalid_parameter_passed(self, notepad_method):
        ''' not only does it assertRaise the ArgumentError exception,
            in addition it also tests that None and -1 doesn't crash npp'''
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None,None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None,None,None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1,-1)


    @staticmethod
    def foreach_window(hwnd, lParam):
        if ctypes.windll.user32.IsWindowVisible(hwnd):
            length = ctypes.windll.user32.GetWindowTextLengthW(hwnd) + 1
            if length > 1:
                buffer = ctypes.create_unicode_buffer(length)
                ctypes.windll.user32.GetWindowTextW(hwnd, buffer, length)
                if buffer.value == ctypes.wstring_at(lParam):
                    return False
        return True


    def find_child_window(self, caption):
        return not ctypes.windll.user32.EnumChildWindows(NPP_HANDLE,
                                                         EnumWindowsProc(self.foreach_window),
                                                         ctypes.create_unicode_buffer(caption))

# old tests

   
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
            lowercaseFiles.append((self.normalise_filename(entry[0]), entry[1], entry[2], entry[3]))

        # clean up
        notepad.activateBufferID(bufferID1)
        editor.setSavePoint()
        notepad.close()
        notepad.activateBufferID(bufferID2)
        editor.setSavePoint()
        notepad.close()

        self.assertIn((self.normalise_filename(file1), bufferID1, index1, 0), lowercaseFiles)
        self.assertIn((self.normalise_filename(file2), bufferID2, index2, 0), lowercaseFiles)
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
        normalisedSessionFiles = [self.normalise_filename(f) for f in sessionFiles]
        self.assertEqual(normalisedSessionFiles, [self.normalise_filename(file1), self.normalise_filename(file2)])


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
        lowercaseSessionFiles = [self.normalise_filename(file) for file in sessionFiles]

        self.assertIn(self.normalise_filename(file1), lowercaseSessionFiles)
        self.assertIn(self.normalise_filename(file2), lowercaseSessionFiles)


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


# new tests
# each test should check
#  1) if expected return value type has been received
#  2) if expected exception gets called when passing invalid parameters and
#  3) if the expected functionality happens

    def test_activateIndex(self):
        ''' '''
        # default: two int parameters
        notepad_method = notepad.activateIndex
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None,None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None,None,None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1,-1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '','')
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '42','42')
        # test return code
        # test functionality


    def test_allocateCmdID(self):
        ''' '''
        notepad_method = notepad.allocateCmdID
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '')
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, 0,0)
        # test return code
        # test functionality


    def test_allocateMarker(self):
        ''' '''
        notepad_method = notepad.allocateMarker
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '')
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, 0,0)
        # test return code
        # test functionality


    def test_allocateSupported(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.allocateSupported)
        # test return code
        # test functionality


    def test_callback(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.callback)
        # test return code
        # test functionality


    def test_clearCallbacks(self):
        ''' '''
        notepad_method = notepad.clearCallbacks
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, 0,0)
        # test return code
        # test functionality


    def test_closeAll(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.closeAll)
        # test return code
        # test functionality


    def test_closeAllButCurrent(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.closeAllButCurrent)
        # test return code
        # test functionality

	# TODO: NPP BUG - Crash
    # 
    # def test_createScintilla(self):
        # ''' '''
        # self.__test_invalid_parameter_passed(notepad.createScintilla)
        # # test return code
        # # test functionality


    def test_decodeSci(self):
        ''' test if codepage changed to ANSI'''
        notepad_method = notepad.decodeSci
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '')
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, 0,0)

        notepad.new()
        self.assertIsInstance(notepad.decodeSci(0), int)
        self.assertEqual(editor.getCodePage(), 0)
        notepad.close()

	# TODO: NPP BUG - Crash
    @unittest.skipUnless(notepad.getVersion() > (7,5,8), "NPP BUG STILL EXISTS")
    def test_destroyScintilla(self):
        ''' '''
        notepad_method = notepad.destroyScintilla
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '')
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, 0,0)
        # test return code
        # test functionality


    def test_disableAutoUpdate(self):
        ''' '''
        WM_CLOSE = 0x010
        def start_and_immediately_stop_new_npp_instance():
            process = subprocess.Popen([r'notepad++.exe', '-multiInst'])
            process_id = ctypes.windll.kernel32.GetProcessId(int(process._handle))

            time.sleep(1) # time to create the window

            def find_newly_created_npp_instance(hwnd, lparam):
                lpdw_process_id = ctypes.c_ulong()
                ctypes.windll.user32.GetWindowThreadProcessId(hwnd, ctypes.byref(lpdw_process_id))
                if lpdw_process_id.value == lparam:
                    curr_class = ctypes.create_unicode_buffer(256)
                    ctypes.windll.user32.GetClassNameW(hwnd, curr_class, 256)
                    if curr_class.value == u'Notepad++':
                        ctypes.windll.user32.SendMessageW(hwnd, WM_CLOSE, 0, 0)
                        return False
                return True

            ctypes.windll.user32.EnumWindows(EnumWindowsProc(find_newly_created_npp_instance), process_id)

        def prepare_auto_updater():
            def reset_auto_updater(hwnd, lParam):
                curr_class = ctypes.create_unicode_buffer(256)
                ctypes.windll.user32.GetClassNameW(hwnd, curr_class, 256)

                length = ctypes.windll.user32.GetWindowTextLengthW(hwnd)
                buff = ctypes.create_unicode_buffer(length + 1)
                ctypes.windll.user32.GetWindowTextW(hwnd, buff, length + 1)

                if curr_class.value.lower() == u'button' and buff.value == u'Enable Notepad++ auto-updater':
                    BM_SETCHECK = 0xF1
                    BST_UNCHECKED = 0
                    BST_CHECKED = 1

                    ctypes.windll.user32.SendMessageW(hwnd,BM_SETCHECK, BST_UNCHECKED, None)
                    ctypes.windll.user32.SendMessageW(hwnd,BM_SETCHECK, BST_CHECKED, None)

                    return False

                return True  # let enumeration continue

            notepad.menuCommand(MENUCOMMAND.SETTING_PREFERENCE)
            prefernces_dialog = ctypes.windll.user32.FindWindowW(None, u'Preferences')
            ctypes.windll.user32.EnumChildWindows(prefernces_dialog, EnumWindowsProc(reset_auto_updater), 0)
            ctypes.windll.user32.SendMessageW(prefernces_dialog, WM_CLOSE, 0, 0)

        self.__test_invalid_parameter_passed(notepad.disableAutoUpdate)
        updater_exe = os.path.join(notepad.getNppDir(), u'updater\gup.exe')
        self.assertTrue(os.path.exists(updater_exe), msg=('Wrong test setup!!\n'
                                                          'It is needed to have a file called gup.exe under updater directory\n'
                                                          'Create one and restart npp and the test'))

        if self._get_disable_update_xml() == 'yes':
            prepare_auto_updater()
        start_and_immediately_stop_new_npp_instance()

        self.assertNotEqual(self._get_disable_update_xml(), 'yes', msg='auto updater is already disabled')
        self.assertIsNone(notepad.disableAutoUpdate())
        start_and_immediately_stop_new_npp_instance()
        self.assertEqual(self._get_disable_update_xml(), 'no')
        prepare_auto_updater()

    def test_docSwitcherDisableColumn(self):
        ''' '''
        def search_for_doc_switcher(hwnd, lParam):
            if ctypes.windll.user32.IsWindowVisible(hwnd):
                length = ctypes.windll.user32.GetWindowTextLengthW(hwnd) + 1
                if length > 1:
                    buffer = ctypes.create_unicode_buffer(length)
                    ctypes.windll.user32.GetWindowTextW(hwnd, buffer, length)
                    if buffer.value == ctypes.wstring_at(lParam):
                        control_dict[buffer.value]=hwnd
                        return False
            return True

        def search_for_doc_switcher_controls(hwnd, lParam):
            curr_class = ctypes.create_unicode_buffer(256)
            ctypes.windll.user32.GetClassNameW(hwnd, curr_class, 256)

            if curr_class.value == 'SysHeader32':
                if ctypes.windll.user32.SendMessageW(hwnd, 0x1200, 0, 0) == lParam:
                    return False
            return True

        notepad_method = notepad.docSwitcherDisableColumn
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '')
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, True,True)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, False,False)

        control_dict = {}
        self.assertIsNone(notepad.docSwitcherDisableColumn(False))
        notepad.showDocSwitcher(True)

        ctypes.windll.user32.EnumChildWindows(NPP_HANDLE,
                                              EnumWindowsProc(search_for_doc_switcher),
                                              ctypes.create_unicode_buffer(u'Doc Switcher'))

        return_code = ctypes.windll.user32.EnumChildWindows(control_dict.get(u'Doc Switcher'),
                                                            EnumWindowsProc(search_for_doc_switcher_controls),
                                                            2)
        self.assertEqual(return_code, 0)

        self.assertIsNone(notepad.docSwitcherDisableColumn(True))

        ctypes.windll.user32.EnumChildWindows(NPP_HANDLE,
                                              EnumWindowsProc(search_for_doc_switcher),
                                              ctypes.create_unicode_buffer(u'Doc Switcher'))

        return_code = ctypes.windll.user32.EnumChildWindows(control_dict.get(u'Doc Switcher'),
                                                            EnumWindowsProc(search_for_doc_switcher_controls),
                                                            1)
        self.assertEqual(return_code, 0)
        notepad.showDocSwitcher(False)


    def test_encodeSci(self):
        ''' test if codepage changed to utf8'''
        notepad_method = notepad.encodeSci
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '')
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, 0,0)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, 1,1)
        self.assertIsInstance(notepad.encodeSci(0), int)

        notepad.new()
        self.assertIsInstance(notepad.encodeSci(0), int)
        self.assertEqual(editor.getCodePage(), 65001)
        notepad.close()


    def test_getAppdataPluginsAllowed(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.getAppdataPluginsAllowed)
        # test existance of file allowAppDataPlugins.xml in nppdir
        npp_dir = notepad.getNppDir()
        appdata_plugins_allowed_file = os.path.join(npp_dir, 'allowAppDataPlugins.xml')
        appdata_plugins_allowed = os.path.exists(appdata_plugins_allowed_file)
        do_local_conf_file = os.path.join(npp_dir, 'doLocalConf.xml')
        do_local_conf = os.path.exists(appdata_plugins_allowed_file)

        if appdata_plugins_allowed and do_local_conf:
            appdata_plugins_allowed = False

        _getAppdataPluginsAllowed = notepad.getAppdataPluginsAllowed()

        self.assertIsInstance(_getAppdataPluginsAllowed, bool)
        self.assertIs(_getAppdataPluginsAllowed, appdata_plugins_allowed)

        # if appdata_plugins_allowed:
            # os.unlink(appdata_plugins_allowed_file)
            # self.assertIs(notepad.getAppdataPluginsAllowed(), False)
            # open(appdata_plugins_allowed_file, 'w').close()
        # else:
            # open(appdata_plugins_allowed_file, 'w').close()
            # self.assertIs(notepad.getAppdataPluginsAllowed(), True)
            # os.unlink(appdata_plugins_allowed_file)


    def test_getBufferFilename(self):
        ''' '''
        notepad_method = notepad.getBufferFilename
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '')
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, 0,0)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, 1,1)
        # test return code
        # test functionality


    def test_getCurrentFilename(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.getCurrentFilename)
        # test return code
        # test functionality


    def test_getCurrentNativeLangEncoding(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.getCurrentNativeLangEncoding)
        native_encoding = notepad.getCurrentNativeLangEncoding()
        self.assertIsInstance(native_encoding, int)
        current_lang_xml_doc = self._get_current_lang_xml()
        if current_lang_xml_doc is None:
            current_lang_xml_doc = 'english.xml'
        self.assertEqual(native_encoding, 0 if current_lang_xml_doc == 'english.xml' else 65001)


    def test_getEditorDefaultBackgroundColor(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.getEditorDefaultBackgroundColor)
        default_background_color = notepad.getEditorDefaultBackgroundColor()
        self.assertIsInstance(default_background_color, tuple)

        styler_xml_doc = et.parse(self._get_active_styler_xml())
        default_bgColor = styler_xml_doc.find('GlobalStyles/WidgetStyle[@name="Default Style"]').get('bgColor')
        self.assertEqual('{:02X}{:02X}{:02X}'.format(*default_background_color), default_bgColor)


    def test_getEditorDefaultForegroundColor(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.getEditorDefaultForegroundColor)
        default_foreground_color = notepad.getEditorDefaultForegroundColor()
        self.assertIsInstance(default_foreground_color, tuple)

        styler_xml_doc = et.parse(self._get_active_styler_xml())
        default_fgColor = styler_xml_doc.find('GlobalStyles/WidgetStyle[@name="Default Style"]').get('fgColor')
        self.assertEqual('{:02X}{:02X}{:02X}'.format(*default_foreground_color), default_fgColor)


    def test_getEnableThemeTextureFunc(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.getEnableThemeTextureFunc)
        enable_theme_texture = notepad.getEnableThemeTextureFunc()
        self.assertIsInstance(enable_theme_texture, bool)

        notepad.launchFindInFilesDlg('','')
        dialog_handle = ctypes.windll.user32.FindWindowW(None, u'Find in Files')

        WM_CLOSE = 0x010
        UxTheme = ctypes.WinDLL('UxTheme.dll',use_last_error=True)
        style = UxTheme.IsThemeDialogTextureEnabled(dialog_handle)
        ctypes.windll.user32.SendMessageW(dialog_handle, WM_CLOSE, 0, 0)
        self.assertEqual(enable_theme_texture, bool(style))


    def test_getFormatType(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.getFormatType)
        # test return code
        # test functionality


    def test_getLanguageDesc(self):
        ''' '''
        notepad_method = notepad.getLanguageDesc
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '')
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, 0,0)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, 1,1)
        language_description = notepad.getLanguageDesc(LANGTYPE.PYTHON)
        self.assertIsInstance(language_description, str)
        self.assertEqual(language_description, 'Python file')


    def test_getLanguageName(self):
        ''' '''
        notepad_method = notepad.getLanguageName
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '')
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, 0,0)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, 1,1)
        language_name = notepad.getLanguageName(LANGTYPE.PYTHON)
        self.assertIsInstance(language_name, str)
        self.assertEqual(language_name, 'Python')


    def test_getMenuHandle(self):
        ''' '''
        notepad_method = notepad.getMenuHandle
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '')
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, 0,0)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, 1,1)

        c_npp_menu_handle = ctypes.windll.user32.GetMenu(NPP_HANDLE)
        c_plugins_menu_handle = ctypes.windll.user32.GetSubMenu(c_npp_menu_handle, 10)

        py_plugin_handle = notepad.getMenuHandle(0)
        py_menu_handle = notepad.getMenuHandle(1)

        self.assertIsInstance(py_plugin_handle, int)
        self.assertIsInstance(py_menu_handle, int)

        self.assertEqual(py_plugin_handle, c_plugins_menu_handle)
        self.assertEqual(py_menu_handle, c_npp_menu_handle)


    def test_getNbUserLang(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.getNbUserLang)
        number_of_udls = notepad.getNbUserLang()
        userdefined_xml_file = os.path.join(self._get_config_directory(), r'userDefineLang.xml')
        if os.path.exists(userdefined_xml_file):
            udl_doc = et.parse(userdefined_xml_file)
            number_of_user_langs_from_xml = len(udl_doc.findall('UserLang'))
        else:
            number_of_user_langs_from_xml = 0
        self.assertIsInstance(number_of_udls, int)
        self.assertEqual(number_of_udls, number_of_user_langs_from_xml)


    def test_getPluginMenuHandle(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.getPluginMenuHandle)
        # test return code
        # test functionality


    def test_getPluginVersion(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.getPluginVersion)
        # test return code
        # test functionality


    def test_getVersion(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.getVersion)
        # test return code
        # test functionality


    def test_getWindowsVersion(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.getWindowsVersion)
        windows_version = notepad.getWindowsVersion()
        self.assertIsInstance(windows_version, WINVER)
        self.assertTrue(windows_version in WINVER.values)


    def test_hideMenu(self):
        ''' '''
        notepad_method = notepad.hideMenu
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '')
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, True,True)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, False,False)

        c_npp_menu_handle = ctypes.windll.user32.GetMenu(NPP_HANDLE)
        was_menu_hidden = notepad.hideMenu(True)
        self.assertIsInstance(was_menu_hidden, bool)
        self.assertEqual(not was_menu_hidden, bool(c_npp_menu_handle))

        c_npp_menu_handle = ctypes.windll.user32.GetMenu(NPP_HANDLE)
        was_menu_hidden = notepad.hideMenu(False)
        self.assertIsInstance(was_menu_hidden, bool)
        self.assertEqual(not was_menu_hidden, bool(c_npp_menu_handle))


    def test_hideStatusBar(self):
        ''' '''
        notepad_method = notepad.hideStatusBar
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '')
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, True,True)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, False,False)
        hidden_status_bar = notepad.hideStatusBar(True)
        self.assertIsInstance(hidden_status_bar, bool)
        self.assertEqual(hidden_status_bar, notepad.isStatusBarHidden())
        hidden_status_bar = notepad.hideStatusBar(False)
        self.assertIsInstance(hidden_status_bar, bool)
        self.assertEqual(hidden_status_bar, notepad.isStatusBarHidden())


    def test_hideTabBar(self):
        ''' '''
        notepad_method = notepad.hideTabBar
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '')
        hidden_tab_bar = notepad.hideTabBar()
        self.assertIsNone(hidden_tab_bar)
        self.assertTrue(notepad.isTabBarHidden())


    def test_hideToolBar(self):
        ''' '''
        notepad_method = notepad.hideToolBar
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None,None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '','')
        hidden_tool_bar = notepad.hideToolBar(True)
        self.assertIsInstance(hidden_tool_bar, bool)
        self.assertEqual(hidden_tool_bar, notepad.isToolBarHidden())
        hidden_tool_bar = notepad.hideToolBar(False)
        self.assertIsInstance(hidden_tool_bar, bool)
        self.assertEqual(hidden_tool_bar, notepad.isToolBarHidden())


    def test_isDocSwitcherShown(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.isDocSwitcherShown)
        doc_switcher_shown = notepad.isDocSwitcherShown()
        self.assertIsInstance(doc_switcher_shown, bool)

        res = self.find_child_window(u'Doc Switcher')
        self.assertEqual(doc_switcher_shown, res)

        notepad.showDocSwitcher(True)
        doc_switcher_shown = notepad.isDocSwitcherShown()
        res = self.find_child_window(u'Doc Switcher')
        self.assertEqual(doc_switcher_shown, res)
        notepad.showDocSwitcher(False)


    def test_isMenuHidden(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.isMenuHidden)
        notepad.hideMenu(True)
        is_hidden = notepad.isMenuHidden()
        self.assertIsInstance(is_hidden, bool)
        self.assertTrue(is_hidden)
        notepad.hideMenu(False)
        is_hidden = notepad.isMenuHidden()
        self.assertIsInstance(is_hidden, bool)
        self.assertFalse(is_hidden)


    def test_isStatusBarHidden(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.isStatusBarHidden)
        notepad.hideStatusBar(True)
        is_hidden = notepad.isStatusBarHidden()
        self.assertIsInstance(is_hidden, bool)
        self.assertTrue(is_hidden)
        notepad.hideStatusBar(False)
        is_hidden = notepad.isStatusBarHidden()
        self.assertIsInstance(is_hidden, bool)
        self.assertFalse(is_hidden)


    def test_isTabBarHidden(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.isTabBarHidden)
        notepad.hideTabBar()
        is_hidden = notepad.isTabBarHidden()
        self.assertIsInstance(is_hidden, bool)
        self.assertTrue(is_hidden)


    def test_isToolBarHidden(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.isToolBarHidden)
        notepad.hideToolBar(True)
        is_hidden = notepad.isToolBarHidden()
        self.assertIsInstance(is_hidden, bool)
        self.assertTrue(is_hidden)
        notepad.hideToolBar(False)
        is_hidden = notepad.isToolBarHidden()
        self.assertIsInstance(is_hidden, bool)
        self.assertFalse(is_hidden)


    def test_launchFindInFilesDlg(self):
        ''' '''

        def foreach_window(hwnd, lParam):
            curr_class = ctypes.create_unicode_buffer(256)
            ctypes.windll.user32.GetClassNameW(hwnd, curr_class, 256)

            length = ctypes.windll.user32.GetWindowTextLengthW(hwnd)
            buff = ctypes.create_unicode_buffer(length + 1)
            ctypes.windll.user32.GetWindowTextW(hwnd, buff, length + 1)

            if curr_class.value.lower() == 'static':
                if buff.value == 'Filter&s :':
                    control_dict['filter'] = ''
                elif buff.value == 'Dir&ectory :':
                    control_dict['directory'] = ''
            elif curr_class.value.lower() == 'edit':
                if control_dict.get('filter', None) == '':
                    control_dict['filter'] = buff.value
                elif control_dict.get('directory', None) == '':
                    control_dict['directory'] = buff.value

            return True


        self.__test_invalid_parameter_passed(notepad.launchFindInFilesDlg)

        WM_CLOSE = 0x010
        _filter = u'*.pü'
        _directory = u'C:\\äö ß\\'

        dialog_handle = ctypes.windll.user32.FindWindowW(None, u'Find in Files')
        is_visible = ctypes.windll.user32.IsWindowVisible(dialog_handle)
        self.assertEqual(is_visible,0,msg='Find in Files dialog already open')

        self.assertIsNone(notepad.launchFindInFilesDlg(_directory,_filter))
        is_visible = ctypes.windll.user32.IsWindowVisible(dialog_handle)
        self.assertEqual(is_visible,1,msg='Find in Files dialog not visible')

        control_dict = {}
        ctypes.windll.user32.EnumChildWindows(dialog_handle, EnumWindowsProc(foreach_window), 0)
        ctypes.windll.user32.SendMessageW(dialog_handle, WM_CLOSE, 0, 0)
        time.sleep(0.5)

        self.assertEqual(_directory, control_dict.get('directory', None))
        self.assertEqual(_filter, control_dict.get('filter', None))

        _filter = '*.py'
        _directory = 'C:\\windows\\'
        is_visible = ctypes.windll.user32.IsWindowVisible(dialog_handle)
        self.assertEqual(is_visible,0,msg='Find in Files dialog already open')
        self.assertIsNone(notepad.launchFindInFilesDlg(_directory,_filter))
        is_visible = ctypes.windll.user32.IsWindowVisible(dialog_handle)
        self.assertEqual(is_visible,1,msg='Find in Files dialog not visible')

        control_dict = {}
        ctypes.windll.user32.EnumChildWindows(dialog_handle, EnumWindowsProc(foreach_window), 0)
        ctypes.windll.user32.SendMessageW(dialog_handle, WM_CLOSE, 0, 0)
        time.sleep(0.5)

        self.assertEqual(_directory, control_dict.get('directory', None))
        self.assertEqual(_filter, control_dict.get('filter', None))


    def test_makeCurrentBufferDirty(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.makeCurrentBufferDirty)

        notepad.closeAll()

        ULONG_PTR = ctypes.wintypes.LPVOID
        MFS_ENABLED = 0x0
        MFS_DISABLED = 0x3
        MIIM_STATE = 0x1

        class MENUITEMINFO(ctypes.Structure):
            _fields_ = [ ("cbSize"       , ctypes.wintypes.UINT),
                         ("fMask"        , ctypes.wintypes.UINT),
                         ("fType"        , ctypes.wintypes.UINT),
                         ("fState"       , ctypes.wintypes.UINT),
                         ("wID"          , ctypes.wintypes.UINT),
                         ("hSubMenu"     , ctypes.wintypes.HMENU),
                         ("hbmpChecked"  , ctypes.wintypes.HBITMAP),
                         ("hbmpUnchecked", ctypes.wintypes.HBITMAP),
                         ("dwItemData"   , ULONG_PTR),
                         ("dwTypeData"   , ctypes.wintypes.LPWSTR),
                         ("cch"          , ctypes.wintypes.UINT),
                         ("hbmpItem"     , ctypes.wintypes.HBITMAP),]

        c_npp_menu_handle = ctypes.windll.user32.GetMenu(NPP_HANDLE)
        c_submenu_handle = ctypes.windll.user32.GetSubMenu(c_npp_menu_handle, 0)

        uitem = 6
        fByPosition = True
        mii = MENUITEMINFO()
        mii.cbSize = ctypes.sizeof(mii)
        mii.fMask = MIIM_STATE
        lpmii = ctypes.byref(mii)

        ctypes.windll.user32.GetMenuItemInfoW(c_submenu_handle,
                                              uitem,
                                              fByPosition,
                                              lpmii)

        self.assertEqual(mii.fState, MFS_DISABLED, msg='Expected document is saved')
        self.assertIsInstance(notepad.makeCurrentBufferDirty(), bool)

        ctypes.windll.user32.GetMenuItemInfoW(c_submenu_handle,
                                              uitem,
                                              fByPosition,
                                              lpmii)

        self.assertEqual(mii.fState, MFS_ENABLED, msg='Expected document is dirty')
        notepad.close()


    def test_menuCommand(self):
        ''' '''
        notepad_method = notepad.menuCommand
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None,None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '')
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '','')
        # test return code
        # test functionality


    def test_messageBox(self):
        ''' '''
        notepad_method = notepad.prompt
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None,None,None,None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1,-1,-1)
        # test return code
        # test functionality


    def test_outputDebugString(self):
        ''' '''
        notepad_method = notepad.outputDebugString
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None,None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1)

        result = notepad.outputDebugString('')
        self.assertIsNone(result)
        # test functionality


    def test_prompt(self):
        ''' '''
        notepad_method = notepad.prompt
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None,None,None,None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1,-1,-1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '')
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '','','','')
        # test return code
        # test functionality


    def test_reloadBuffer(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.reloadBuffer)
        # test return code
        # test functionality


    def test_reloadCurrentDocument(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.reloadCurrentDocument)
        # test return code
        # test functionality


    def test_runPluginCommand(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.runPluginCommand)
        # test return code
        # test functionality


    def test_saveAllFiles(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.saveAllFiles)
        # test return code
        # test functionality


    def test_saveFile(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.saveFile)
        self.assertIsNone(notepad.saveFile(''))

        tmpfile = self.get_temp_filename()
        notepad.open(tmpfile)
        text_to_be_saved = 'example_text'
        editor.addText(text_to_be_saved)
        with open(tmpfile, 'r') as f:
            _content = f.read()

        self.assertEqual(_content, '')

        notepad.saveFile(tmpfile)
        with open(tmpfile, 'r') as f:
            _content = f.read()

        self.assertEqual(_content, text_to_be_saved)
        notepad.close()


    def test_setEditorBorderEdge(self):
        ''' '''
        notepad_method = notepad.setEditorBorderEdge
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None,None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None,None,None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1,-1)

        GWL_EXSTYLE = -20
        self.assertIsNone(notepad.setEditorBorderEdge(False))
        style = ctypes.windll.user32.GetWindowLongW(SCI_HANDLE, GWL_EXSTYLE)
        self.assertEqual(style, 0)

        self.assertIsNone(notepad.setEditorBorderEdge(True))
        style = ctypes.windll.user32.GetWindowLongW(SCI_HANDLE, GWL_EXSTYLE)
        self.assertEqual(style, 512)


    def test_setFormatType(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.setFormatType)
        # test return code
        # test functionality


    def test_setSmoothFont(self):
        ''' '''
        notepad_method = notepad.setSmoothFont
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None,None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None,None,None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1,-1)

        return_code = notepad.setSmoothFont(False)
        self.assertIsNone(return_code)
        self.assertEqual(editor.getFontQuality(), 0)

        return_code = notepad.setSmoothFont(True)
        self.assertIsNone(return_code)
        self.assertEqual(editor.getFontQuality(), 3)


    def test_setStatusBar(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.setStatusBar)
        # test return code
        # test functionality


    def test_showDocSwitcher(self):
        ''' '''
        notepad_method = notepad.showDocSwitcher
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None,None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None,None,None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1,-1)

        self.assertIsNone(notepad.showDocSwitcher(True))
        res = self.find_child_window(u'Doc Switcher')
        self.assertTrue(res)

        self.assertIsNone(notepad.showDocSwitcher(False))
        res = self.find_child_window(u'Doc Switcher')
        self.assertFalse(res)


    def test_showTabBar(self):
        ''' '''
        self.__test_invalid_parameter_passed(notepad.showTabBar)
        notepad.showTabBar()
        is_hidden = notepad.isTabBarHidden()
        self.assertIsInstance(is_hidden, bool)
        self.assertFalse(is_hidden)


    def test_triggerTabbarContextMenu(self):
        ''' '''
        notepad_method = notepad.triggerTabbarContextMenu
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None,None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, None,None,None)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, -1,-1,-1)
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '','')
        with self.assertRaises(ArgumentError):
            self._invalid_parameter_passed(notepad_method, '42','42')

        def start_monitor():
            WM_CLOSE = 0x10
            MN_GETHMENU = 0x1E1
            DELAYED_START = 0.5

            time.sleep(DELAYED_START)
            tabbar_context_menu_hwnd = ctypes.windll.user32.FindWindowExW(None, None, u'#32768', None)
            timer.cancel()
            self.assertNotEqual(tabbar_context_menu_hwnd, 0, msg=u"Can't find popup menu")

            menu_handle = ctypes.windll.user32.SendMessageW(tabbar_context_menu_hwnd, MN_GETHMENU, 0, 0)
            item_count = ctypes.windll.user32.GetMenuItemCount(menu_handle)
            self.assertEqual(item_count, 28, msg=u'Expected 28 menu items but received:{}'.format(item_count))
            ctypes.windll.user32.SendMessageW(tabbar_context_menu_hwnd, WM_CLOSE, 0, 0)

        timer = Timer(1, start_monitor)
        timer.start()
        self.assertIsNone(notepad.triggerTabbarContextMenu(0,0))



if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(NotepadTestCase)
    alltests = unittest.TestSuite(suite)

    results = unittest.TestResult()
    alltests.run(results)

    def writeTestFailure(error):
        console.write('\nTEST: %s\n' % error[0])
        console.writeError(error[1])
        console.write('\n----------------------------\n')

    if results.errors:
        for error in results.errors:
            writeTestFailure(error)

    if results.failures:
        for error in results.failures:
            writeTestFailure(error)

    if results.errors or results.failures:
        console.writeError('Tests Run: {}\n  Errors  : {}\n  Failures: {}\n'.format(results.testsRun, len(results.errors), len(results.failures)))
    else:
        console.write('Tests Run: {}\n  Errors  : {}\n  Failures: {}\n'.format(results.testsRun, len(results.errors), len(results.failures)))
        if results.skipped:
            console.write('Skipped: {}\n'.format(len(results.skipped)))
            for skipped_test in results.skipped:
                console.write('     {}  -  {}\n'.format(skipped_test[0], skipped_test[1]))
    console.show()
else:
    suite = unittest.TestLoader().loadTestsFromTestCase(NotepadTestCase)