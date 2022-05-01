# -*- coding: utf-8 -*-
from Npp import editor, notepad
from unidecode import unidecode
from collections import OrderedDict
import tkinter as tk
import random

flags = OrderedDict() # default setting
flags['sorting'            ] = True
flags['case_sensitive'     ] = False
flags['accent_converted'   ] = False
flags['reverse_ordering'   ] = False
flags['random_sort'        ] = False
flags['remove_duplicates'  ] = False

'''
    Implements basic line sort functionality, current feature list
        - sorting ascending, descending, reverse ordering
        - ignore accented characters -> means e.g. Ä gets replaced by A
        - remove duplicate lines (even without sorting)
        - sort whole text or selected lines only

    Outstanding (and known issues):
        - sort rectangular selection with zero-width
        - removing duplicates on rectangular selection

    Current version: 0.3

    Version: 0.3
        - ignore leading/trailing spaces when comparing duplicates
        - sort rectangular selection only on selected lines

    Version: 0.2
        - bug fixes (thanks to Scott)
        - change to unicode (accented chars bug)
        - first rectangular selection implemented (zero-width outstanding)
        - sort as numeric (for rectangular selection only)
        - random sort (thanks to Scott again)

    Version 0.1
        - sorting ascending, descending, reverse ordering
        - ignore accented characters -> means e.g. Ä gets replaced by A
        - remove duplicate lines (even without sorting)
        - sort selected text only (vertical selection)
'''

class SorterWindow():

    @staticmethod
    def remove_duplicates(lines, case_sensitive=False, sort=True):
        __unique = set()
        for line in lines:
            line = line.strip()
            if (sort is False) and (case_sensitive is False):
                line = line.lower()
            if line not in __unique:
                yield line
                __unique.add(line)


    def __sort(self, text, user_defined_key=None):
        _key = unicode.lower if not flags['case_sensitive'] else None
        if not user_defined_key is None:
            _key = user_defined_key

        _lines = text.splitlines()

        if flags['accent_converted']:
            _lines = [unidecode(line).decode('utf8') for line in _lines]
            print ('\n'.join(_lines))

        if flags['sorting'] and not flags['random_sort']:
            _lines.sort(key=_key, reverse=flags['reverse_ordering'])
        elif flags['random_sort']:
            _lines = random.sample(_lines, len(_lines))

        if flags['remove_duplicates']:
            _lines = list(self.remove_duplicates(_lines, flags['case_sensitive'], flags['sorting']))

        return _lines


    def center(self):
        self.window.update_idletasks()
        w = self.window.winfo_screenwidth()
        h = self.window.winfo_screenheight()
        size = tuple(int(_) for _ in self.window.geometry().split('+')[0].split('x'))
        x = w/2 - size[0]/2
        y = h/2 - size[1]/2
        self.window.geometry("%dx%d+%d+%d" % (size + (x, y)))

    def run(self):
        line_ending = ['\r\n', '\r', '\n'][notepad.getFormatType()]

        if editor.getSelectionEmpty():
            editor.setText(line_ending.join(self.__sort(editor.getCharacterPointer())))

        elif editor.getSelectionMode() == 1:
            print ('-->> {}'.format(editor.getSelectionNStart(0)))
            start = editor.getSelectionNStart(0)
            end = editor.getSelectionNEnd(0)
            start_column = editor.getColumn(start)
            end_column = editor.getColumn(end)
            line_start, line_end = editor.getUserLineSelection()
            start_position_selected_lines = editor.positionFromLine(line_start)
            end_position_selected_lines = editor.getLineEndPosition(line_end)

            def sort_as_int_if_possible(text):
                return int(text.strip()) if text.strip().isdigit() else text

            if (line_start==0) and (line_end==editor.getLineCount()-1):
                editor.setText(line_ending.join(self.__sort(editor.getCharacterPointer(),lambda x: sort_as_int_if_possible(x[start_column:end_column]))))
            else:
                lines = self.__sort(editor.getTextRange(start_position_selected_lines, end_position_selected_lines),
                                    lambda x: sort_as_int_if_possible(x[start_column:end_column]))

                print (line_ending.join(lines))
                editor.setTarget(start_position_selected_lines, end_position_selected_lines)
                editor.replaceTarget(line_ending.join(lines))
        else:
            text_of_sel = editor.getTextRange(*editor.getUserCharSelection())
            line_end_of_sel = line_ending if text_of_sel.endswith(line_ending) else ''
            lines = self.__sort(text_of_sel)
            editor.replaceSel(line_ending.join(lines) + line_end_of_sel)

        self.window.destroy()

    def func(self, event):
        self.run()

    def onClick(self):
        for i, key in enumerate(flags.keys()):
            flags[key] = bool(self.vars[i].get())


    def __init__(self):
        self.window = tk.Tk()
        self.window.title('Sorter')
        self.window.resizable(False, False)
        self.window.attributes("-toolwindow",1)
        self.window.attributes("-topmost",1)
        self.window.bind('<Return>', self.func)
        self.window.geometry('142x{}'.format((len(flags.keys())+2)*23))


        self.vars = []
        for i, option in enumerate(flags.keys()):
            self.var = tk.BooleanVar()
            cb = tk.Checkbutton(self.window, text='', variable=self.var, command=self.onClick)
            if flags.get(option, False):
                cb.select()
            cb.grid(row=i,column=0)

            tk.Label(self.window, text=option, relief=tk.FLAT).grid(row=i,column=1,sticky=tk.W)
            self.vars.append(self.var)


        tk.Button(self.window,
                  text='RUN IT',
                  command=self.run,
                  relief=tk.FLAT).grid(row=i+1,
                                       column=0,
                                       columnspan=2,
                                       sticky=tk.N+tk.S+tk.W+tk.E,
                                       padx=5,
                                       pady=5 )

        self.center()
        self.window.focus_force()
        self.window.mainloop()

SorterWindow()
