# -*- coding: utf-8 -*-
"""
   Script:   GotoLineCol.py
   Utility:  1. Moves the cursor position to the specified line and column for a file in Notepad++.
                Especially useful for inspecting data files in fixed-width record formats.
             2. Also, displays the character code (SBCS & LTR) in decimal and hex at the specified position.
   Requires: Python Script plugin in Notepad++

   Customizable parameters for the goToLineCol function call in main():
             bRepeatPrompt: Whether to repeat prompting when the specified number value is out of range
               iEdgeBuffer: Ensures that the caret will be that many characters inside the left and right edges of the editor viewing area, when possible
      iCaretHiliteDuration: Caret will be in Block mode for specified seconds
          bCallTipAutoHide: Whether to hide the call tip automatically in sync when caret highlighting is turned off
              bBraceHilite: Whether to use brace highlighting style for the character at the specified position. Automatically turns off when current line changes.

Known Issues: 1. Character code display in the call tip is functional with SBCS (Single-Byte Character Sets) and LTR (left-to-right) direction.
                 With MBCS (Bulti-Bytes Character Sets) or RTL (right-to-left) direction, results will not be reliable.
              2. If iCaretHiliteDuration is set to a high value (>3 seconds), and the user tries to rerun the script
                 while the previous execution is still running, the Python Script plugin will display an error message:
                 "Another script is still running..." So set this parameter to 3 seconds or lower.

   Author:   Shridhar Kumar
   Date:     2019-08-15
"""

def main():
   goToLineCol(bRepeatPrompt = True,
               iEdgeBuffer = 5,
               iCaretHiliteDuration = 5,
               bCallTipAutoHide = False,
               bBraceHilite = True)


def getDisplayLineCol():
   iCurrLine = editor.lineFromPosition(editor.getCurrentPos())
   iCurrCol = editor.getCurrentPos() - editor.positionFromLine(iCurrLine)
   return str(iCurrLine + 1), str(iCurrCol + 1)

def promptValue(sInfoText, sTitleText, sDefaultVal, iMinVal, iMaxVal, sRangeError, bRepeatPrompt):
   while True:
      sNewVal = notepad.prompt(sInfoText, sTitleText, sDefaultVal)
      if sNewVal == None:
         return None

      try:
         iNewVal = int(sNewVal)
         if iMinVal <= iNewVal <= iMaxVal:
            return iNewVal
         else:
            raise
      except:
         notepad.messageBox(sRangeError + '.\n\nYou specified: ' + sNewVal +
                              '\n\nPlease specify a number between ' + str(iMinVal) + ' and ' + str(iMaxVal) + '.',
                              'Specified value is out of range')
         if not bRepeatPrompt:
            return None


def goToLineCol(bRepeatPrompt, iEdgeBuffer, iCaretHiliteDuration, bCallTipAutoHide, bBraceHilite):
   import time

   sCurrLine, sCurrCol = getDisplayLineCol()
   iMaxLines = editor.getLineCount()

   iNewLine = promptValue(sInfoText = 'Line number (between 1 and ' + str(iMaxLines) + '):',
                           sTitleText = 'Specify line number',
                           sDefaultVal = sCurrLine,
                           iMinVal = 1,
                           iMaxVal = iMaxLines,
                           sRangeError = 'File line count is only ' + str(iMaxLines),
                           bRepeatPrompt = bRepeatPrompt)
   if iNewLine == None:
      return

   # Get the character count plus 1 for the specified line
   # Plus 1 is to account for the caret position at the end of the line, past all characters but before EOL/EOF
   # Since lineLength already includes EOL, we just need to subtract 1 only when EOL is 2 chars. i.e., CRLF
   # For the last line in file, there is no 2-character CRLF EOL; only a single character EOF.
   iMaxCols = max(1, editor.lineLength(iNewLine - 1))
   if (editor.getEOLMode() == ENDOFLINE.CRLF) and (iNewLine < iMaxLines):
      iMaxCols -= 1

   iNewCol = promptValue(sInfoText = 'Column position (between 1 and ' + str(iMaxCols) +  ') for line ' + str(iNewLine) + ':',
                           sTitleText = 'Specify column position',
                           sDefaultVal = sCurrCol,
                           iMinVal = 1,
                           iMaxVal = iMaxCols,
                           sRangeError = 'There are only ' + str(iMaxCols) + ' characters in line ' + str(iNewLine),
                           bRepeatPrompt = bRepeatPrompt)

   # Navigate to the specified position in the document
   iLineStartPos = editor.positionFromLine(iNewLine - 1)
   iNewPos = iLineStartPos + iNewCol - 1
   editor.ensureVisible(iNewLine - 1)
   editor.gotoPos( min(iLineStartPos + iMaxCols, iNewPos + iEdgeBuffer) )    # Ensure that caret is 'iEdgeBuffer' characters inside right edge when possible
   editor.gotoPos( max(iLineStartPos, iNewPos - iEdgeBuffer) )               # Ensure that caret is 'iEdgeBuffer' characters inside left edge when possible
   editor.gotoPos(iNewPos)                                                   # Finally, move caret to the specified position

   # Obtain current caret style to restore it later on
   currCS = editor.getCaretStyle()

   # Set the caret to block style to highlight the new position
   editor.setCaretStyle(CARETSTYLE.BLOCK)

   # Display a call tip with the new line and column numbers with verification
   # Also display the character code in decimal and hex
   sCurrLine, sCurrCol = getDisplayLineCol()
   editor.callTipShow(iNewPos, '     Line: ' + sCurrLine +
                             '\n   Column: ' + sCurrCol +
                             '\nChar Code: ' + str(editor.getCharAt(iNewPos)) + ' [' + hex(editor.getCharAt(iNewPos)) + ']')

   if iCaretHiliteDuration > 0:
      time.sleep(iCaretHiliteDuration)

   # Reset the caret style
   editor.setCaretStyle(currCS)

   if bCallTipAutoHide:
      editor.callTipCancel()

   if bBraceHilite:
      editor.braceHighlight(iNewPos, iNewPos)


main()
