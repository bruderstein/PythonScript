# -*- coding: utf-8 -*-
"""
    HideLines - Demo

    As name suggests it hides the selected lines,
    if there is no selection then it hides the current line

    Usage:
    Select lines to hide and run script to see how it works.
    If no line is selected the line where the cursor is gets hide.

"""
from Npp import editor, SCINTILLANOTIFICATION, MARKERSYMBOL, FOLDACTION, notepad, NOTIFICATION, console, MESSAGEBOXFLAGS

EXTEND_AUTOMATICALLY = False # used as global overwrite

# marker symbols
hidelines_begin14 = '''\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF9\xA6\x72\xFF\xFA\xA7\x73\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF7\xA4\x70\xFF\xF8\xB6\x8B\xFF\xF8\xA5\x71\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF5\xA2\x6E\xFF\xFE\xCA\xA8\xFF\xF4\xB3\x87\xFF\xF6\xA3\x6F\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF3\xA0\x6C\xFF\xF6\xC5\xA2\xFF\xF6\xC6\xA2\xFF\xF0\xB0\x83\xFF\xF4\xA1\x6D\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF1\x9E\x6A\xFF\xFD\xC1\x9B\xFF\xFE\xC1\x9C\xFF\xF0\xC2\x9D\xFF\xFD\xAE\x81\xFF\xF3\xA0\x6C\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xEF\x9C\x68\xFF\xF5\xBC\x95\xFF\xF6\xBD\x95\xFF\xF8\xBE\x97\xFF\xFB\xBF\x99\xFF\xFB\xAD\x7F\xFF\xF1\x9E\x6A\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xED\x9A\x66\xFF\xFD\xB8\x8E\xFF\xFE\xB8\x8F\xFF\xF0\xB9\x90\xFF\xF4\xBB\x93\xFF\xF9\xBE\x97\xFF\xF9\xAB\x7D\xFF\xFF\x9C\x68\xFF\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xEC\x99\x65\xFF\xF4\xB3\x87\xFF\xF6\xB4\x88\xFF\xF8\xB5\x8A\xFF\xFD\xB8\x8E\xFF\xF2\xBA\x92\xFF\xF1\xA6\x76\xFF\xFD\x9A\x66\xFF\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xEA\x97\x63\xFF\xFC\xAE\x80\xFF\xFE\xAF\x82\xFF\xF1\xB1\x84\xFF\xF7\xB4\x89\xFF\xFC\xA3\x72\xFF\xFB\x98\x64\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xE8\x95\x61\xFF\xF4\xAA\x7A\xFF\xF6\xAB\x7C\xFF\xFB\xAE\x80\xFF\xF6\x9F\x6D\xFF\xF9\x96\x62\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xE6\x93\x5F\xFF\xFC\xA5\x73\xFF\xFF\xA7\x76\xFF\xF1\x9B\x68\xFF\xF7\x94\x60\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xE4\x91\x5D\xFF\xF4\xA1\x6D\xFF\xFC\x97\x64\xFF\xF5\x92\x5E\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xE2\x8F\x5B\xFF\xFA\x96\x63\xFF\xF2\x8F\x5B\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xE0\x8D\x59\xFF\xF0\x8D\x59\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'''

