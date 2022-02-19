# -*- coding: utf-8 -*-
"""
    RegexTester

    Regular expression tester

    Usage:
        Every execution toggles the run status - indicated by the status line.

        The REGEX_TESTER_FILE_NAME is created within PythonScript\scripts directory
        and is called RegexTester.txt. It stores the used regular expressions
        If it should be changed, modify the REGEX_TESTER_FILE_NAME variable below.

        The COMMENT_CHAR can be changed, if needed, but if the regex
        begins with a COMMENT_CHAR, it must be escaped to make the whole regex working.
        ( For instance the regex \#.* colours all comment lines of a python file )

        Per default an indicator (COLOR_NO_MATCHES_FOUND) is used to inform about no matches.
        To disable it, INFORM_ABOUT_NO_MATCHES flag needs to be set to False

        Regular expression can lead to zero length matches, which basically means that there
        is nothing to colour. Per default it is set to ignore these matches, if it should be changed
        IGNORE_0_LENGTH_MATCHES flag needs to be set to False

        Used indicator colours might be changed depending on used theme.
"""
from Npp import editor, editor1, editor2, notepad, console
from Npp import SCINTILLANOTIFICATION, NOTIFICATION, KEYMOD, MENUCOMMAND, INDICATORSTYLE
import os
import re

# ---------------------------- <CONFIGURATION AREA> ------------------------------
REGEX_TESTER_FILE_NAME    = os.path.join(notepad.getPluginConfigDir(), r'PythonScript\scripts', 'RegexTester.txt')
COLOR_MAIN_MATCH          = (100,215,100)
COLOR_ODD_MATCH           = (95,215,184)
COLOR_EVEN_MATCH          = (195,215,184)
COLOR_INVALID_REGEX       = (255,0,0)
COLOR_NO_MATCHES_FOUND    = (255,120,0)
COLOR_REGEX_TESTER_STATUS = (0,255,0)
COMMENT_CHAR              = '#'
INFORM_ABOUT_NO_MATCHES   = True
IGNORE_0_LENGTH_MATCHES   = True
# ---------------------------- </CONFIGURATION AREA> -----------------------------

_g = globals()
REGEX_TESTER_IS_RUNNING = _g.get('REGEX_TESTER_IS_RUNNING', False)
REGEX_TESTER_INPUT_TAB = _g.get('REGEX_TESTER_INPUT_TAB', 0)
COLORED_DOCS_LIST = _g.get('COLORED_DOCS_LIST', [])

# --------------------------------------------------------------------------------

editor1.indicSetStyle(8,INDICATORSTYLE.FULLBOX)
editor1.indicSetFore(8,COLOR_MAIN_MATCH)
editor1.indicSetAlpha(8,85)
editor1.indicSetOutlineAlpha(8,85)
editor1.indicSetUnder(8,True)

editor1.indicSetStyle(9,INDICATORSTYLE.ROUNDBOX)
editor1.indicSetFore(9,COLOR_ODD_MATCH)
editor1.indicSetAlpha(9,50)
editor1.indicSetOutlineAlpha(9,255)
editor1.indicSetUnder(9,True)

editor1.indicSetStyle(10,INDICATORSTYLE.ROUNDBOX)
editor1.indicSetFore(10,COLOR_EVEN_MATCH)
editor1.indicSetAlpha(10,50)
editor1.indicSetOutlineAlpha(10,255)
editor1.indicSetUnder(10,True)

editor2.indicSetStyle(11,INDICATORSTYLE.STRAIGHTBOX)
editor2.indicSetFore(11,COLOR_INVALID_REGEX)
editor2.indicSetAlpha(11,55)
editor2.indicSetOutlineAlpha(11,255)
editor2.indicSetUnder(11,True)

editor2.indicSetStyle(12,INDICATORSTYLE.STRAIGHTBOX)
editor2.indicSetFore(12,COLOR_NO_MATCHES_FOUND)
editor2.indicSetAlpha(12,55)
editor2.indicSetOutlineAlpha(12,255)
editor2.indicSetUnder(12,True)

