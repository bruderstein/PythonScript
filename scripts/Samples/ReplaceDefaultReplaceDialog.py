# -*- coding: utf-8 -*-
"""
    ReplaceDefaultReplaceDialog

    The built-in replace dialog doesn't allow to have a
    multiline replacement value.
    This script solves this missing feature.

    In order to achieve this goal using WinAPI (ctypes) is needed.
    What happens is basically the following:
        - if two selections are made
            - call built-in dialogue
            - find needed window control handles
            - replace the content in those controls with the multiline version of text
        - else, just call built-in dialogue

    Usage:
        Make two selections and execute script.
        Note, rectangular selections are not supported and due to
        internal buffer restrictions text is truncated to 2046 if needed.

"""
from Npp import editor, notepad, BUFFERENCODING, MENUCOMMAND, SELECTIONMODE
import ctypes
from ctypes.wintypes import BOOL, HWND, LPARAM

try:
    # on first run this will generate an NameError exception
    REPLACE_DIALOG().show()
except NameError:

    class REPLACE_DIALOG_SINGLETON(type):
        ''' Ensures that only one instance exists '''
        _instance = None
        def __call__(cls, *args, **kwargs):
            ''' The real constructor and first method called when
                a new instance should be created.
                On first instantiation class variable _instance gets itself assigned,
                every subsequent instantiation try returns this object
            '''
            if cls._instance is None:
                cls._instance = super(REPLACE_DIALOG_SINGLETON, cls).__call__(*args, **kwargs)
            return cls._instance

    class REPLACE_DIALOG(object):
        ''' Extend default replace dialog with multiline replace feature. '''
        __metaclass__ = REPLACE_DIALOG_SINGLETON

        def __init__(self):
            ''' defines the needed WinAPI functions
                as well as the replace and findwhat lables,
                which might be adapted if a localized version is used
            '''
            self.WNDENUMPROC = ctypes.WINFUNCTYPE(BOOL, HWND, LPARAM)
            self.FindWindow = ctypes.windll.user32.FindWindowW
            self.EnumChildWindows = ctypes.windll.user32.EnumChildWindows
            self.GetClassName = ctypes.windll.user32.GetClassNameW
            self.GetWindowText = ctypes.windll.user32.GetWindowTextW
            self.GetWindowTextLength = ctypes.windll.user32.GetWindowTextLengthW
            self.SetWindowText = ctypes.windll.user32.SetWindowTextW
            self.GetACP = ctypes.windll.kernel32.GetACP
            self.create_unicode_buffer = ctypes.create_unicode_buffer

            self.replace_tab_caption  = u'Replace'
            self.replacewith_caption = u'Rep&lace with :'
            self.replacewith_is_next_ctrl = False
            self.replacewith_handle = None

            self.findwhat_caption = u'&Find what :'
            self.findwhat_is_next_ctrl = False
            self.findwhat_handle = None


        def return_proper_string(self, selected_text):
            if notepad.getEncoding() == BUFFERENCODING.ENC8BIT:
                _selected_text = unicode(selected_text, '{}'.format(self.GetACP()))
            else:
                _selected_text = unicode(selected_text, 'utf-8')
            return _selected_text


        def enum_callback(self, hwnd, lparam):
            curr_class = self.create_unicode_buffer(256)
            self.GetClassName(hwnd, curr_class, 256)
            length = self.GetWindowTextLength(hwnd)
            buff = self.create_unicode_buffer(length + 1)
            self.GetWindowText(hwnd, buff, length + 1)
            if curr_class.value.lower() == 'static':
                if buff.value == self.replacewith_caption:
                    self.replacewith_is_next_ctrl = True
                elif buff.value == self.findwhat_caption:
                    self.findwhat_is_next_ctrl = True
            elif curr_class.value.lower() == 'edit':
                if self.findwhat_is_next_ctrl:
                    self.findwhat_handle = hwnd
                    self.findwhat_is_next_ctrl = False
                elif self.replacewith_is_next_ctrl:
                    self.replacewith_handle = hwnd
                    self.replacewith_is_next_ctrl = False
                    return False  # stop enumeration
            return True  # let enumeration continue


        def find_controls(self):
            find_dialog_hwnd = self.FindWindow(None, self.replace_tab_caption)
            if find_dialog_hwnd:
                self.EnumChildWindows(find_dialog_hwnd, self.WNDENUMPROC(self.enum_callback), 0)
            else:
                notepad.messageBox('Error: Bad value for find_dialog_hwnd')


        def show(self):
            msg = ''
            notepad.menuCommand(MENUCOMMAND.SEARCH_REPLACE)
            if (self.findwhat_handle is None) or (self.replacewith_handle is None):
                self.find_controls()

            if editor.getSelections() == 2:
                if (editor.getSelectionMode() == SELECTIONMODE.STREAM) and (not editor.getSelectionEmpty()):
                    _find_what = editor.getTextRange(editor.getSelectionNStart(0), editor.getSelectionNEnd(0))
                    _replace_with = editor.getTextRange(editor.getSelectionNStart(1), editor.getSelectionNEnd(1))

                    if len(_find_what) > 2046:
                        _find_what = _find_what[:2046]
                        msg += 'Warning:  Selected text too long for find-what box; truncating to 2046 characters.\n'

                    if len(_replace_with) > 2046:
                        _replace_with = _replace_with[:2046]
                        msg += 'Warning:  Selected text too long for replace-with box; truncating to 2046 characters.'

                    if self.SetWindowText(self.findwhat_handle, self.return_proper_string(_find_what)) == 0:
                        msg += 'Error:  Problem setting find-what text\n'

                    if self.SetWindowText(self.replacewith_handle, self.return_proper_string(_replace_with)) == 0:
                        msg += 'Error:  Problem setting replace-with text'

                else:
                    msg = 'Error:  Either empty selection or unsupported selection mode detected'

                if len(msg) > 0: notepad.messageBox(msg)


    REPLACE_DIALOG().show()