hidelines_end14 = '''\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFA\xA7\x73\xFF\xF9\xA6\x72\xFF\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF8\xA5\x71\xFF\xF8\xB6\x8B\xFF\xF7\xA4\x70\xFF\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF6\xA3\x6F\xFF\xF4\xB3\x87\xFF\xFE\xCA\xA8\xFF\xF5\xA2\x6E\xFF\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF4\xA1\x6D\xFF\xF0\xB0\x83\xFF\xF6\xC6\xA2\xFF\xF6\xC5\xA2\xFF\xF3\xA0\x6C\xFF\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF3\xA0\x6C\xFF\xFD\xAE\x81\xFF\xF0\xC2\x9D\xFF\xFE\xC1\x9C\xFF\xFD\xC1\x9B\xFF\xF1\x9E\x6A\xFF\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF1\x9E\x6A\xFF\xFB\xAD\x7F\xFF\xFB\xBF\x99\xFF\xF8\xBE\x97\xFF\xF6\xBD\x95\xFF\xF5\xBC\x95\xFF\xEF\x9C\x68\xFF\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF\x9C\x68\xFF\xF9\xAB\x7D\xFF\xF9\xBE\x97\xFF\xF4\xBB\x93\xFF\xF0\xB9\x90\xFF\xFE\xB8\x8F\xFF\xFD\xB8\x8E\xFF\xED\x9A\x66\xFF\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFD\x9A\x66\xFF\xF1\xA6\x76\xFF\xF2\xBA\x92\xFF\xFD\xB8\x8E\xFF\xF8\xB5\x8A\xFF\xF6\xB4\x88\xFF\xF4\xB3\x87\xFF\xEC\x99\x65\xFF\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFB\x98\x64\xFF\xFC\xA3\x72\xFF\xF7\xB4\x89\xFF\xF1\xB1\x84\xFF\xFE\xAF\x82\xFF\xFC\xAE\x80\xFF\xEA\x97\x63\xFF\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF9\x96\x62\xFF\xF6\x9F\x6D\xFF\xFB\xAE\x80\xFF\xF6\xAB\x7C\xFF\xF4\xAA\x7A\xFF\xE8\x95\x61\xFF\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF7\x94\x60\xFF\xF1\x9B\x68\xFF\xFF\xA7\x76\xFF\xFC\xA5\x73\xFF\xE6\x93\x5F\xFF\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF5\x92\x5E\xFF\xFC\x97\x64\xFF\xF4\xA1\x6D\xFF\xE4\x91\x5D\xFF\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF2\x8F\x5B\xFF\xFA\x96\x63\xFF\xE2\x8F\x5B\xFF\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF0\x8D\x59\xFF\xE0\x8D\x59\xFF\x00\x00\x00\x00\x00\x00\x00\x00'''

# standard npp markers - needed for cheating npp
NPP_MARK_BOOKMARK = 24
NPP_MARK_BOOKMARK_MASK = 1<<24
NPP_MARK_HIDELINESBEGIN_MASK = 1<<23
NPP_MARK_HIDELINESEND_MASK = 1<<22
NPP_MARK_HIDELINESUNDERLINE_MASK = 1<<21

# define different ids otherwise npp
# would jump in and act accordingly
MARK_HIDELINESBEGIN = 15
MARK_HIDELINESEND = 14
MARK_HIDELINESUNDERLINE = 13

MARK_HIDELINESBEGIN_MASK = 1<<MARK_HIDELINESBEGIN
MARK_HIDELINESEND_MASK = 1<<MARK_HIDELINESEND
MARK_HIDELINESUNDERLINE_MASK = 1<<MARK_HIDELINESUNDERLINE
MARK_COMBINED_MASK = MARK_HIDELINESBEGIN_MASK+MARK_HIDELINESEND_MASK

editor.setMarginMaskN(1,(MARK_HIDELINESBEGIN_MASK
                        |MARK_HIDELINESEND_MASK
                        |MARK_HIDELINESUNDERLINE_MASK
                        |NPP_MARK_BOOKMARK_MASK
                        |NPP_MARK_HIDELINESBEGIN_MASK
                        |NPP_MARK_HIDELINESEND_MASK
                        |NPP_MARK_HIDELINESUNDERLINE_MASK))

editor.rGBAImageSetWidth(14)
editor.rGBAImageSetHeight(14)
editor.markerDefineRGBAImage(MARK_HIDELINESBEGIN, hidelines_begin14)
editor.markerDefineRGBAImage(MARK_HIDELINESEND, hidelines_end14)
editor.markerDefine(MARK_HIDELINESUNDERLINE, MARKERSYMBOL.UNDERLINE)
editor.markerSetBack(MARK_HIDELINESUNDERLINE, (0xF7,0xCC,0x77))


# ---------------------------------------------------------------------------------
# helper functions
# toggle_bookmark is only needed to cheat npp
def toggle_bookmark(current_line):
    current_marker_mask = editor.markerGet(current_line)
    if (current_marker_mask & NPP_MARK_BOOKMARK_MASK) == NPP_MARK_BOOKMARK_MASK:
        editor.markerDelete(current_line, NPP_MARK_BOOKMARK)
    else:
        editor.markerAdd(current_line,NPP_MARK_BOOKMARK)


def rehide_lines(_start_line, _end_line):
    # check if we revealed new hide markers,
    # if so, we need to hide it again
    if _start_line >= 0:
        _next_marker = editor.markerNext(_start_line,MARK_HIDELINESBEGIN_MASK)
        while _start_line <= _next_marker < _end_line:
            _matching_marker = find_correspondig_marker(_next_marker, MARK_HIDELINESEND_MASK)
            editor.hideLines(_next_marker+1, _matching_marker-1)
            _next_marker = editor.markerNext(_matching_marker,MARK_HIDELINESBEGIN_MASK)