editor2.indicSetStyle(13,INDICATORSTYLE.STRAIGHTBOX)
editor2.indicSetFore(13,COLOR_REGEX_TESTER_STATUS)
editor2.indicSetAlpha(13,55)
editor2.indicSetOutlineAlpha(13,255)
editor2.indicSetUnder(13,True)

# --------------------------------------------------------------------------------

IS_ODD = False
PREVIOUS_REGEX = {}
NO_MATCH_FOUND = True
MATCH_POSITIONS = {}

CURRENT_BUFFER_ID = None
LAST_POSITION = -1
FORWARD_SEARCH = 0
BACKWARD_SEARCH = 1
REGEX_LINE_CLEAR = 0
REGEX_LINE_NO_MATCH = 1
REGEX_LINE_INVALID = -1

# --------------------------------------------------------------------------------

def match_found(m):
    _m = m.span()
    if IGNORE_0_LENGTH_MATCHES and (_m[0] == _m[1]):
        return

    global IS_ODD
    global MATCH_POSITIONS
    global NO_MATCH_FOUND

    _match_positions = []
    if REPORT_MATCHES:
        console.show()
        editor.grabFocus()
        console.write('Match:{} = {}\n'.format(_m, editor1.getTextRange(*_m)))

    if m.lastindex > 0:
        editor1.setIndicatorCurrent(8)
        editor1.indicatorFillRange(m.span(0)[0], m.span(0)[1] - m.span(0)[0])
        _match_positions.append(m.span())
        for i in range(1, m.lastindex + 1):
            if (m.span(i)[0] != m.span(0)[0]) or (m.span(i)[1] != m.span(0)[1]):
                editor1.setIndicatorCurrent(9 if IS_ODD else 10)
                editor1.indicatorFillRange(m.span(i)[0], m.span(i)[1] - m.span(i)[0])

                IS_ODD = not IS_ODD
                if REPORT_MATCHES:
                    _textrange = editor1.getTextRange(*m.span(i)) if -1 not in m.span(i) else ''
                    console.write('  SubMatch:{} = {}\n'.format(m.span(i), _textrange))

    else:
        editor1.setIndicatorCurrent(9 if IS_ODD else 10)
        editor1.indicatorFillRange(_m[0], _m[1] - _m[0])

        IS_ODD = not IS_ODD
        _match_positions.append(m.span())

    MATCH_POSITIONS[CURRENT_BUFFER_ID].extend(_match_positions)

    NO_MATCH_FOUND = False

# --------------------------------------------------------------------------------

def set_current_buffer_id():
    global CURRENT_BUFFER_ID
    doc_idx = notepad.getCurrentDocIndex(0)
    CURRENT_BUFFER_ID = notepad.getFiles()[doc_idx][1]

# --------------------------------------------------------------------------------

def track_document():
    global COLORED_DOCS_LIST
    set_current_buffer_id()
    if not CURRENT_BUFFER_ID in COLORED_DOCS_LIST:
        COLORED_DOCS_LIST.append(CURRENT_BUFFER_ID)

# --------------------------------------------------------------------------------

def clear_indicator():
    for i in [8,9,10]:
        editor1.setIndicatorCurrent(i)
        editor1.indicatorClearRange(0,editor1.getTextLength())

# --------------------------------------------------------------------------------

def get_regex_flags():
    global TIME_REGEX
    global USE_PYTHON_ENGINE
    global REPORT_MATCHES
    TIME_REGEX = False
    USE_PYTHON_ENGINE = False
    REPORT_MATCHES = False

    flags = 0
    flag_line = editor2.getLine(0)
    for char in range(flag_line.find('[')+1,flag_line.find(']')):
        if flag_line[char].upper() == 'I':
            flags |= re.IGNORECASE
        elif flag_line[char].upper() == 'L':
            flags |= re.LOCALE
        elif flag_line[char].upper() == 'M':
            flags |= re.MULTILINE
        elif flag_line[char].upper() == 'S':
            flags |= re.DOTALL
        elif flag_line[char].upper() == 'U':
            flags |= re.UNICODE
        elif flag_line[char].upper() == 'X':
            flags |= re.VERBOSE

        elif flag_line[char].upper() == 'T':
            TIME_REGEX = True
        elif flag_line[char].upper() == 'P':
            USE_PYTHON_ENGINE = True
        elif flag_line[char].upper() == 'R':
            REPORT_MATCHES = True
    return flags

