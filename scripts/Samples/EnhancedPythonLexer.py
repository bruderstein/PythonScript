# -*- coding: utf-8 -*-
"""
    EnhancedPythonLexer
    
    Sometimes a built-in lexer isn't doing what one wants.
    To enhance the built-in lexer, indicators can be used, which also automatically 
    prevent clashes with the styling operations done by the built-in lexer.
    The only potential issue might be using an indicator used by npp itself.
    
    Usage: 
        In the main function
            - define the lexer_name to be enhanced and
            - create the regex_dict with the regular expressions and
            - the associate colours
            
        The regex dict must be in form key_tuple:value_tuple.
        
        The key_tuple needs to have an unique ID followed by a colour tuple,
        like (3, (181, 188, 201)). 
        In this case the same colour can be used with different regexes.
        
        The value_tuple should use an unicode string followed by a number
        indicating the (sub-)match, 
        like (u'\\b(cls|self)\\b',0)
        which would colour the main match (the words cls and self) or like
        (u'(?:(?:def)\s\w+)\s*\((.*)\):',1)
        which would colour the first submatch, which is (.*) and in this case
        are basically the parameters of a python function.      
        
        Run script.
        
        Note: could be used as an UDL alternative as well.

"""

from Npp import editor, editor1, editor2, notepad, NOTIFICATION, SCINTILLANOTIFICATION, INDICATORSTYLE

try:
    EnhancedPythonLexer().main()
except NameError:

    # ensure that only a single instance is used to prevent getting
    # multiple callbacks executed
    class SingletonEnhancedPythonLexer(type):
        _instance = None
        def __call__(cls, *args, **kwargs):
            if cls._instance is None:
                cls._instance = super(SingletonEnhancedPythonLexer, cls).__call__(*args, **kwargs)
            return cls._instance


    # main class
    class EnhancedPythonLexer(object):

        __metaclass__ = SingletonEnhancedPythonLexer

        @staticmethod
        def set_indicator_attributes(indicator_number, fore=(0,0,0)):
            editor1.indicSetFore(indicator_number, fore)
            editor1.indicSetStyle(indicator_number, INDICATORSTYLE.TEXTFORE)
            editor2.indicSetFore(indicator_number, fore)
            editor2.indicSetStyle(indicator_number, INDICATORSTYLE.TEXTFORE)
            return indicator_number


        @staticmethod
        def paint_it(indicator, pos, length):
            current_line = editor.lineFromPosition(pos)
            line_start_position = editor.positionFromLine(current_line)
            text = editor.getLine(current_line)
            found_comment_char = text.find('#')
            relative_line_position = pos-line_start_position
            if((-1 < found_comment_char < relative_line_position) or
               (text.count('"', 0, relative_line_position) % 2 == 1) or
               (text.count("'", 0, relative_line_position) % 2 == 1)):
                return
            else:
                editor.setIndicatorCurrent(indicator)
                editor.indicatorFillRange(pos,length)


        def do_regex(self, regex, indicator, start_position, end_position):
            editor.setIndicatorCurrent(indicator)
            editor.indicatorClearRange(start_position, end_position-start_position)
            editor.research(regex[0],
                            lambda m: self.paint_it(indicator, m.span(regex[1])[0], m.span(regex[1])[1] - m.span(regex[1])[0]),
                            0,
                            start_position,
                            end_position)


        def style(self):
            line_number = editor.getFirstVisibleLine()
            start_position = editor.positionFromLine(line_number)
            end_position = editor.getLineEndPosition(line_number + editor.linesOnScreen())
            for indicator, regex in self.regex_dict.items():
                self.do_regex(regex, indicator, start_position, end_position)


        def __init__(self):
            editor.callbackSync(self.on_updateui, [SCINTILLANOTIFICATION.UPDATEUI])
            notepad.callback(self.on_langchanged, [NOTIFICATION.LANGCHANGED])
            notepad.callback(self.on_bufferactivated, [NOTIFICATION.BUFFERACTIVATED])
            self.__is_lexer_doc = False
            self.get_lexer_name = lambda: notepad.getLanguageName(notepad.getLangType())


        def set_lexer_doc(self,bool_value):
            editor.setProperty(self.__class__.__name__, 1 if bool_value is True else -1)
            self.__is_lexer_doc = bool_value


        def on_bufferactivated(self,args):
            if (self.get_lexer_name() == self.lexer_name) and (editor.getPropertyInt(self.__class__.__name__) != -1):
                self.__is_lexer_doc = True
            else:
                self.__is_lexer_doc = False


        def on_updateui(self,args):
            if self.__is_lexer_doc:
                self.style()


        def on_langchanged(self,args):
            self.set_lexer_doc(True if self.get_lexer_name() == self.lexer_name else False)


        # customize, if needed
        def main(self):
            # basically what is returned by notepad.getLanguageName(notepad.getLangType())
            self.lexer_name = 'Python'

            self.regex_dict = {
                                self.set_indicator_attributes(0, (224, 108, 117)) : (u'\\b(cls|self)\\b', 0), # cls and self objects
                                self.set_indicator_attributes(1, (209, 154, 102)) : (u'(?:(?:def)\s\w+)\s*\((.*)\):', 1), # function parameter
                                self.set_indicator_attributes(2, (86, 182, 194))  : (u'(\\beditor\\b|\\beditor1\\b|\\beditor2\\b|\\bnotepad\\b|\\bconsole\\b|\\b__init__\\b|\\b__call__\\b|\\b__del__\\b|\\bsuper\\b|\\bobject\\b|\\btype\\b|\\bprint\\b)', 0), # dunder functions and special keywords
                                self.set_indicator_attributes(3, (181, 188, 201)) : (u'[\(\)\[\]\{\}\=,]', 0), # delimiters - default color
                                self.set_indicator_attributes(4, (181, 188, 201)) : (u'\(\\b(bcls|self)\\b', 1), # cls and self object within function parameters - default color
                                self.set_indicator_attributes(5, (181, 188, 201)) : (u'\(\s*\K\*\*|\(\s*\K\*|,\s*\K\*\*|,\s*\K\*', 0), # args and kwargs references within function parameters - default color
                                self.set_indicator_attributes(6, (79, 175, 239))  : (u'(?(?<=def )(?:)(?=\()|(?(?<!class )\\b\w+?\\b(?=\()|(?:)))', 0), # functions and class instances but not definitions
                              }

            self.set_lexer_doc(True)
            self.style()


    EnhancedPythonLexer().main()

