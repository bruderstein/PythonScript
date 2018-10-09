# -*- coding: utf-8 -*-
from Npp import editor
"""
    MultiEdit
    
    acts based on the position of the cursor, NOT on selection
    - if cursor is located before the word it will prepend new text
    - if cursor is within a word it will overwrite the words and
    - if cursor is at the end of a word if will append new text
    
    Usage: 
    every run will activate the multi_edit function,
    in order to stop it, cancelling the selection would be needed - aka use arrow keys
    
"""

def multi_edit():
    ''' main entry point '''
    
    # multi_edit flags
    ACTION_PREPEND = 0
    ACTION_APPEND = 1
    ACTION_OVERWRITE = 2

    def select_words(current_position, action_type):
        ''' Creates a list of position tuples by using a
            regular expression with boundary flag /b,
            current cursor position gets removed from list
            In addition, creates multiple selection cursors.
            Has undo functionality
        '''

        matches = []
        editor.research('\\b{}\\b'.format(current_word),lambda m: matches.append(m.span(0)))
        matches.remove((word_start_position,word_end_position))
                        
        if action_type == ACTION_PREPEND:
            editor.setSelection(current_position,current_position)
            [editor.addSelection(x[0],x[0]) for x in matches]
        elif action_type == ACTION_APPEND:
            editor.setSelection(current_position,current_position)
            [editor.addSelection(x[1],x[1]) for x in matches]
        elif action_type == ACTION_OVERWRITE:
            editor.setSelection(word_start_position,word_end_position)
            [editor.addSelection(x[0],x[1]) for x in matches]
        else:
            return None
        editor.setMainSelection(0)


    editor.beginUndoAction()

    current_word = editor.getCurrentWord()
    if current_word != '':
        current_position = editor.getCurrentPos()
        word_start_position = editor.wordStartPosition(current_position, True)
        word_end_position = editor.wordEndPosition(current_position, True)
        
        if word_start_position == current_position:
            select_words(current_position, ACTION_PREPEND)
        elif word_end_position == current_position:
            select_words(current_position, ACTION_APPEND)
        else:
            select_words(current_position, ACTION_OVERWRITE)

    editor.endUndoAction()

multi_edit()