# --------------------------------------------------------------------------------

def regex_exec(pattern, flags):
    try:
        matches = []
        for i in range(1):
            editor1.research(pattern, lambda m: matches.append(m), flags)
        console.write('matches found:{}\n'.format(len(matches)))
        return True
    except:
        return False

def regex():
    global PREVIOUS_REGEX
    global NO_MATCH_FOUND
    global MATCH_POSITIONS
    global IS_ODD

    IS_ODD = False
    MATCH_POSITIONS[CURRENT_BUFFER_ID] = []

    clear_indicator()
    pattern = ''

    if (editor2.getSelections() == 1) and (editor2.getSelectionEmpty() == False):
        start = editor2.lineFromPosition(editor2.getSelectionNStart(0))
        end = editor2.lineFromPosition(editor2.getSelectionNEnd(0))
        for i in range(start,end+1):
            pattern += editor2.getLine(i).rstrip('\r\n')
    else:
        pattern = current_regex()

    PREVIOUS_REGEX[CURRENT_BUFFER_ID] = pattern
    NO_MATCH_FOUND = True

    if (pattern != '' and
        pattern != '.' and
        pattern != '()' and not
        pattern.startswith(COMMENT_CHAR) and not
        pattern.isspace() and
        re.match('^\(\?[mis-]*\)$',pattern) is None and
        editor2.lineFromPosition(editor2.getCurrentPos()) > 0):

        regex_flag = get_regex_flags()

        if TIME_REGEX:
            if regex_exec(pattern,regex_flag):
                track_document()
            else:
                NO_MATCH_FOUND = None
        else:
            try:
                if USE_PYTHON_ENGINE:
                    map(match_found, re.finditer(pattern, editor1.getText(), regex_flag))
                else:
                    editor1.research(pattern, match_found, regex_flag)
                track_document()
            except Exception as e:
                console.write('Exception:{}\n'.format(e))
                NO_MATCH_FOUND = None

    else:
        NO_MATCH_FOUND = None

# --------------------------------------------------------------------------------

def current_regex():
    return editor2.getCurLine().rstrip()

# --------------------------------------------------------------------------------

def regex_tester_doc_is_current_doc():
    return REGEX_TESTER_INPUT_TAB == notepad.getCurrentBufferID()

# --------------------------------------------------------------------------------

def get_current_regex_position_and_length():
    current_line = editor2.lineFromPosition(editor2.getCurrentPos())
    if current_line != 0 and not editor2.getLine(current_line).startswith(COMMENT_CHAR):
        lenght_line = editor2.lineLength(current_line)
        position = editor2.positionFromLine(current_line)
        return position, lenght_line
    return None, None

# --------------------------------------------------------------------------------

def mark_regex_line(match_indicator):
    position, lenght = get_current_regex_position_and_length()
    if position:
        if match_indicator == REGEX_LINE_NO_MATCH:
            editor2.setIndicatorCurrent(12)
            editor2.indicatorFillRange(position, lenght)
        elif match_indicator == REGEX_LINE_INVALID:
            editor2.setIndicatorCurrent(11)
            editor2.indicatorFillRange(position, lenght)
        else:
            for i in [11,12]:
                editor2.setIndicatorCurrent(i)
                editor2.indicatorClearRange(0, editor2.getTextLength())

# --------------------------------------------------------------------------------