def show_lines(start_line, end_line):
    # as showLines shows folded lines as well
    # we need to check if there are
    # contracted fold levels, if so,
    # store in a list for later purpose
    folded_lines_list = []
    folded_line = editor.contractedFoldNext(start_line)

    while (folded_line != -1) and (folded_line <= end_line):
        folded_lines_list.append(folded_line)
        folded_line = editor.contractedFoldNext(folded_line+1)

    # delete the current markers and show the lines
    editor.markerDelete(start_line, MARK_HIDELINESBEGIN)
    editor.markerDelete(start_line, MARK_HIDELINESUNDERLINE)
    editor.markerDelete(end_line, MARK_HIDELINESEND)
    editor.showLines(start_line, end_line)

    # if there were contracted lines, refold it
    for folded_line in folded_lines_list:
        editor.foldLine(folded_line, FOLDACTION._CONTRACT)

    # if there were hidden lines underneath - hide it again
    rehide_lines(start_line, end_line)


def find_correspondig_marker(next_marker_line, searching_marker_mask):

    start_marker_count = 0
    end_marker_count = 0

    # to avoid having two functions, markerNext or markerPrevious
    # gets assigned to find_marker variable
    if searching_marker_mask == MARK_HIDELINESEND_MASK:
        find_marker = editor.markerNext
        add_to_line_value = 1
        start_marker_count = 1
    else:
        find_marker = editor.markerPrevious
        add_to_line_value = -1
        end_marker_count = 1

    # the idea is to search as long for start and end markers
    # until we get the same amount for both
    while (start_marker_count != end_marker_count):

        next_marker_line = find_marker(next_marker_line+add_to_line_value, MARK_COMBINED_MASK)

        if (next_marker_line != -1):
            if (editor.markerGet(next_marker_line) & MARK_COMBINED_MASK) == MARK_COMBINED_MASK:

                if searching_marker_mask == MARK_HIDELINESEND_MASK:
                    end_marker_count += 1
                    if end_marker_count == start_marker_count:
                        break # found the matching marker
                    start_marker_count += 1
                else:
                    start_marker_count += 1
                    if end_marker_count == start_marker_count:
                        break # found the matching marker
                    end_marker_count += 1

            elif editor.markerGet(next_marker_line) & MARK_HIDELINESBEGIN_MASK:
                start_marker_count += 1

            elif editor.markerGet(next_marker_line) & MARK_HIDELINESEND_MASK:
                end_marker_count += 1

        else:
            msg = 'Now we are in trouble - should not happen !! ??'
            notepad.messageBox(msg, 'ERROR')
            list_all_markers() # only used for debugging purpose
            next_marker_line = -1
            break

    return next_marker_line


def list_all_markers():
    # used only for debugging purpose
    for i in range(editor.getLineCount()):
        _marker = editor.markerGet(i)
        if _marker > 0:
            console.writeError('{:>3} = {}\n'.format(i, _marker))


# ---------------------------------------------------------------------------------
## callback area
def callback_BUFFERACTIVATED(args):
    # switching the buffer results in showing already hidden lines
    # so we need to rehide if necessary
    _next_marker_line = editor.markerNext(0,MARK_HIDELINESBEGIN_MASK)
    while _next_marker_line != -1:
        _matching_marker_line = find_correspondig_marker(_next_marker_line, MARK_HIDELINESEND_MASK)
        editor.hideLines(_next_marker_line+1, _matching_marker_line-1)
        _next_marker_line = editor.markerNext(_matching_marker_line,MARK_HIDELINESBEGIN_MASK)


# I guess having two marginclick callbacks isn't necessary for npp internally
def sync_callback_MARGINCLICK(args):
    if args['margin'] == 1: # symbol margin
        clicked_line = editor.lineFromPosition(args['position'])
        current_marker_mask = editor.markerGet(clicked_line)

        # if clicked on a opening marker - find corresponding end marker
        if (current_marker_mask & MARK_HIDELINESBEGIN_MASK) == MARK_HIDELINESBEGIN_MASK:
            end_line = find_correspondig_marker(clicked_line, MARK_HIDELINESEND_MASK)
            show_lines(clicked_line, end_line)
        # else if clicked on a closing marker, then find corresponding start marker
        elif (current_marker_mask & MARK_HIDELINESEND_MASK) == MARK_HIDELINESEND_MASK:
            start_line = find_correspondig_marker(clicked_line, MARK_HIDELINESBEGIN_MASK)
            show_lines(start_line, clicked_line)
        else: # no hiding marker, just bookmark
            toggle_bookmark(clicked_line) # cheating npp ;-)

        toggle_bookmark(clicked_line)    # double cheating npp :D

