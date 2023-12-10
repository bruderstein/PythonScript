# -*- coding: utf-8 -*-
"""
    ColumnLexer - Demo

    A column based lexer, odd and even columns will be styled differently.

    Usage:
    Default column separator is comma, redefine COLUMN_SEPARATOR if needed.
    Run script.

    Note: By commenting or deleting everything, including,
    the <comment_or_delete> tags it is ready to be used
    as an additional lexer

"""

from Npp import editor, notepad, LEXER, SCINTILLANOTIFICATION, NOTIFICATION, LANGTYPE

try:
    # on first run this will generate an NameError exception
    COLUMN_LEXER().main()
except NameError:

    class COLUMN_LEXER_SINGLETON(type):
        ''' Ensures that only one column lexer instance exists and
            prevents of getting multiple callbacks
        '''
        _instance = None
        def __call__(cls, *args, **kwargs):
            ''' The real constructor and first method called when
                a new instance should be created.
                On first instantiation class variable _instance gets itself assigned,
                every subsequent instantiation try returns this object
            '''
            if cls._instance is None:
                cls._instance = super(COLUMN_LEXER_SINGLETON, cls).__call__(*args, **kwargs)
            return cls._instance

    class COLUMN_LEXER(object):
        ''' A column based lexer implementation.
            Odd and even columns do get different styles assigned.
            Only one separator can be defined.
            Line-based approach - styling takes place when new line has been added
        '''
        __metaclass__ = COLUMN_LEXER_SINGLETON

        DEFAULT = 0 # don't change it
        ODD_COLUMN_STYLE = 50
        EVEN_COLUMN_STYLE = 51
        COLUMN_SEPARATOR = ','

        def __init__(self):
            ''' Register needed callbacks on first class instantiation '''
            editor.callbackSync(self.styleneeded_callback, [SCINTILLANOTIFICATION.STYLENEEDED])
            notepad.callback(self.bufferactivated_callback, [NOTIFICATION.BUFFERACTIVATED])
            notepad.callback(self.langchanged_callback, [NOTIFICATION.LANGCHANGED])


        def column_lexer(self, start_pos, end_pos):
            ''' Main lexing logic.
                Gets called by styleneeded callback
            '''

            def style_it(start, length, STYLE):
                ''' Inform scintilla to do the styling'''
                if length >= 0:
                    editor.startStyling(start, 31)
                    editor.setStyling(length, STYLE)

            # first everything will be styled with default style
            style_it(start_pos, end_pos-start_pos, self.DEFAULT)

            # loop over line indexes from start_pos and end_pos
            for line in range(editor.lineFromPosition(start_pos), editor.lineFromPosition(end_pos)):
                # get start position for each line
                line_start_pos = editor.positionFromLine(line)
                # split current line into columns
                columns = editor.getLine(line).split(self.COLUMN_SEPARATOR)
                if len(columns) > 0:
                    # iterate over all columns
                    for i, column in enumerate(columns):
                        # get the width of the current column
                        col_length = len(column)
                        if i % 2 == 0:
                            # even column
                            style_it(line_start_pos,col_length,self.EVEN_COLUMN_STYLE)
                        else:
                            # odd column
                            style_it(line_start_pos,col_length, self.ODD_COLUMN_STYLE)
                        # recalculate start position for next column
                        line_start_pos += col_length+1

            # this needs to stay and to be the last line, to signal scintilla we are done.
            editor.startStyling(end_pos,31)

        def init_scintilla(self):
            ''' Initialize configured styles '''
            editor.setMarginWidthN(0,38)
            editor.setMarginWidthN(1,14)
            editor.setMarginWidthN(2,0)

            if editor.getLexer() != LEXER.CONTAINER:
                editor.setILexer(0)

            editor.styleSetFore(self.ODD_COLUMN_STYLE,  (54,125,198))
            editor.styleSetFore(self.EVEN_COLUMN_STYLE, (87,166,74))
            editor.styleSetBack(self.EVEN_COLUMN_STYLE, (0,50,20))


        def set_lexer_doc(self, bool_value):
            ''' Assign the class name as an additional property
                to every document which should be handled by this lexer
                A value of 1 indicates it should be handled.
            '''
            editor.setProperty(self.__class__.__name__, 1 if bool_value is True else 0)


        def is_lexer_doc(self):
            ''' Check if the current document is of interest
                by reading the class name property.
            '''
            return True if editor.getPropertyInt(self.__class__.__name__, 0) == 1 else False


        def styleneeded_callback(self,args):
            ''' Called by scintilla to inform the lexer
                about the need to style the document.
                If document is of interest call main logic (column_lexer) function
                Ensures that the start position is really the first position per line
            '''
            if self.is_lexer_doc():
                startPos = editor.getEndStyled()
                lineNumber = editor.lineFromPosition(startPos)
                startPos = editor.positionFromLine(lineNumber)
                self.column_lexer(startPos, args['position'])


        def bufferactivated_callback(self,args):
            ''' Called by notepad when document switch happens
                If document is of interest styles need to be reinitialized
            '''
            if self.is_lexer_doc():
                self.init_scintilla()


        def langchanged_callback(self,args):
            ''' Called by notepad when a built-in or udl language switch happens
                If document was previously styled by this lexer it will be reset
                and therefore will not be styled by this lexer anymore until
                script gets executed on this document again.
            '''
            if self.is_lexer_doc():
                self.set_lexer_doc(False)


        def main(self):
            ''' Main entry point
                To prevent issues with other lexers document language will
                be set to normal text, then document does get the class name
                property assigned, styles do get initialized and main lexing
                function does get called on whole document
            '''
            notepad.setLangType(LANGTYPE.TXT)
            self.set_lexer_doc(True)
            self.init_scintilla()
            self.column_lexer(0, editor.getTextLength())

        # <comment_or_delete>
        # just some demo text not really needed by lexer
        notepad.new()
        editor.appendText('''
col1,col2,col3,col4
col1 , col2, col3  ,  col4
,,col3,col4
col1,column2,col3,colum4
jdjkslddskjfd,jfdksldlskdfkjklsdfs,jfkdslldkjfds,jkfdjskdlskdfj
'''
        )
        # </comment_or_delete>

    COLUMN_LEXER().main()
