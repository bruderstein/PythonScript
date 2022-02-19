# -*- coding: utf-8 -*-
#From https://notepad-plus-plus.org/community/topic/14501/has-a-plugin-like-sublime-plugin-brackethighlighter/7

try:

    BH__dict

except NameError:

    BH__dict = dict()

    BH__dict['indic_for_box_at_caret'] = 10  # pick a free indicator number

    def indicatorOptionsSet(indicator_number, indicator_style, rgb_color_tup, alpha, outline_alpha, draw_under_text, which_editor=editor):
        which_editor.indicSetStyle(indicator_number, indicator_style)       # e.g. INDICATORSTYLE.ROUNDBOX
        which_editor.indicSetFore(indicator_number, rgb_color_tup)
        which_editor.indicSetAlpha(indicator_number, alpha)                 # integer
        which_editor.indicSetOutlineAlpha(indicator_number, outline_alpha)  # integer
        which_editor.indicSetUnder(indicator_number, draw_under_text)       # boolean

    for editorX in (editor1, editor2):
        indicatorOptionsSet(BH__dict['indic_for_box_at_caret'], INDICATORSTYLE.STRAIGHTBOX, (238,121,159), 0, 255, True, editorX)  # white box rimmed in "pale violet red 2"

    BH__dict['last_modificationType_for_hack'] = None

    def BH__containing_box_indices_into_string(str_containing_caret, caret_index_into_str):

        class Stack:
            def __init__(self): self.clear()
            def isEmpty(self): return self.size() == 0
            def push(self, item): self.items.append(item)
            def pop(self): return None if self.size() == 0 else self.items.pop()
            def peek(self): return None if self.size() == 0 else self.items[self.size() - 1]
            def size(self): return len(self.items)
            def clear(self): self.items = []

        retval = (None, None)  # default to no valid box

        get_opening_char_via_closing_char_dict = {
            ')' : '(',
            ']' : '[',
            '}' : '{',
            }
        get_closing_char_via_opening_char_dict = dict((v, k) for (k, v) in get_opening_char_via_closing_char_dict.items())

        closing_chars = get_opening_char_via_closing_char_dict.keys()
        opening_chars = get_opening_char_via_closing_char_dict.values()

        box_ending_index = -1
        box_starting_index = -1

        stack = Stack()

        for j in range(caret_index_into_str, len(str_containing_caret)):
            c = str_containing_caret[j]
            if c in closing_chars:
                if stack.isEmpty():
                    box_ending_index = j
                    break
                else:
                    if stack.peek() ==  get_opening_char_via_closing_char_dict[c]:
                        stack.pop()
                    else:
                        break  # unbalanced
            elif c in opening_chars:
                stack.push(c)

        if box_ending_index != -1:
            stack.clear()
            box_starting_index = -1
            for j in range(caret_index_into_str - 1, -1, -1):
                c = str_containing_caret[j]
                if c in opening_chars:
                    if stack.isEmpty():
                        box_starting_index = j
                        break
                    else:
                        if stack.peek() ==  get_closing_char_via_opening_char_dict[c]:
                            stack.pop()
                        else:
                            break  # unbalanced
                elif c in closing_chars:
                    stack.push(c)

        if box_ending_index != -1:
            if box_starting_index != -1:
                if str_containing_caret[box_ending_index] == get_closing_char_via_opening_char_dict[str_containing_caret[box_starting_index]]:
                    retval = (box_starting_index, box_ending_index + 1)

        return retval

    def BH__callback_sci_MODIFIED(args):
        global BH__dict
        BH__dict['last_modificationType_for_hack'] = args['modificationType']

    def BH__fileIsCloned(file_name_to_test):
        retval = False
        clone_detect_dict = {}
        file_tup_list = notepad.getFiles()
        for tup in file_tup_list:
            (filename, _, _, _) = tup
            if filename not in clone_detect_dict:
                clone_detect_dict[filename] = 0
            else:
                clone_detect_dict[filename] += 1
                if filename == file_name_to_test: break
        if file_name_to_test in clone_detect_dict:
            if clone_detect_dict[file_name_to_test] >= 1: retval = True
        return retval

    def BH__fileIsClonedAndIsActiveInBothViews(file_name_to_test):
        retval = False
        if editor1 and editor2:
            # both views are in use
            if BH__fileIsCloned(file_name_to_test):
                curr_doc_index_main_view = notepad.getCurrentDocIndex(0)
                curr_doc_index_2nd_view = notepad.getCurrentDocIndex(1)
                main_view_active_doc_bool = False
                secondary_view_active_doc_bool = False
                file_tup_list = notepad.getFiles()
                for tup in file_tup_list:
                    (filename, _, index_in_view, view_number) = tup
                    if filename == file_name_to_test:
                        if view_number == 0:
                            if index_in_view == curr_doc_index_main_view:
                                main_view_active_doc_bool = True
                        elif view_number == 1:
                            if index_in_view == curr_doc_index_2nd_view:
                                secondary_view_active_doc_bool = True
                        if main_view_active_doc_bool and secondary_view_active_doc_bool:
                            retval = True
                            break
        return retval

    def BH__getViewableEditorAndRangeTupleListList(work_across_both_views):
        retval = []
        # retval looks like these examples:
        #  [ ( editor, [ (0, 1000), (2020, 3000) ] ) ]
        #  [ ( editor1, [ (0, 1000), (2020, 3000) ] ), ( editor2, [ (4000, 5000), (6020, 7000) ] ) ]
        def consolidate_range_tuple_list(range_tup_list):
            sorted_range_tup_list = sorted(range_tup_list)  # sort criteria is first element of tuple in list
            saved_2element_list = list(sorted_range_tup_list[0])
            for (start, end) in sorted_range_tup_list:
                if start <= saved_2element_list[1]:
                    saved_2element_list[1] = max(saved_2element_list[1], end)
                else:
                    yield tuple(saved_2element_list)
                    saved_2element_list[0] = start
                    saved_2element_list[1] = end
            yield tuple(saved_2element_list)
        def get_onscreen_pos_tup_list(which_editor):  # which_editor is editor1 or editor2 (or maybe even just plain editor)
            # loosely based upon the N++ source for SmartHighlighter::highlightViewWithWord()
            retval_tup_list = list()
            temp_tup_list = []
            MAXLINEHIGHLIGHT = 400
            firstLine = which_editor.getFirstVisibleLine()
            currentLine = firstLine
            nbLineOnScreen = which_editor.linesOnScreen()
            nrLines = min(nbLineOnScreen, MAXLINEHIGHLIGHT) + 1
            lastLine = firstLine + nrLines
            prevDocLineChecked = -1
            break_out = False
            while currentLine < lastLine:
                docLine = which_editor.docLineFromVisible(currentLine)
                if docLine != prevDocLineChecked:
                    prevDocLineChecked = docLine
                    startPos = which_editor.positionFromLine(docLine)
                    endPos = which_editor.positionFromLine(docLine + 1)
                    if endPos == -1:
                        endPos = which_editor.getTextLength() - 1
                        break_out = True
                    if endPos > startPos: temp_tup_list.append((startPos, endPos))
                    if break_out: break
                currentLine += 1
            if len(temp_tup_list) > 0:
                retval_tup_list = list(consolidate_range_tuple_list(temp_tup_list))
            return retval_tup_list
        both_views_open = True if editor1 and editor2 else False
        curr_file_active_in_both_views = BH__fileIsClonedAndIsActiveInBothViews(notepad.getCurrentFilename()) if both_views_open else False
        if both_views_open:
            ed1_range_tup_list = get_onscreen_pos_tup_list(editor1)
            ed2_range_tup_list = get_onscreen_pos_tup_list(editor2)
        if curr_file_active_in_both_views:
            range_tup_list = list(consolidate_range_tuple_list(ed1_range_tup_list + ed2_range_tup_list))
            retval.append((editor, range_tup_list))
        elif both_views_open and work_across_both_views:
            retval.append((editor1, ed1_range_tup_list))
            retval.append((editor2, ed2_range_tup_list))
        else:
            range_tup_list = get_onscreen_pos_tup_list(editor)
            retval.append((editor, range_tup_list))
        return retval

    def BH__callback_sci_UPDATEUI(args):

        # hack, see https://notepad-plus-plus.org/community/topic/12360/vi-simulator-how-to-highlight-a-word/27, look for "16400" in code:
        if args['updated'] == UPDATE.CONTENT and BH__dict['last_modificationType_for_hack'] == (MODIFICATIONFLAGS.CHANGEINDICATOR | MODIFICATIONFLAGS.USER): return

        for (editorX, pos_range_tuple_list) in BH__getViewableEditorAndRangeTupleListList(True):

            # clear out any existing highlighting in areas the user can currently see
            for (start_pos, end_pos) in pos_range_tuple_list:
                editorX.setIndicatorCurrent(BH__dict['indic_for_box_at_caret'])
                editorX.indicatorClearRange(start_pos, end_pos - start_pos)

            for (start_pos, end_pos) in pos_range_tuple_list:

                if start_pos <= editorX.getCurrentPos() <= end_pos:

                    (box_start_offset, box_end_offset) = BH__containing_box_indices_into_string(
                        editorX.getTextRange(start_pos, end_pos),
                        editorX.getCurrentPos() - start_pos
                        )

                    if box_start_offset != None:
                        size_of_box_in_chars = box_end_offset - box_start_offset
                        if size_of_box_in_chars <= 2:
                            pass  # rather pointless to box in if the opening and closing delims are right next to each other
                        else:
                            editorX.setIndicatorCurrent(BH__dict['indic_for_box_at_caret'])
                            editorX.indicatorFillRange(start_pos + box_start_offset, size_of_box_in_chars)

    editor.callbackSync(BH__callback_sci_UPDATEUI, [SCINTILLANOTIFICATION.UPDATEUI])  # install callback
    editor.callbackSync(BH__callback_sci_MODIFIED, [SCINTILLANOTIFICATION.MODIFIED])  # may not need to be "Sync", but for now we'll make it that way

else:


    editor.setSelectionMode(editor.getSelectionMode())  # force manual UPDATEUI to happen