def regex_tester_updateui_callback(args):
    if (regex_tester_doc_is_current_doc() and PREVIOUS_REGEX.get(CURRENT_BUFFER_ID, '') != current_regex()):
        mark_regex_line(REGEX_LINE_CLEAR)
        regex()
        if NO_MATCH_FOUND and INFORM_ABOUT_NO_MATCHES:
            mark_regex_line(REGEX_LINE_NO_MATCH)
        elif NO_MATCH_FOUND is None:
            mark_regex_line(REGEX_LINE_INVALID)

# --------------------------------------------------------------------------------

def regex_tester_file_before_close_callback(args):
    if (args['bufferID'] == REGEX_TESTER_INPUT_TAB):
        stop_regex_tester()

# --------------------------------------------------------------------------------

def regex_tester_buffer_activated_callback(args):
    global PREVIOUS_REGEX
    set_current_buffer_id()

    if not CURRENT_BUFFER_ID in PREVIOUS_REGEX:
        PREVIOUS_REGEX[CURRENT_BUFFER_ID] = ''

    if not REGEX_TESTER_IS_RUNNING:
        global COLORED_DOCS_LIST
        if args['bufferID'] in COLORED_DOCS_LIST:
            clear_indicator()
            COLORED_DOCS_LIST.remove(args['bufferID'])

        if len(COLORED_DOCS_LIST) == 0:
            notepad.clearCallbacks([NOTIFICATION.BUFFERACTIVATED])

# --------------------------------------------------------------------------------

def regex_tester_doc_already_exists():
    files = notepad.getFiles()
    f = [i[1][1] for i in enumerate(files) if i[1][0].upper() == REGEX_TESTER_FILE_NAME.upper()]
    if f:
        return f[0]
    else:
        return 0

# --------------------------------------------------------------------------------

def color_regex_tester_status():
    status_line = editor2.getLine(0)
    if REGEX_TESTER_IS_RUNNING:
        start = status_line.find('[')
        stop = status_line.find(']')+1
        editor2.setIndicatorCurrent(13)
        editor2.indicatorFillRange(0,20)
        editor2.setIndicatorCurrent(13)
        editor2.indicatorFillRange(start,stop-start)
    else:
        editor2.setIndicatorCurrent(13)
        editor2.indicatorClearRange(0,len(status_line))

# --------------------------------------------------------------------------------

def start_regex_tester():
    if REGEX_TESTER_FILE_NAME == '':
        notepad.messageBox('You need to indicate, in the REGEX_TESTER_FILE_NAME variable,\n' +
                           'the fully qualified file name of the TEXT file containing ' +
                           'the REGEX(ES) to test', 'REGEX_TESTER_FILE_NAME is not set')
        return False

    current_document = 0 if notepad.getCurrentView() == 1 else notepad.getCurrentBufferID()

    global REGEX_TESTER_INPUT_TAB
    REGEX_TESTER_INPUT_TAB = regex_tester_doc_already_exists()
    if REGEX_TESTER_INPUT_TAB == 0 :
        notepad.open(REGEX_TESTER_FILE_NAME)
        if notepad.getCurrentFilename().upper() == REGEX_TESTER_FILE_NAME.upper():
            REGEX_TESTER_INPUT_TAB = notepad.getCurrentBufferID()
        else:
            notepad.messageBox('Could not open specified file\n' +
                               '{0}'.format(REGEX_TESTER_FILE_NAME),
                               'Regex Tester Startup Failed', 0)
            return False
    else:
        notepad.activateBufferID(REGEX_TESTER_INPUT_TAB)

    if notepad.getCurrentView() != 1:
        notepad.menuCommand(MENUCOMMAND.VIEW_GOTO_ANOTHER_VIEW)

    STATUS_LINE = 'RegexTester isActive [] flags:sitpr'

    current_status_line = editor2.getLine(0)
    if 'RegexTester' in current_status_line:
        editor2.replace('RegexTester inActive', 'RegexTester isActive')
    else:
        editor2.insertText(0, STATUS_LINE)


    global REGEX_TESTER_IS_RUNNING
    REGEX_TESTER_IS_RUNNING = True

    color_regex_tester_status()

    set_current_buffer_id()

    global PREVIOUS_REGEX
    PREVIOUS_REGEX[CURRENT_BUFFER_ID] = ''

    editor.callbackSync(regex_tester_updateui_callback, [SCINTILLANOTIFICATION.UPDATEUI])

    if current_document != 0:
        notepad.activateBufferID(current_document)

    editor2.setFocus(True)
    editor2.gotoLine(0)
    notepad.save()

    notepad.callback(regex_tester_file_before_close_callback, [NOTIFICATION.FILEBEFORECLOSE])
    notepad.callback(regex_tester_buffer_activated_callback, [NOTIFICATION.BUFFERACTIVATED])

    return True

