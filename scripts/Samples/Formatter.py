# -*- coding: utf-8 -*-
"""
    Formatter - Demo

    Format column based document on user choice

    Usage:
    Run script to see how it works

    Note: By commenting or deleting everything, including,
    the <comment_or_delete> tags it is ready to be used

"""
from Npp import editor, notepad, console, MESSAGEBOXFLAGS
from itertools import izip_longest

try:
    import Tkinter as tk
    import ttk
except ImportError as e:
    notepad.messageBox(('Unable to import Tkinter libraries,\n'
                        'these are needed for the UI.\n\n'
                        'Check your installation.\n\n'
                        '{}'.format(e.message)),'Missing Library', MESSAGEBOXFLAGS.ICONERROR)   
    console.show()
    raise


def main():

    list_of_separators = [('Comma',0),('Semicolon',1),('Space',2),('Clipboard', 3)]
    separator_dict = {0:',', 1:';', 2:' ', 3:'clipboard'}

    list_of_areas = [('Whole document',0),('Selection only',1),('Visible lines only',2)]

    format_types_dict = {0:'\r\n',1:'\r',2:'\n'}


    def button_reformat():
        reformat(separator_value.get(), reformat_value.get())
        window.destroy()

    def button_quit(): window.destroy()

    def set_reformat_choice(): reformat_value.set(reformat_value.get())

    def set_separator_choice(): separator_value.set(separator_value.get())

    def prepare(separator, rows, eol):
        column_width_list = []
        for row in rows:
            splitted_row = row.split(separator)
            column_width_list = [max(x) for x in izip_longest(column_width_list, [len(x.strip()) for x in splitted_row], fillvalue=(0))]

        new_formatted_list = []
        for row in (x for x in rows):
            splitted_row = row.split(separator)
            new_formatted_list.append('{} '.format(separator).join(['{0:<{1}}'.format(x.strip(),column_width_list[i]) for i,x in enumerate(splitted_row)]))

        return new_formatted_list

    def reformat(separator,area):
        separator = window.clipboard_get() if separator_dict.get(separator) == 'clipboard' else separator_dict.get(separator)

        used_eol = format_types_dict.get(notepad.getFormatType())

        editor.beginUndoAction()

        if area == 0:
            rows = editor.getCharacterPointer().splitlines()
            rows = prepare(separator, rows, used_eol)
            editor.setText(used_eol.join(rows))
        elif area == 1:
            if editor.selectionIsRectangle():

                selections = [(editor.getSelectionNStart(i),editor.getSelectionNEnd(i)) for i in range(editor.getSelections())]
                rows = [editor.getRangePointer(x, y-x) for x,y in selections]
                rows = prepare(separator, rows, used_eol)

                editor.clearSelections()

                for i, position in zip(reversed(range(len(selections))), reversed(selections)):
                    editor.setTarget(*position)
                    editor.replaceTarget(rows[i])
            else:
                first_selected_line, last_selected_line = editor.getUserLineSelection()
                startpos = editor.positionFromLine(first_selected_line)
                endpos = editor.positionFromLine(last_selected_line)

                rows = editor.getRangePointer(startpos, endpos-startpos)
                rows = prepare(separator, rows.split(used_eol), used_eol)

                editor.setTarget(startpos, endpos)
                editor.replaceTarget(used_eol.join(rows))
        else:
            first_visible_line = editor.getFirstVisibleLine()
            startpos = editor.positionFromLine(first_visible_line)
            endpos = editor.positionFromLine(editor.linesOnScreen() + first_visible_line)

            rows = editor.getRangePointer(startpos, endpos-startpos)
            rows = prepare(separator, rows.split(used_eol), used_eol)

            editor.setTarget(startpos, endpos)
            editor.replaceTarget(used_eol.join(rows))
        editor.endUndoAction()

    window = tk.Tk()
    window.title("Formatter")
    window.attributes('-toolwindow', 1)
    window.attributes('-topmost', 1)
    window.configure(background='#282C34')
    window.grid_columnconfigure(0, weight=1)
    window.grid_columnconfigure(1, weight=1)
        
    control_styles = ttk.Style()
    control_styles.configure('TButton', font=('courier', 16, 'bold'), background='#282C34', relief='flat')
    control_styles.configure('TCheckbutton', font=('courier', 12), background='#282C34', foreground='#B5BCC9')
    control_styles.configure('TRadiobutton', font=('courier', 12), background='#282C34', foreground='#B5BCC9')
    control_styles.configure('TLabelframe.Label', font=('courier', 12, 'italic'), background='#282C34', foreground='#61AFEF')
    control_styles.configure('TLabelframe', background='#282C34', foreground='#B5BCC9')
    control_styles.configure('TLabel', font=('courier', 12), background='#282C34', foreground='#B5BCC9')
    control_styles.configure('TFrame', background='#282C34')
    
    separator_value = tk.IntVar()
    reformat_value = tk.IntVar()

    separator_frame = ttk.LabelFrame(window)
    separator_frame.configure(text='Separator')
    separator_frame.configure(width=280)

    for label, _value in list_of_separators:
        ttk.Radiobutton(separator_frame,
                        text=label,
                        command = set_separator_choice,
                        variable = separator_value,
                        value = _value).pack(padx=10, anchor=tk.W)
    separator_frame.grid(row=0, column=0, padx=10, pady=15, sticky=tk.W+tk.E+tk.N+tk.S)

    reformat_frame = ttk.LabelFrame(window)
    reformat_frame.configure(text='Area to reformat')
    reformat_frame.configure(width=280)

    for label, _value in list_of_areas:
        ttk.Radiobutton(reformat_frame,
                        text=label,
                        command=set_reformat_choice,
                        variable=reformat_value,
                        value=_value).pack(padx=10, anchor=tk.W)
    reformat_frame.grid(row=0, column=1, padx=10, pady=15, sticky=tk.W+tk.E+tk.N+tk.S)

    btn_frame = ttk.Frame(window)
    btn_frame.place(relx=0.02, rely=0.64, relheight=0.32, relwidth=0.96)
    btn_frame.configure(relief=tk.GROOVE)
    btn_frame.configure(borderwidth="2")
    btn_frame.configure(width=550)
    
    btn_reformat = ttk.Button(btn_frame)
    btn_reformat.place(relx=0.07, rely=0.24, height=47, width=207)
    btn_reformat.configure(text='Reformat')
    btn_reformat.configure(width=207)
    btn_reformat.configure(command=button_reformat)

    btn_quit = ttk.Button(btn_frame)
    btn_quit.place(relx=0.58, rely=0.26, height=47, width=207)
    btn_quit.configure(text='Quit')
    btn_quit.configure(width=207)
    btn_quit.configure(command=button_quit)

    # center window
    w = window.winfo_screenwidth()
    h = window.winfo_screenheight()
    size = (600,250)
    xpos = w/2 - size[0]/2
    ypos = h/2 - size[1]/2
    window.geometry("{}x{}+{}+{}".format(*(size + (xpos, ypos))))

    # start app - blocks rest of the script
    window.mainloop()


# <comment_or_delete>
def run_demo():

    demo_message = ('Running in demo mode\n'
                    'By commenting run_demo() line or by commenting/deleting the whole tag it is ready to be used')
    notepad.messageBox(demo_message,'Demo Mode')

    import random
    _number_of_lines = 100000
    _s = 50
    _line_split_separator = ','
    _data = ['{0}{3}{1}{3}{2}\r\n'.format('a'*random.randint(0,_s),'b'*random.randint(0,_s),'c'*random.randint(0,_s),_line_split_separator) for i in xrange(_number_of_lines)]
    notepad.new()
    editor.setText(''.join(_data))
    del _data

run_demo()
# </comment_or_delete>

main()