def async_callback_MARGINCLICK(args):
    # as unfold hasn't finished yet
    # we need asynchronous call to be
    # able to rehide if needed,
    if args['margin'] == 2: # folding margin
        clicked_line = editor.lineFromPosition(args['position'])
        if editor.getFoldExpanded(clicked_line):
            rehide_lines(clicked_line, editor.getLineCount())


# ---------------------------------------------------------------------------------
## main
def main():
    if editor.getSelectionEmpty():
        # user hasn't selected anything, hide cursor line
        start_line = end_line = editor.lineFromPosition(editor.getCurrentPos())
    else:
        start_line, end_line = editor.getUserLineSelection()

    total_number_less_one_line = editor.getLineCount()-1 # zero-based

    # recalculate which lines to hide as first and last line cannot be hide
    start_line = start_line if start_line > 0 else 1
    end_line = end_line if total_number_less_one_line > end_line else end_line-1

    # calculate at which lines marker need to be placed
    marker_start_line = start_line-1 if start_line > 0 else start_line
    marker_end_line = end_line+1 if end_line < total_number_less_one_line else end_line

    # recalculate in case that marker(start/end)lines are not visible
    # either because they are part of a folding tree or already hidden
    while not editor.getLineVisible(marker_start_line):
        marker_start_line -= 1

    if not editor.getLineVisible(marker_end_line):
        visible_line = editor.visibleFromDocLine(marker_end_line)
        marker_end_line = editor.docLineFromVisible(visible_line)

    # check if there is already a marker set at those lines
    marker_at_marker_start_line = editor.markerGet(marker_start_line)
    marker_at_marker_end_line = editor.markerGet(marker_end_line)


    marker_already_set = False
    if (marker_at_marker_start_line & MARK_HIDELINESBEGIN_MASK) == MARK_HIDELINESBEGIN_MASK:
        marker_type = 'start'
        marker_already_set = True

    elif (marker_at_marker_end_line & MARK_HIDELINESEND_MASK) == MARK_HIDELINESEND_MASK:
        marker_type = 'end'
        marker_already_set = True

    # already markers set - inform user
    if marker_already_set:

        if EXTEND_AUTOMATICALLY is False:
            answer = notepad.messageBox(('There can only be one {} marker per line\r\n'
                                         'Should it be extended instead?\r\n'
                                         "If it shouldn't, it doesn't do anything").format(marker_type), 'Info!',4)

        if EXTEND_AUTOMATICALLY or answer == MESSAGEBOXFLAGS.RESULTYES:
            if marker_type == 'start':
                _matching_marker_line = find_correspondig_marker(marker_start_line, MARK_HIDELINESEND_MASK)
                _start_marker_line_to_delete = marker_start_line
                _end_marker_line_to_delete = _matching_marker_line
            else:
                _matching_marker_line = find_correspondig_marker(marker_end_line, MARK_HIDELINESBEGIN_MASK)
                _start_marker_line_to_delete = _matching_marker_line
                _end_marker_line_to_delete = marker_end_line

            editor.markerDelete(_start_marker_line_to_delete, MARK_HIDELINESBEGIN)
            editor.markerDelete(_start_marker_line_to_delete, MARK_HIDELINESUNDERLINE)
            editor.markerDelete(_end_marker_line_to_delete, MARK_HIDELINESEND)

        else:
            return


    editor.hideLines(start_line, end_line)
    editor.markerAdd(marker_start_line,MARK_HIDELINESBEGIN)
    editor.markerAdd(marker_start_line,MARK_HIDELINESUNDERLINE)
    editor.markerAdd(marker_end_line,MARK_HIDELINESEND)
    editor.gotoLine(marker_start_line)

# ---------------------------------------------------------------------------------

try:
    MARGINCLICK_CALLBACK_REGISTERED
except NameError:
    editor.callbackSync(sync_callback_MARGINCLICK, [SCINTILLANOTIFICATION.MARGINCLICK])
    editor.callback(async_callback_MARGINCLICK, [SCINTILLANOTIFICATION.MARGINCLICK])
    notepad.callback(callback_BUFFERACTIVATED, [NOTIFICATION.BUFFERACTIVATED])
    MARGINCLICK_CALLBACK_REGISTERED = True

main()