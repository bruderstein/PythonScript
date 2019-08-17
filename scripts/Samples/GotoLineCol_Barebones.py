curLine = editor.lineFromPosition(editor.getCurrentPos())

line = int( notepad.prompt('Line Number:', 'Navigate to a line and character position', str(curLine + 1) ))
pos = int( notepad.prompt('Character Position:', 'Navigate to a line and character position', str( editor.getCurrentPos() - editor.positionFromLine(curLine) + 1 ) ))

editor.gotoPos(editor.positionFromLine(line - 1) + pos - 1)
