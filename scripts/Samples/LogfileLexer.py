# -*- coding: utf-8 -*-
"""
    LogfileLexer - Demo

    Highlights whole lines which matches the regex.

    Usage:
    Load a logfile, modify error and warning regex and
    run script to see how it works

    Note: By commenting or deleting everything, including,
    the <comment_or_delete> tags it is ready to be used
    as an additional lexer

"""
from Npp import editor, notepad, LEXER, SCINTILLANOTIFICATION, NOTIFICATION, LANGTYPE
import re

try:
    # on first run this will generate an NameError exception
    LOGFILE_LEXER().main()
except NameError:

    class LOGFILE_LEXER_SINGLETON(type):
        ''' Ensures that only one log file lexer instance exists and
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
                cls._instance = super(LOGFILE_LEXER_SINGLETON, cls).__call__(*args, **kwargs)
            return cls._instance

    class LOGFILE_LEXER(object):
        ''' A line based lexer implementation.
            Whole line gets coloured if part or complete line matches regex
        '''
        __metaclass__ = LOGFILE_LEXER_SINGLETON

        DEFAULT = 0 # the current default style
        ERROR_STYLE = 60
        WARNING_STYLE = 61

        # define the style colors
        ERROR_STYLE_FOREGROUND = (224,108,117)
        WARNING_STYLE_FOREGROUND = (255,255,128)

        # define the regex which return the position of the matches
        ERROR_REGEX   = '.*error.*'
        WARNING_REGEX = '.*warning.*'

        def __init__(self):
            ''' Register needed callbacks on first class instantiation '''
            editor.callbackSync(self.styleneeded_callback, [SCINTILLANOTIFICATION.STYLENEEDED])
            notepad.callback(self.bufferactivated_callback, [NOTIFICATION.BUFFERACTIVATED])
            notepad.callback(self.langchanged_callback, [NOTIFICATION.LANGCHANGED])


        def logfile_lexer(self, start_pos, end_pos):
            ''' Main lexing logic.
                Gets called by styleneeded callback
            '''

            def style_it(match, STYLE):
                ''' Inform scintilla to do the styling'''
                if match[1]-match[0] >= 0:
                    editor.startStyling(start_pos + match[0], 31)
                    editor.setStyling(match[1]-match[0], STYLE)

            def do_regex(regex):
                ''' return a list of match positions
                    Note, is using python regular expression instead of boost::re
                '''
                return [m.span(0) for m in re.finditer(regex, text, flags=re.I)]

            # ensure that position is really the first position for each line
            start_pos = editor.positionFromLine(editor.lineFromPosition(start_pos))
            # fast but potentially unsafe way to get the text of the line
            text = editor.getRangePointer(start_pos, end_pos-start_pos)

            # first everything will be styled with default style
            style_it((start_pos, end_pos), self.DEFAULT)

            # map style_it function to each match returned by do_regex
            # ordering is important as it might be that a line matches
            # multiple regexes - the last do_regex overwrites previous styling
            map(lambda match: style_it(match, self.WARNING_STYLE), do_regex(self.WARNING_REGEX))
            map(lambda match: style_it(match, self.ERROR_STYLE), do_regex(self.ERROR_REGEX))

            # this needs to stay and to be the last line, to signal scintilla we are done.
            editor.startStyling(end_pos,31)


        def init_scintilla(self):
            ''' Initialize configured styles '''
            editor.setMarginWidthN(0,38)
            editor.setMarginWidthN(1,14)
            editor.setMarginWidthN(2,0)

            if editor.getLexer() != LEXER.CONTAINER:
                editor.setLexer(LEXER.CONTAINER)

            editor.styleSetFore(self.ERROR_STYLE, self.ERROR_STYLE_FOREGROUND)
            editor.styleSetFore(self.WARNING_STYLE, self.WARNING_STYLE_FOREGROUND)


        def set_lexer_doc(self,bool_value):
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
                If document is of interest call main logic (logfile_lexer) function
                Ensures that the start position is really the first position per line
            '''
            if self.is_lexer_doc():
                startPos = editor.getEndStyled()
                lineNumber = editor.lineFromPosition(startPos)
                startPos = editor.positionFromLine(lineNumber)
                self.logfile_lexer(startPos, args['position'])


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
            self.logfile_lexer(0, editor.getTextLength())

        # <comment_or_delete>
        # just some demo text not really needed by lexer
        notepad.new()
        editor.appendText('''
A line with no keywords
followed by a warning line
some
fillings
until
a line with ERROR appears
again
some
text
and a line with error and WARNING
''')
        # </comment_or_delete>

    LOGFILE_LEXER().main()