# --------------------------------------------------------------------------------

def stop_regex_tester():
    editor.clearCallbacks([SCINTILLANOTIFICATION.UPDATEUI])
    notepad.clearCallbacks([NOTIFICATION.FILEBEFORECLOSE])

    notepad.activateBufferID(REGEX_TESTER_INPUT_TAB)

    if regex_tester_doc_is_current_doc():
        editor2.replace('RegexTester isActive', 'RegexTester inActive')
        notepad.save()

    global REGEX_TESTER_IS_RUNNING
    REGEX_TESTER_IS_RUNNING = False

    clear_indicator()
    color_regex_tester_status()
    mark_regex_line(REGEX_LINE_CLEAR)

    global MATCH_POSITIONS
    MATCH_POSITIONS = {}

    editor1.setFocus(True)

# --------------------------------------------------------------------------------

def main():
    if REGEX_TESTER_IS_RUNNING:
        stop_regex_tester()
    else:
        if start_regex_tester():

            # disable control chars in editor2
            editor2.assignCmdKey(ord('E') + (KEYMOD.CTRL << 16),2172)
            editor2.assignCmdKey(ord('R') + (KEYMOD.CTRL << 16),2172)
            editor2.assignCmdKey(ord('Y') + (KEYMOD.SHIFT + KEYMOD.CTRL << 16),2172)
            editor2.assignCmdKey(ord('W') + (KEYMOD.SHIFT + KEYMOD.CTRL << 16),2172)
            editor2.assignCmdKey(ord('O') + (KEYMOD.SHIFT + KEYMOD.CTRL << 16),2172)
            editor2.assignCmdKey(ord('A') + (KEYMOD.SHIFT + KEYMOD.CTRL << 16),2172)
            editor2.assignCmdKey(ord('D') + (KEYMOD.SHIFT + KEYMOD.CTRL << 16),2172)
            editor2.assignCmdKey(ord('G') + (KEYMOD.SHIFT + KEYMOD.CTRL << 16),2172)
            editor2.assignCmdKey(ord('H') + (KEYMOD.SHIFT + KEYMOD.CTRL << 16),2172)
            editor2.assignCmdKey(ord('Z') + (KEYMOD.SHIFT + KEYMOD.CTRL << 16),2172)
            editor2.assignCmdKey(ord('X') + (KEYMOD.SHIFT + KEYMOD.CTRL << 16),2172)
            editor2.assignCmdKey(ord('C') + (KEYMOD.SHIFT + KEYMOD.CTRL << 16),2172)
            editor2.assignCmdKey(ord('V') + (KEYMOD.SHIFT + KEYMOD.CTRL << 16),2172)
            editor2.assignCmdKey(ord('B') + (KEYMOD.SHIFT + KEYMOD.CTRL << 16),2172)
            editor2.assignCmdKey(ord('N') + (KEYMOD.SHIFT + KEYMOD.CTRL << 16),2172)
            editor2.assignCmdKey(ord('6') + (KEYMOD.SHIFT + KEYMOD.CTRL << 16),2172)
            editor2.assignCmdKey(189 + (KEYMOD.SHIFT + KEYMOD.CTRL << 16),2172)
            editor2.assignCmdKey(7 + (KEYMOD.SHIFT << 16),2172)
            editor2.assignCmdKey(3 + (KEYMOD.CTRL << 16),2172)

# --------------------------------------------------------------------------------

main()
