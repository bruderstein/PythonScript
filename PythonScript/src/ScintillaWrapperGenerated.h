/** Add text to the document at current position.
  */
int ScintillaWrapper::AddText(str text);
/** Add array of cells to document.
  */
int ScintillaWrapper::AddStyledText(ScintillaCells c);
/** Insert string at a position.
  */
void ScintillaWrapper::InsertText(int pos, str text);
/** Delete all text in the document.
  */
void ScintillaWrapper::ClearAll();
/** Set all style bytes to 0, remove all folding information.
  */
void ScintillaWrapper::ClearDocumentStyle();
/** Returns the number of bytes in the document.
  */
int ScintillaWrapper::GetLength();
/** Returns the character byte at the position.
  */
int ScintillaWrapper::GetCharAt(int pos);
/** Returns the position of the caret.
  */
int ScintillaWrapper::GetCurrentPos();
/** Returns the position of the opposite end of the selection to the caret.
  */
int ScintillaWrapper::GetAnchor();
/** Returns the style byte at the position.
  */
int ScintillaWrapper::GetStyleAt(int pos);
/** Redoes the next action on the undo history.
  */
void ScintillaWrapper::Redo();
/** Choose between collecting actions into the undo
  * history and discarding them.
  */
void ScintillaWrapper::SetUndoCollection(bool collectUndo);
/** Select all the text in the document.
  */
void ScintillaWrapper::SelectAll();
/** Remember the current position in the undo history as the position
  * at which the document was saved.
  */
void ScintillaWrapper::SetSavePoint();
/** Retrieve a buffer of cells.
  * Returns the number of bytes in the buffer not including terminating NULs.
  */
str ScintillaWrapper::GetStyledText(int start, int end);
/** Are there any redoable actions in the undo history?
  */
bool ScintillaWrapper::CanRedo();
/** Retrieve the line number at which a particular marker is located.
  */
int ScintillaWrapper::MarkerLineFromHandle(int handle);
/** Delete a marker.
  */
void ScintillaWrapper::MarkerDeleteHandle(int handle);
/** Is undo history being collected?
  */
bool ScintillaWrapper::GetUndoCollection();
/** Are white space characters currently visible?
  * Returns one of SCWS_* constants.
  */
int ScintillaWrapper::GetViewWS();
/** Make white space characters invisible, always visible or visible outside indentation.
  */
void ScintillaWrapper::SetViewWS(int viewWS);
/** Find the position from a point within the window.
  */
int ScintillaWrapper::PositionFromPoint(int x, int y);
/** Find the position from a point within the window but return
  * INVALID_POSITION if not close to text.
  */
int ScintillaWrapper::PositionFromPointClose(int x, int y);
/** Set caret to start of a line and ensure it is visible.
  */
void ScintillaWrapper::GotoLine(int line);
/** Set caret to a position and ensure it is visible.
  */
void ScintillaWrapper::GotoPos(int pos);
/** Set the selection anchor to a position. The anchor is the opposite
  * end of the selection from the caret.
  */
void ScintillaWrapper::SetAnchor(int posAnchor);
/** Retrieve the text of the line containing the caret.
  * Returns the index of the caret on the line.
  */
str ScintillaWrapper::GetCurLine();
/** Retrieve the position of the last correctly styled character.
  */
int ScintillaWrapper::GetEndStyled();
/** Convert all line endings in the document to one mode.
  */
void ScintillaWrapper::ConvertEOLs(int eolMode);
/** Retrieve the current end of line mode - one of CRLF, CR, or LF.
  */
int ScintillaWrapper::GetEOLMode();
/** Set the current end of line mode.
  */
void ScintillaWrapper::SetEOLMode(int eolMode);
/** Set the current styling position to pos and the styling mask to mask.
  * The styling mask can be used to protect some bits in each styling byte from modification.
  */
void ScintillaWrapper::StartStyling(int pos, int mask);
/** Change style from current styling position for length characters to a style
  * and move the current styling position to after this newly styled segment.
  */
void ScintillaWrapper::SetStyling(int length, int style);
/** Is drawing done first into a buffer or direct to the screen?
  */
bool ScintillaWrapper::GetBufferedDraw();
/** If drawing is buffered then each line of text is drawn into a bitmap buffer
  * before drawing it to the screen to avoid flicker.
  */
void ScintillaWrapper::SetBufferedDraw(bool buffered);
/** Change the visible size of a tab to be a multiple of the width of a space character.
  */
void ScintillaWrapper::SetTabWidth(int tabWidth);
/** Retrieve the visible size of a tab.
  */
int ScintillaWrapper::GetTabWidth();
/** Set the code page used to interpret the bytes of the document as characters.
  * The SC_CP_UTF8 value can be used to enter Unicode mode.
  */
void ScintillaWrapper::SetCodePage(int codePage);
/** In palette mode, Scintilla uses the environment's palette calls to display
  * more colours. This may lead to ugly displays.
  */
void ScintillaWrapper::SetUsePalette(bool usePalette);
/** Set the symbol used for a particular marker number.
  */
void ScintillaWrapper::MarkerDefine(int markerNumber, int markerSymbol);
/** Set the foreground colour used for a particular marker number.
  */
void ScintillaWrapper::MarkerSetFore(int markerNumber, int foreRed, int foreGreen, int foreBlue);
/** Set the background colour used for a particular marker number.
  */
void ScintillaWrapper::MarkerSetBack(int markerNumber, int backRed, int backGreen, int backBlue);
/** Add a marker to a line, returning an ID which can be used to find or delete the marker.
  */
int ScintillaWrapper::MarkerAdd(int line, int markerNumber);
/** Delete a marker from a line.
  */
void ScintillaWrapper::MarkerDelete(int line, int markerNumber);
/** Delete all markers with a particular number from all lines.
  */
void ScintillaWrapper::MarkerDeleteAll(int markerNumber);
/** Get a bit mask of all the markers set on a line.
  */
int ScintillaWrapper::MarkerGet(int line);
/** Find the next line after lineStart that includes a marker in mask.
  */
int ScintillaWrapper::MarkerNext(int lineStart, int markerMask);
/** Find the previous line before lineStart that includes a marker in mask.
  */
int ScintillaWrapper::MarkerPrevious(int lineStart, int markerMask);
/** Define a marker from a pixmap.
  */
void ScintillaWrapper::MarkerDefinePixmap(int markerNumber, str pixmap);
/** Add a set of markers to a line.
  */
void ScintillaWrapper::MarkerAddSet(int line, int set);
/** Set the alpha used for a marker that is drawn in the text area, not the margin.
  */
void ScintillaWrapper::MarkerSetAlpha(int markerNumber, int alpha);
/** Set a margin to be either numeric or symbolic.
  */
void ScintillaWrapper::SetMarginTypeN(int margin, int marginType);
/** Retrieve the type of a margin.
  */
int ScintillaWrapper::GetMarginTypeN(int margin);
/** Set the width of a margin to a width expressed in pixels.
  */
void ScintillaWrapper::SetMarginWidthN(int margin, int pixelWidth);
/** Retrieve the width of a margin in pixels.
  */
int ScintillaWrapper::GetMarginWidthN(int margin);
/** Set a mask that determines which markers are displayed in a margin.
  */
void ScintillaWrapper::SetMarginMaskN(int margin, int mask);
/** Retrieve the marker mask of a margin.
  */
int ScintillaWrapper::GetMarginMaskN(int margin);
/** Make a margin sensitive or insensitive to mouse clicks.
  */
void ScintillaWrapper::SetMarginSensitiveN(int margin, bool sensitive);
/** Retrieve the mouse click sensitivity of a margin.
  */
bool ScintillaWrapper::GetMarginSensitiveN(int margin);
/** Clear all the styles and make equivalent to the global default style.
  */
void ScintillaWrapper::StyleClearAll();
/** Set the foreground colour of a style.
  */
void ScintillaWrapper::StyleSetFore(int style, int foreRed, int foreGreen, int foreBlue);
/** Set the background colour of a style.
  */
void ScintillaWrapper::StyleSetBack(int style, int backRed, int backGreen, int backBlue);
/** Set a style to be bold or not.
  */
void ScintillaWrapper::StyleSetBold(int style, bool bold);
/** Set a style to be italic or not.
  */
void ScintillaWrapper::StyleSetItalic(int style, bool italic);
/** Set the size of characters of a style.
  */
void ScintillaWrapper::StyleSetSize(int style, int sizePoints);
/** Set the font of a style.
  */
void ScintillaWrapper::StyleSetFont(int style, str fontName);
/** Set a style to have its end of line filled or not.
  */
void ScintillaWrapper::StyleSetEOLFilled(int style, bool filled);
/** Reset the default style to its state at startup
  */
void ScintillaWrapper::StyleResetDefault();
/** Set a style to be underlined or not.
  */
void ScintillaWrapper::StyleSetUnderline(int style, bool underline);
/** Get the foreground colour of a style.
  */
colour ScintillaWrapper::StyleGetFore(int style);
/** Get the background colour of a style.
  */
colour ScintillaWrapper::StyleGetBack(int style);
/** Get is a style bold or not.
  */
bool ScintillaWrapper::StyleGetBold(int style);
/** Get is a style italic or not.
  */
bool ScintillaWrapper::StyleGetItalic(int style);
/** Get the size of characters of a style.
  */
int ScintillaWrapper::StyleGetSize(int style);
/** Get the font of a style.
  * Returns the length of the fontName
  */
str ScintillaWrapper::StyleGetFont();
/** Get is a style to have its end of line filled or not.
  */
bool ScintillaWrapper::StyleGetEOLFilled(int style);
/** Get is a style underlined or not.
  */
bool ScintillaWrapper::StyleGetUnderline(int style);
/** Get is a style mixed case, or to force upper or lower case.
  */
int ScintillaWrapper::StyleGetCase(int style);
/** Get the character get of the font in a style.
  */
int ScintillaWrapper::StyleGetCharacterSet(int style);
/** Get is a style visible or not.
  */
bool ScintillaWrapper::StyleGetVisible(int style);
/** Get is a style changeable or not (read only).
  * Experimental feature, currently buggy.
  */
bool ScintillaWrapper::StyleGetChangeable(int style);
/** Get is a style a hotspot or not.
  */
bool ScintillaWrapper::StyleGetHotSpot(int style);
/** Set a style to be mixed case, or to force upper or lower case.
  */
void ScintillaWrapper::StyleSetCase(int style, int caseForce);
/** Set the character set of the font in a style.
  */
void ScintillaWrapper::StyleSetCharacterSet(int style, int characterSet);
/** Set a style to be a hotspot or not.
  */
void ScintillaWrapper::StyleSetHotSpot(int style, bool hotspot);
/** Set the foreground colour of the main and additional selections and whether to use this setting.
  */
void ScintillaWrapper::SetSelFore(bool useSetting, int foreRed, int foreGreen, int foreBlue);
/** Set the background colour of the main and additional selections and whether to use this setting.
  */
void ScintillaWrapper::SetSelBack(bool useSetting, int backRed, int backGreen, int backBlue);
/** Get the alpha of the selection.
  */
int ScintillaWrapper::GetSelAlpha();
/** Set the alpha of the selection.
  */
void ScintillaWrapper::SetSelAlpha(int alpha);
/** Is the selection end of line filled?
  */
bool ScintillaWrapper::GetSelEOLFilled();
/** Set the selection to have its end of line filled or not.
  */
void ScintillaWrapper::SetSelEOLFilled(bool filled);
/** Set the foreground colour of the caret.
  */
void ScintillaWrapper::SetCaretFore(int foreRed, int foreGreen, int foreBlue);
/** When key+modifier combination km is pressed perform msg.
  */
void ScintillaWrapper::AssignCmdKey(keymod km, int msg);
/** When key+modifier combination km is pressed do nothing.
  */
void ScintillaWrapper::ClearCmdKey(keymod km);
/** Drop all key mappings.
  */
void ScintillaWrapper::ClearAllCmdKeys();
/** Set the styles for a segment of the document.
  */
int ScintillaWrapper::SetStylingEx(str styles);
/** Set a style to be visible or not.
  */
void ScintillaWrapper::StyleSetVisible(int style, bool visible);
/** Get the time in milliseconds that the caret is on and off.
  */
int ScintillaWrapper::GetCaretPeriod();
/** Get the time in milliseconds that the caret is on and off. 0 = steady on.
  */
void ScintillaWrapper::SetCaretPeriod(int periodMilliseconds);
/** Set the set of characters making up words for when moving or selecting by word.
  * First sets defaults like SetCharsDefault.
  */
void ScintillaWrapper::SetWordChars(str characters);
/** Start a sequence of actions that is undone and redone as a unit.
  * May be nested.
  */
void ScintillaWrapper::BeginUndoAction();
/** End a sequence of actions that is undone and redone as a unit.
  */
void ScintillaWrapper::EndUndoAction();
/** Set an indicator to plain, squiggle or TT.
  */
void ScintillaWrapper::IndicSetStyle(int indic, int style);
/** Retrieve the style of an indicator.
  */
int ScintillaWrapper::IndicGetStyle(int indic);
/** Set the foreground colour of an indicator.
  */
void ScintillaWrapper::IndicSetFore(int indic, int foreRed, int foreGreen, int foreBlue);
/** Retrieve the foreground colour of an indicator.
  */
colour ScintillaWrapper::IndicGetFore(int indic);
/** Set an indicator to draw under text or over(default).
  */
void ScintillaWrapper::IndicSetUnder(int indic, bool under);
/** Retrieve whether indicator drawn under or over text.
  */
bool ScintillaWrapper::IndicGetUnder(int indic);
/** Set the foreground colour of all whitespace and whether to use this setting.
  */
void ScintillaWrapper::SetWhitespaceFore(bool useSetting, int foreRed, int foreGreen, int foreBlue);
/** Set the background colour of all whitespace and whether to use this setting.
  */
void ScintillaWrapper::SetWhitespaceBack(bool useSetting, int backRed, int backGreen, int backBlue);
/** Set the size of the dots used to mark space characters.
  */
void ScintillaWrapper::SetWhitespaceSize(int size);
/** Get the size of the dots used to mark space characters.
  */
int ScintillaWrapper::GetWhitespaceSize();
/** Divide each styling byte into lexical class bits (default: 5) and indicator
  * bits (default: 3). If a lexer requires more than 32 lexical states, then this
  * is used to expand the possible states.
  */
void ScintillaWrapper::SetStyleBits(int bits);
/** Retrieve number of bits in style bytes used to hold the lexical state.
  */
int ScintillaWrapper::GetStyleBits();
/** Used to hold extra styling information for each line.
  */
void ScintillaWrapper::SetLineState(int line, int state);
/** Retrieve the extra styling information for a line.
  */
int ScintillaWrapper::GetLineState(int line);
/** Retrieve the last line number that has line state.
  */
int ScintillaWrapper::GetMaxLineState();
/** Is the background of the line containing the caret in a different colour?
  */
bool ScintillaWrapper::GetCaretLineVisible();
/** Display the background of the line containing the caret in a different colour.
  */
void ScintillaWrapper::SetCaretLineVisible(bool show);
/** Get the colour of the background of the line containing the caret.
  */
colour ScintillaWrapper::GetCaretLineBack();
/** Set the colour of the background of the line containing the caret.
  */
void ScintillaWrapper::SetCaretLineBack(int backRed, int backGreen, int backBlue);
/** Set a style to be changeable or not (read only).
  * Experimental feature, currently buggy.
  */
void ScintillaWrapper::StyleSetChangeable(int style, bool changeable);
/** Display a auto-completion list.
  * The lenEntered parameter indicates how many characters before
  * the caret should be used to provide context.
  */
void ScintillaWrapper::AutoCShow(int lenEntered, str itemList);
/** Remove the auto-completion list from the screen.
  */
void ScintillaWrapper::AutoCCancel();
/** Is there an auto-completion list visible?
  */
bool ScintillaWrapper::AutoCActive();
/** Retrieve the position of the caret when the auto-completion list was displayed.
  */
int ScintillaWrapper::AutoCPosStart();
/** User has selected an item so remove the list and insert the selection.
  */
void ScintillaWrapper::AutoCComplete();
/** Define a set of character that when typed cancel the auto-completion list.
  */
void ScintillaWrapper::AutoCStops(str characterSet);
/** Change the separator character in the string setting up an auto-completion list.
  * Default is space but can be changed if items contain space.
  */
void ScintillaWrapper::AutoCSetSeparator(int separatorCharacter);
/** Retrieve the auto-completion list separator character.
  */
int ScintillaWrapper::AutoCGetSeparator();
/** Select the item in the auto-completion list that starts with a string.
  */
void ScintillaWrapper::AutoCSelect(str text);
/** Should the auto-completion list be cancelled if the user backspaces to a
  * position before where the box was created.
  */
void ScintillaWrapper::AutoCSetCancelAtStart(bool cancel);
/** Retrieve whether auto-completion cancelled by backspacing before start.
  */
bool ScintillaWrapper::AutoCGetCancelAtStart();
/** Define a set of characters that when typed will cause the autocompletion to
  * choose the selected item.
  */
void ScintillaWrapper::AutoCSetFillUps(str characterSet);
/** Should a single item auto-completion list automatically choose the item.
  */
void ScintillaWrapper::AutoCSetChooseSingle(bool chooseSingle);
/** Retrieve whether a single item auto-completion list automatically choose the item.
  */
bool ScintillaWrapper::AutoCGetChooseSingle();
/** Set whether case is significant when performing auto-completion searches.
  */
void ScintillaWrapper::AutoCSetIgnoreCase(bool ignoreCase);
/** Retrieve state of ignore case flag.
  */
bool ScintillaWrapper::AutoCGetIgnoreCase();
/** Display a list of strings and send notification when user chooses one.
  */
void ScintillaWrapper::UserListShow(int listType, str itemList);
/** Set whether or not autocompletion is hidden automatically when nothing matches.
  */
void ScintillaWrapper::AutoCSetAutoHide(bool autoHide);
/** Retrieve whether or not autocompletion is hidden automatically when nothing matches.
  */
bool ScintillaWrapper::AutoCGetAutoHide();
/** Set whether or not autocompletion deletes any word characters
  * after the inserted text upon completion.
  */
void ScintillaWrapper::AutoCSetDropRestOfWord(bool dropRestOfWord);
/** Retrieve whether or not autocompletion deletes any word characters
  * after the inserted text upon completion.
  */
bool ScintillaWrapper::AutoCGetDropRestOfWord();
/** Register an XPM image for use in autocompletion lists.
  */
void ScintillaWrapper::RegisterImage(int type, str xpmData);
/** Clear all the registered XPM images.
  */
void ScintillaWrapper::ClearRegisteredImages();
/** Retrieve the auto-completion list type-separator character.
  */
int ScintillaWrapper::AutoCGetTypeSeparator();
/** Change the type-separator character in the string setting up an auto-completion list.
  * Default is '?' but can be changed if items contain '?'.
  */
void ScintillaWrapper::AutoCSetTypeSeparator(int separatorCharacter);
/** Set the maximum width, in characters, of auto-completion and user lists.
  * Set to 0 to autosize to fit longest item, which is the default.
  */
void ScintillaWrapper::AutoCSetMaxWidth(int characterCount);
/** Get the maximum width, in characters, of auto-completion and user lists.
  */
int ScintillaWrapper::AutoCGetMaxWidth();
/** Set the maximum height, in rows, of auto-completion and user lists.
  * The default is 5 rows.
  */
void ScintillaWrapper::AutoCSetMaxHeight(int rowCount);
/** Set the maximum height, in rows, of auto-completion and user lists.
  */
int ScintillaWrapper::AutoCGetMaxHeight();
/** Set the number of spaces used for one level of indentation.
  */
void ScintillaWrapper::SetIndent(int indentSize);
/** Retrieve indentation size.
  */
int ScintillaWrapper::GetIndent();
/** Indentation will only use space characters if useTabs is false, otherwise
  * it will use a combination of tabs and spaces.
  */
void ScintillaWrapper::SetUseTabs(bool useTabs);
/** Retrieve whether tabs will be used in indentation.
  */
bool ScintillaWrapper::GetUseTabs();
/** Change the indentation of a line to a number of columns.
  */
void ScintillaWrapper::SetLineIndentation(int line, int indentSize);
/** Retrieve the number of columns that a line is indented.
  */
int ScintillaWrapper::GetLineIndentation(int line);
/** Retrieve the position before the first non indentation character on a line.
  */
int ScintillaWrapper::GetLineIndentPosition(int line);
/** Retrieve the column number of a position, taking tab width into account.
  */
int ScintillaWrapper::GetColumn(int pos);
/** Show or hide the horizontal scroll bar.
  */
void ScintillaWrapper::SetHScrollBar(bool show);
/** Is the horizontal scroll bar visible?
  */
bool ScintillaWrapper::GetHScrollBar();
/** Show or hide indentation guides.
  */
void ScintillaWrapper::SetIndentationGuides(int indentView);
/** Are the indentation guides visible?
  */
int ScintillaWrapper::GetIndentationGuides();
/** Set the highlighted indentation guide column.
  * 0 = no highlighted guide.
  */
void ScintillaWrapper::SetHighlightGuide(int column);
/** Get the highlighted indentation guide column.
  */
int ScintillaWrapper::GetHighlightGuide();
/** Get the position after the last visible characters on a line.
  */
int ScintillaWrapper::GetLineEndPosition(int line);
/** Get the code page used to interpret the bytes of the document as characters.
  */
int ScintillaWrapper::GetCodePage();
/** Get the foreground colour of the caret.
  */
colour ScintillaWrapper::GetCaretFore();
/** In palette mode?
  */
bool ScintillaWrapper::GetUsePalette();
/** In read-only mode?
  */
bool ScintillaWrapper::GetReadOnly();
/** Sets the position of the caret.
  */
void ScintillaWrapper::SetCurrentPos(int pos);
/** Sets the position that starts the selection - this becomes the anchor.
  */
void ScintillaWrapper::SetSelectionStart(int pos);
/** Returns the position at the start of the selection.
  */
int ScintillaWrapper::GetSelectionStart();
/** Sets the position that ends the selection - this becomes the currentPosition.
  */
void ScintillaWrapper::SetSelectionEnd(int pos);
/** Returns the position at the end of the selection.
  */
int ScintillaWrapper::GetSelectionEnd();
/** Sets the print magnification added to the point size of each style for printing.
  */
void ScintillaWrapper::SetPrintMagnification(int magnification);
/** Returns the print magnification.
  */
int ScintillaWrapper::GetPrintMagnification();
/** Modify colours when printing for clearer printed text.
  */
void ScintillaWrapper::SetPrintColourMode(int mode);
/** Returns the print colour mode.
  */
int ScintillaWrapper::GetPrintColourMode();
/** Find some text in the document.
  */
tuple ScintillaWrapper::FindText(int flags, int start, int end, str ft);
/** Retrieve the display line at the top of the display.
  */
int ScintillaWrapper::GetFirstVisibleLine();
/** Retrieve the contents of a line.
  * Returns the length of the line.
  */
str ScintillaWrapper::GetLine();
/** Returns the number of lines in the document. There is always at least one.
  */
int ScintillaWrapper::GetLineCount();
/** Sets the size in pixels of the left margin.
  */
void ScintillaWrapper::SetMarginLeft(int pixelWidth);
/** Returns the size in pixels of the left margin.
  */
int ScintillaWrapper::GetMarginLeft();
/** Sets the size in pixels of the right margin.
  */
void ScintillaWrapper::SetMarginRight(int pixelWidth);
/** Returns the size in pixels of the right margin.
  */
int ScintillaWrapper::GetMarginRight();
/** Is the document different from when it was last saved?
  */
bool ScintillaWrapper::GetModify();
/** Select a range of text.
  */
void ScintillaWrapper::SetSel(int start, int end);
/** Retrieve the selected text.
  * Return the length of the text.
  */
str ScintillaWrapper::GetSelText();
/** Retrieve a range of text.
  * Return the length of the text.
  */
str ScintillaWrapper::GetTextRange(int start, int end);
/** Draw the selection in normal style or with selection highlighted.
  */
void ScintillaWrapper::HideSelection(bool normal);
/** Retrieve the x value of the point in the window where a position is displayed.
  */
int ScintillaWrapper::PointXFromPosition(int pos);
/** Retrieve the y value of the point in the window where a position is displayed.
  */
int ScintillaWrapper::PointYFromPosition(int pos);
/** Retrieve the line containing a position.
  */
int ScintillaWrapper::LineFromPosition(int pos);
/** Retrieve the position at the start of a line.
  */
int ScintillaWrapper::PositionFromLine(int line);
/** Scroll horizontally and vertically.
  */
void ScintillaWrapper::LineScroll(int columns, int lines);
/** Ensure the caret is visible.
  */
void ScintillaWrapper::ScrollCaret();
/** Replace the selected text with the argument text.
  */
void ScintillaWrapper::ReplaceSel(str text);
/** Set to read only or read write.
  */
void ScintillaWrapper::SetReadOnly(bool readOnly);
/** Null operation.
  */
void ScintillaWrapper::Null();
/** Will a paste succeed?
  */
bool ScintillaWrapper::CanPaste();
/** Are there any undoable actions in the undo history?
  */
bool ScintillaWrapper::CanUndo();
/** Delete the undo history.
  */
void ScintillaWrapper::EmptyUndoBuffer();
/** Undo one action in the undo history.
  */
void ScintillaWrapper::Undo();
/** Cut the selection to the clipboard.
  */
void ScintillaWrapper::Cut();
/** Copy the selection to the clipboard.
  */
void ScintillaWrapper::Copy();
/** Paste the contents of the clipboard into the document replacing the selection.
  */
void ScintillaWrapper::Paste();
/** Clear the selection.
  */
void ScintillaWrapper::Clear();
/** Replace the contents of the document with the argument text.
  */
void ScintillaWrapper::SetText(str text);
/** Retrieve all the text in the document.
  * Returns number of characters retrieved.
  */
str ScintillaWrapper::GetText();
/** Retrieve the number of characters in the document.
  */
int ScintillaWrapper::GetTextLength();
/** Retrieve a pointer to a function that processes messages for this Scintilla.
  */
int ScintillaWrapper::GetDirectFunction();
/** Retrieve a pointer value to use as the first argument when calling
  * the function returned by GetDirectFunction.
  */
int ScintillaWrapper::GetDirectPointer();
/** Set to overtype (true) or insert mode.
  */
void ScintillaWrapper::SetOvertype(bool overtype);
/** Returns true if overtype mode is active otherwise false is returned.
  */
bool ScintillaWrapper::GetOvertype();
/** Set the width of the insert mode caret.
  */
void ScintillaWrapper::SetCaretWidth(int pixelWidth);
/** Returns the width of the insert mode caret.
  */
int ScintillaWrapper::GetCaretWidth();
/** Sets the position that starts the target which is used for updating the
  * document without affecting the scroll position.
  */
void ScintillaWrapper::SetTargetStart(int pos);
/** Get the position that starts the target.
  */
int ScintillaWrapper::GetTargetStart();
/** Sets the position that ends the target which is used for updating the
  * document without affecting the scroll position.
  */
void ScintillaWrapper::SetTargetEnd(int pos);
/** Get the position that ends the target.
  */
int ScintillaWrapper::GetTargetEnd();
/** Replace the target text with the argument text.
  * Text is counted so it can contain NULs.
  * Returns the length of the replacement text.
  */
int ScintillaWrapper::ReplaceTarget(str text);
/** Replace the target text with the argument text after \d processing.
  * Text is counted so it can contain NULs.
  * Looks for \d where d is between 1 and 9 and replaces these with the strings
  * matched in the last search operation which were surrounded by \( and \).
  * Returns the length of the replacement text including any change
  * caused by processing the \d patterns.
  */
int ScintillaWrapper::ReplaceTargetRE(str text);
/** Search for a counted string in the target and set the target to the found
  * range. Text is counted so it can contain NULs.
  * Returns length of range or -1 for failure in which case target is not moved.
  */
int ScintillaWrapper::SearchInTarget(str text);
/** Set the search flags used by SearchInTarget.
  */
void ScintillaWrapper::SetSearchFlags(int flags);
/** Get the search flags used by SearchInTarget.
  */
int ScintillaWrapper::GetSearchFlags();
/** Show a call tip containing a definition near position pos.
  */
void ScintillaWrapper::CallTipShow(int pos, str definition);
/** Remove the call tip from the screen.
  */
void ScintillaWrapper::CallTipCancel();
/** Is there an active call tip?
  */
bool ScintillaWrapper::CallTipActive();
/** Retrieve the position where the caret was before displaying the call tip.
  */
int ScintillaWrapper::CallTipPosStart();
/** Highlight a segment of the definition.
  */
void ScintillaWrapper::CallTipSetHlt(int start, int end);
/** Set the background colour for the call tip.
  */
void ScintillaWrapper::CallTipSetBack(int backRed, int backGreen, int backBlue);
/** Set the foreground colour for the call tip.
  */
void ScintillaWrapper::CallTipSetFore(int foreRed, int foreGreen, int foreBlue);
/** Set the foreground colour for the highlighted part of the call tip.
  */
void ScintillaWrapper::CallTipSetForeHlt(int foreRed, int foreGreen, int foreBlue);
/** Enable use of STYLE_CALLTIP and set call tip tab size in pixels.
  */
void ScintillaWrapper::CallTipUseStyle(int tabSize);
/** Find the display line of a document line taking hidden lines into account.
  */
int ScintillaWrapper::VisibleFromDocLine(int line);
/** Find the document line of a display line taking hidden lines into account.
  */
int ScintillaWrapper::DocLineFromVisible(int lineDisplay);
/** The number of display lines needed to wrap a document line
  */
int ScintillaWrapper::WrapCount(int line);
/** Set the fold level of a line.
  * This encodes an integer level along with flags indicating whether the
  * line is a header and whether it is effectively white space.
  */
void ScintillaWrapper::SetFoldLevel(int line, int level);
/** Retrieve the fold level of a line.
  */
int ScintillaWrapper::GetFoldLevel(int line);
/** Find the last child line of a header line.
  */
int ScintillaWrapper::GetLastChild(int line, int level);
/** Find the parent line of a child line.
  */
int ScintillaWrapper::GetFoldParent(int line);
/** Make a range of lines visible.
  */
void ScintillaWrapper::ShowLines(int lineStart, int lineEnd);
/** Make a range of lines invisible.
  */
void ScintillaWrapper::HideLines(int lineStart, int lineEnd);
/** Is a line visible?
  */
bool ScintillaWrapper::GetLineVisible(int line);
/** Show the children of a header line.
  */
void ScintillaWrapper::SetFoldExpanded(int line, bool expanded);
/** Is a header line expanded?
  */
bool ScintillaWrapper::GetFoldExpanded(int line);
/** Switch a header line between expanded and contracted.
  */
void ScintillaWrapper::ToggleFold(int line);
/** Ensure a particular line is visible by expanding any header line hiding it.
  */
void ScintillaWrapper::EnsureVisible(int line);
/** Set some style options for folding.
  */
void ScintillaWrapper::SetFoldFlags(int flags);
/** Ensure a particular line is visible by expanding any header line hiding it.
  * Use the currently set visibility policy to determine which range to display.
  */
void ScintillaWrapper::EnsureVisibleEnforcePolicy(int line);
/** Sets whether a tab pressed when caret is within indentation indents.
  */
void ScintillaWrapper::SetTabIndents(bool tabIndents);
/** Does a tab pressed when caret is within indentation indent?
  */
bool ScintillaWrapper::GetTabIndents();
/** Sets whether a backspace pressed when caret is within indentation unindents.
  */
void ScintillaWrapper::SetBackSpaceUnIndents(bool bsUnIndents);
/** Does a backspace pressed when caret is within indentation unindent?
  */
bool ScintillaWrapper::GetBackSpaceUnIndents();
/** Sets the time the mouse must sit still to generate a mouse dwell event.
  */
void ScintillaWrapper::SetMouseDwellTime(int periodMilliseconds);
/** Retrieve the time the mouse must sit still to generate a mouse dwell event.
  */
int ScintillaWrapper::GetMouseDwellTime();
/** Get position of start of word.
  */
int ScintillaWrapper::WordStartPosition(int pos, bool onlyWordCharacters);
/** Get position of end of word.
  */
int ScintillaWrapper::WordEndPosition(int pos, bool onlyWordCharacters);
/** Sets whether text is word wrapped.
  */
void ScintillaWrapper::SetWrapMode(int mode);
/** Retrieve whether text is word wrapped.
  */
int ScintillaWrapper::GetWrapMode();
/** Set the display mode of visual flags for wrapped lines.
  */
void ScintillaWrapper::SetWrapVisualFlags(int wrapVisualFlags);
/** Retrive the display mode of visual flags for wrapped lines.
  */
int ScintillaWrapper::GetWrapVisualFlags();
/** Set the location of visual flags for wrapped lines.
  */
void ScintillaWrapper::SetWrapVisualFlagsLocation(int wrapVisualFlagsLocation);
/** Retrive the location of visual flags for wrapped lines.
  */
int ScintillaWrapper::GetWrapVisualFlagsLocation();
/** Set the start indent for wrapped lines.
  */
void ScintillaWrapper::SetWrapStartIndent(int indent);
/** Retrive the start indent for wrapped lines.
  */
int ScintillaWrapper::GetWrapStartIndent();
/** Sets how wrapped sublines are placed. Default is fixed.
  */
void ScintillaWrapper::SetWrapIndentMode(int mode);
/** Retrieve how wrapped sublines are placed. Default is fixed.
  */
int ScintillaWrapper::GetWrapIndentMode();
/** Sets the degree of caching of layout information.
  */
void ScintillaWrapper::SetLayoutCache(int mode);
/** Retrieve the degree of caching of layout information.
  */
int ScintillaWrapper::GetLayoutCache();
/** Sets the document width assumed for scrolling.
  */
void ScintillaWrapper::SetScrollWidth(int pixelWidth);
/** Retrieve the document width assumed for scrolling.
  */
int ScintillaWrapper::GetScrollWidth();
/** Sets whether the maximum width line displayed is used to set scroll width.
  */
void ScintillaWrapper::SetScrollWidthTracking(bool tracking);
/** Retrieve whether the scroll width tracks wide lines.
  */
bool ScintillaWrapper::GetScrollWidthTracking();
/** Measure the pixel width of some text in a particular style.
  * NUL terminated text argument.
  * Does not handle tab or control characters.
  */
int ScintillaWrapper::TextWidth(int style, str text);
/** Sets the scroll range so that maximum scroll position has
  * the last line at the bottom of the view (default).
  * Setting this to false allows scrolling one page below the last line.
  */
void ScintillaWrapper::SetEndAtLastLine(bool endAtLastLine);
/** Retrieve whether the maximum scroll position has the last
  * line at the bottom of the view.
  */
bool ScintillaWrapper::GetEndAtLastLine();
/** Retrieve the height of a particular line of text in pixels.
  */
int ScintillaWrapper::TextHeight(int line);
/** Show or hide the vertical scroll bar.
  */
void ScintillaWrapper::SetVScrollBar(bool show);
/** Is the vertical scroll bar visible?
  */
bool ScintillaWrapper::GetVScrollBar();
/** Append a string to the end of the document without changing the selection.
  */
int ScintillaWrapper::AppendText(str text);
/** Is drawing done in two phases with backgrounds drawn before faoregrounds?
  */
bool ScintillaWrapper::GetTwoPhaseDraw();
/** In twoPhaseDraw mode, drawing is performed in two phases, first the background
  * and then the foreground. This avoids chopping off characters that overlap the next run.
  */
void ScintillaWrapper::SetTwoPhaseDraw(bool twoPhase);
/** Choose the quality level for text from the FontQuality enumeration.
  */
void ScintillaWrapper::SetFontQuality(int fontQuality);
/** Retrieve the quality level for text.
  */
int ScintillaWrapper::GetFontQuality();
/** Scroll so that a display line is at the top of the display.
  */
void ScintillaWrapper::SetFirstVisibleLine(int lineDisplay);
/** Change the effect of pasting when there are multiple selections.
  */
void ScintillaWrapper::SetMultiPaste(int multiPaste);
/** Retrieve the effect of pasting when there are multiple selections..
  */
int ScintillaWrapper::GetMultiPaste();
/** Retrieve the value of a tag from a regular expression search.
  */
str ScintillaWrapper::GetTag();
/** Make the target range start and end be the same as the selection range start and end.
  */
void ScintillaWrapper::TargetFromSelection();
/** Join the lines in the target.
  */
void ScintillaWrapper::LinesJoin();
/** Split the lines in the target into lines that are less wide than pixelWidth
  * where possible.
  */
void ScintillaWrapper::LinesSplit(int pixelWidth);
/** Set the colours used as a chequerboard pattern in the fold margin
  */
void ScintillaWrapper::SetFoldMarginColour(bool useSetting, int backRed, int backGreen, int backBlue);
/** Set the colours used as a chequerboard pattern in the fold margin
  */
void ScintillaWrapper::SetFoldMarginHiColour(bool useSetting, int foreRed, int foreGreen, int foreBlue);
/** Move caret down one line.
  */
void ScintillaWrapper::LineDown();
/** Move caret down one line extending selection to new caret position.
  */
void ScintillaWrapper::LineDownExtend();
/** Move caret up one line.
  */
void ScintillaWrapper::LineUp();
/** Move caret up one line extending selection to new caret position.
  */
void ScintillaWrapper::LineUpExtend();
/** Move caret left one character.
  */
void ScintillaWrapper::CharLeft();
/** Move caret left one character extending selection to new caret position.
  */
void ScintillaWrapper::CharLeftExtend();
/** Move caret right one character.
  */
void ScintillaWrapper::CharRight();
/** Move caret right one character extending selection to new caret position.
  */
void ScintillaWrapper::CharRightExtend();
/** Move caret left one word.
  */
void ScintillaWrapper::WordLeft();
/** Move caret left one word extending selection to new caret position.
  */
void ScintillaWrapper::WordLeftExtend();
/** Move caret right one word.
  */
void ScintillaWrapper::WordRight();
/** Move caret right one word extending selection to new caret position.
  */
void ScintillaWrapper::WordRightExtend();
/** Move caret to first position on line.
  */
void ScintillaWrapper::Home();
/** Move caret to first position on line extending selection to new caret position.
  */
void ScintillaWrapper::HomeExtend();
/** Move caret to last position on line.
  */
void ScintillaWrapper::LineEnd();
/** Move caret to last position on line extending selection to new caret position.
  */
void ScintillaWrapper::LineEndExtend();
/** Move caret to first position in document.
  */
void ScintillaWrapper::DocumentStart();
/** Move caret to first position in document extending selection to new caret position.
  */
void ScintillaWrapper::DocumentStartExtend();
/** Move caret to last position in document.
  */
void ScintillaWrapper::DocumentEnd();
/** Move caret to last position in document extending selection to new caret position.
  */
void ScintillaWrapper::DocumentEndExtend();
/** Move caret one page up.
  */
void ScintillaWrapper::PageUp();
/** Move caret one page up extending selection to new caret position.
  */
void ScintillaWrapper::PageUpExtend();
/** Move caret one page down.
  */
void ScintillaWrapper::PageDown();
/** Move caret one page down extending selection to new caret position.
  */
void ScintillaWrapper::PageDownExtend();
/** Switch from insert to overtype mode or the reverse.
  */
void ScintillaWrapper::EditToggleOvertype();
/** Cancel any modes such as call tip or auto-completion list display.
  */
void ScintillaWrapper::Cancel();
/** Delete the selection or if no selection, the character before the caret.
  */
void ScintillaWrapper::DeleteBack();
/** If selection is empty or all on one line replace the selection with a tab character.
  * If more than one line selected, indent the lines.
  */
void ScintillaWrapper::Tab();
/** Dedent the selected lines.
  */
void ScintillaWrapper::BackTab();
/** Insert a new line, may use a CRLF, CR or LF depending on EOL mode.
  */
void ScintillaWrapper::NewLine();
/** Insert a Form Feed character.
  */
void ScintillaWrapper::FormFeed();
/** Move caret to before first visible character on line.
  * If already there move to first character on line.
  */
void ScintillaWrapper::VCHome();
/** Like VCHome but extending selection to new caret position.
  */
void ScintillaWrapper::VCHomeExtend();
/** Magnify the displayed text by increasing the sizes by 1 point.
  */
void ScintillaWrapper::ZoomIn();
/** Make the displayed text smaller by decreasing the sizes by 1 point.
  */
void ScintillaWrapper::ZoomOut();
/** Delete the word to the left of the caret.
  */
void ScintillaWrapper::DelWordLeft();
/** Delete the word to the right of the caret.
  */
void ScintillaWrapper::DelWordRight();
/** Delete the word to the right of the caret, but not the trailing non-word characters.
  */
void ScintillaWrapper::DelWordRightEnd();
/** Cut the line containing the caret.
  */
void ScintillaWrapper::LineCut();
/** Delete the line containing the caret.
  */
void ScintillaWrapper::LineDelete();
/** Switch the current line with the previous.
  */
void ScintillaWrapper::LineTranspose();
/** Duplicate the current line.
  */
void ScintillaWrapper::LineDuplicate();
/** Transform the selection to lower case.
  */
void ScintillaWrapper::LowerCase();
/** Transform the selection to upper case.
  */
void ScintillaWrapper::UpperCase();
/** Scroll the document down, keeping the caret visible.
  */
void ScintillaWrapper::LineScrollDown();
/** Scroll the document up, keeping the caret visible.
  */
void ScintillaWrapper::LineScrollUp();
/** Delete the selection or if no selection, the character before the caret.
  * Will not delete the character before at the start of a line.
  */
void ScintillaWrapper::DeleteBackNotLine();
/** Move caret to first position on display line.
  */
void ScintillaWrapper::HomeDisplay();
/** Move caret to first position on display line extending selection to
  * new caret position.
  */
void ScintillaWrapper::HomeDisplayExtend();
/** Move caret to last position on display line.
  */
void ScintillaWrapper::LineEndDisplay();
/** Move caret to last position on display line extending selection to new
  * caret position.
  */
void ScintillaWrapper::LineEndDisplayExtend();
/** These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
  * except they behave differently when word-wrap is enabled:
  * They go first to the start / end of the display line, like (Home|LineEnd)Display
  * The difference is that, the cursor is already at the point, it goes on to the start
  * or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
  */
void ScintillaWrapper::HomeWrap();
/** These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
  * except they behave differently when word-wrap is enabled:
  * They go first to the start / end of the display line, like (Home|LineEnd)Display
  * The difference is that, the cursor is already at the point, it goes on to the start
  * or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
  */
void ScintillaWrapper::HomeWrapExtend();
/** These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
  * except they behave differently when word-wrap is enabled:
  * They go first to the start / end of the display line, like (Home|LineEnd)Display
  * The difference is that, the cursor is already at the point, it goes on to the start
  * or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
  */
void ScintillaWrapper::LineEndWrap();
/** These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
  * except they behave differently when word-wrap is enabled:
  * They go first to the start / end of the display line, like (Home|LineEnd)Display
  * The difference is that, the cursor is already at the point, it goes on to the start
  * or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
  */
void ScintillaWrapper::LineEndWrapExtend();
/** These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
  * except they behave differently when word-wrap is enabled:
  * They go first to the start / end of the display line, like (Home|LineEnd)Display
  * The difference is that, the cursor is already at the point, it goes on to the start
  * or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
  */
void ScintillaWrapper::VCHomeWrap();
/** These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
  * except they behave differently when word-wrap is enabled:
  * They go first to the start / end of the display line, like (Home|LineEnd)Display
  * The difference is that, the cursor is already at the point, it goes on to the start
  * or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
  */
void ScintillaWrapper::VCHomeWrapExtend();
/** Copy the line containing the caret.
  */
void ScintillaWrapper::LineCopy();
/** Move the caret inside current view if it's not there already.
  */
void ScintillaWrapper::MoveCaretInsideView();
/** How many characters are on a line, including end of line characters?
  */
int ScintillaWrapper::LineLength(int line);
/** Highlight the characters at two positions.
  */
void ScintillaWrapper::BraceHighlight(int pos1, int pos2);
/** Highlight the character at a position indicating there is no matching brace.
  */
void ScintillaWrapper::BraceBadLight(int pos);
/** Find the position of a matching brace or INVALID_POSITION if no match.
  */
int ScintillaWrapper::BraceMatch(int pos);
/** Are the end of line characters visible?
  */
bool ScintillaWrapper::GetViewEOL();
/** Make the end of line characters visible or invisible.
  */
void ScintillaWrapper::SetViewEOL(bool visible);
/** Retrieve a pointer to the document object.
  */
int ScintillaWrapper::GetDocPointer();
/** Change the document object used.
  */
void ScintillaWrapper::SetDocPointer(int pointer);
/** Set which document modification events are sent to the container.
  */
void ScintillaWrapper::SetModEventMask(int mask);
/** Retrieve the column number which text should be kept within.
  */
int ScintillaWrapper::GetEdgeColumn();
/** Set the column number of the edge.
  * If text goes past the edge then it is highlighted.
  */
void ScintillaWrapper::SetEdgeColumn(int column);
/** Retrieve the edge highlight mode.
  */
int ScintillaWrapper::GetEdgeMode();
/** The edge may be displayed by a line (EDGE_LINE) or by highlighting text that
  * goes beyond it (EDGE_BACKGROUND) or not displayed at all (EDGE_NONE).
  */
void ScintillaWrapper::SetEdgeMode(int mode);
/** Retrieve the colour used in edge indication.
  */
colour ScintillaWrapper::GetEdgeColour();
/** Change the colour used in edge indication.
  */
void ScintillaWrapper::SetEdgeColour(int edgeColourRed, int edgeColourGreen, int edgeColourBlue);
/** Sets the current caret position to be the search anchor.
  */
void ScintillaWrapper::SearchAnchor();
/** Find some text starting at the search anchor.
  * Does not ensure the selection is visible.
  */
int ScintillaWrapper::SearchNext(int flags, str text);
/** Find some text starting at the search anchor and moving backwards.
  * Does not ensure the selection is visible.
  */
int ScintillaWrapper::SearchPrev(int flags, str text);
/** Retrieves the number of lines completely visible.
  */
int ScintillaWrapper::LinesOnScreen();
/** Set whether a pop up menu is displayed automatically when the user presses
  * the wrong mouse button.
  */
void ScintillaWrapper::UsePopUp(bool allowPopUp);
/** Is the selection rectangular? The alternative is the more common stream selection.
  */
bool ScintillaWrapper::SelectionIsRectangle();
/** Set the zoom level. This number of points is added to the size of all fonts.
  * It may be positive to magnify or negative to reduce.
  */
void ScintillaWrapper::SetZoom(int zoom);
/** Retrieve the zoom level.
  */
int ScintillaWrapper::GetZoom();
/** Create a new document object.
  * Starts with reference count of 1 and not selected into editor.
  */
int ScintillaWrapper::CreateDocument();
/** Extend life of document.
  */
void ScintillaWrapper::AddRefDocument(int doc);
/** Release a reference to the document, deleting document if it fades to black.
  */
void ScintillaWrapper::ReleaseDocument(int doc);
/** Get which document modification events are sent to the container.
  */
int ScintillaWrapper::GetModEventMask();
/** Change internal focus flag.
  */
void ScintillaWrapper::SetFocus(bool focus);
/** Get internal focus flag.
  */
bool ScintillaWrapper::GetFocus();
/** Change error status - 0 = OK.
  */
void ScintillaWrapper::SetStatus(int statusCode);
/** Get error status.
  */
int ScintillaWrapper::GetStatus();
/** Set whether the mouse is captured when its button is pressed.
  */
void ScintillaWrapper::SetMouseDownCaptures(bool captures);
/** Get whether mouse gets captured.
  */
bool ScintillaWrapper::GetMouseDownCaptures();
/** Sets the cursor to one of the SC_CURSOR* values.
  */
void ScintillaWrapper::SetCursor(int cursorType);
/** Get cursor type.
  */
int ScintillaWrapper::GetCursor();
/** Change the way control characters are displayed:
  * If symbol is < 32, keep the drawn way, else, use the given character.
  */
void ScintillaWrapper::SetControlCharSymbol(int symbol);
/** Get the way control characters are displayed.
  */
int ScintillaWrapper::GetControlCharSymbol();
/** Move to the previous change in capitalisation.
  */
void ScintillaWrapper::WordPartLeft();
/** Move to the previous change in capitalisation extending selection
  * to new caret position.
  */
void ScintillaWrapper::WordPartLeftExtend();
/** Move to the change next in capitalisation.
  */
void ScintillaWrapper::WordPartRight();
/** Move to the next change in capitalisation extending selection
  * to new caret position.
  */
void ScintillaWrapper::WordPartRightExtend();
/** Set the way the display area is determined when a particular line
  * is to be moved to by Find, FindNext, GotoLine, etc.
  */
void ScintillaWrapper::SetVisiblePolicy(int visiblePolicy, int visibleSlop);
/** Delete back from the current position to the start of the line.
  */
void ScintillaWrapper::DelLineLeft();
/** Delete forwards from the current position to the end of the line.
  */
void ScintillaWrapper::DelLineRight();
/** Get and Set the xOffset (ie, horizonal scroll position).
  */
void ScintillaWrapper::SetXOffset(int newOffset);
/** Get and Set the xOffset (ie, horizonal scroll position).
  */
int ScintillaWrapper::GetXOffset();
/** Set the last x chosen value to be the caret x position.
  */
void ScintillaWrapper::ChooseCaretX();
/** Set the focus to this Scintilla widget.
  */
void ScintillaWrapper::GrabFocus();
/** Set the way the caret is kept visible when going sideway.
  * The exclusion zone is given in pixels.
  */
void ScintillaWrapper::SetXCaretPolicy(int caretPolicy, int caretSlop);
/** Set the way the line the caret is on is kept visible.
  * The exclusion zone is given in lines.
  */
void ScintillaWrapper::SetYCaretPolicy(int caretPolicy, int caretSlop);
/** Set printing to line wrapped (SC_WRAP_WORD) or not line wrapped (SC_WRAP_NONE).
  */
void ScintillaWrapper::SetPrintWrapMode(int mode);
/** Is printing line wrapped?
  */
int ScintillaWrapper::GetPrintWrapMode();
/** Set a fore colour for active hotspots.
  */
void ScintillaWrapper::SetHotspotActiveFore(bool useSetting, int foreRed, int foreGreen, int foreBlue);
/** Get the fore colour for active hotspots.
  */
colour ScintillaWrapper::GetHotspotActiveFore();
/** Set a back colour for active hotspots.
  */
void ScintillaWrapper::SetHotspotActiveBack(bool useSetting, int backRed, int backGreen, int backBlue);
/** Get the back colour for active hotspots.
  */
colour ScintillaWrapper::GetHotspotActiveBack();
/** Enable / Disable underlining active hotspots.
  */
void ScintillaWrapper::SetHotspotActiveUnderline(bool underline);
/** Get whether underlining for active hotspots.
  */
bool ScintillaWrapper::GetHotspotActiveUnderline();
/** Limit hotspots to single line so hotspots on two lines don't merge.
  */
void ScintillaWrapper::SetHotspotSingleLine(bool singleLine);
/** Get the HotspotSingleLine property
  */
bool ScintillaWrapper::GetHotspotSingleLine();
/** Move caret between paragraphs (delimited by empty lines).
  */
void ScintillaWrapper::ParaDown();
/** Move caret between paragraphs (delimited by empty lines).
  */
void ScintillaWrapper::ParaDownExtend();
/** Move caret between paragraphs (delimited by empty lines).
  */
void ScintillaWrapper::ParaUp();
/** Move caret between paragraphs (delimited by empty lines).
  */
void ScintillaWrapper::ParaUpExtend();
/** Given a valid document position, return the previous position taking code
  * page into account. Returns 0 if passed 0.
  */
int ScintillaWrapper::PositionBefore(int pos);
/** Given a valid document position, return the next position taking code
  * page into account. Maximum value returned is the last position in the document.
  */
int ScintillaWrapper::PositionAfter(int pos);
/** Copy a range of text to the clipboard. Positions are clipped into the document.
  */
void ScintillaWrapper::CopyRange(int start, int end);
/** Copy argument text to the clipboard.
  */
int ScintillaWrapper::CopyText(str text);
/** Set the selection mode to stream (SC_SEL_STREAM) or rectangular (SC_SEL_RECTANGLE/SC_SEL_THIN) or
  * by lines (SC_SEL_LINES).
  */
void ScintillaWrapper::SetSelectionMode(int mode);
/** Get the mode of the current selection.
  */
int ScintillaWrapper::GetSelectionMode();
/** Retrieve the position of the start of the selection at the given line (INVALID_POSITION if no selection on this line).
  */
int ScintillaWrapper::GetLineSelStartPosition(int line);
/** Retrieve the position of the end of the selection at the given line (INVALID_POSITION if no selection on this line).
  */
int ScintillaWrapper::GetLineSelEndPosition(int line);
/** Move caret down one line, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::LineDownRectExtend();
/** Move caret up one line, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::LineUpRectExtend();
/** Move caret left one character, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::CharLeftRectExtend();
/** Move caret right one character, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::CharRightRectExtend();
/** Move caret to first position on line, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::HomeRectExtend();
/** Move caret to before first visible character on line.
  * If already there move to first character on line.
  * In either case, extend rectangular selection to new caret position.
  */
void ScintillaWrapper::VCHomeRectExtend();
/** Move caret to last position on line, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::LineEndRectExtend();
/** Move caret one page up, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::PageUpRectExtend();
/** Move caret one page down, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::PageDownRectExtend();
/** Move caret to top of page, or one page up if already at top of page.
  */
void ScintillaWrapper::StutteredPageUp();
/** Move caret to top of page, or one page up if already at top of page, extending selection to new caret position.
  */
void ScintillaWrapper::StutteredPageUpExtend();
/** Move caret to bottom of page, or one page down if already at bottom of page.
  */
void ScintillaWrapper::StutteredPageDown();
/** Move caret to bottom of page, or one page down if already at bottom of page, extending selection to new caret position.
  */
void ScintillaWrapper::StutteredPageDownExtend();
/** Move caret left one word, position cursor at end of word.
  */
void ScintillaWrapper::WordLeftEnd();
/** Move caret left one word, position cursor at end of word, extending selection to new caret position.
  */
void ScintillaWrapper::WordLeftEndExtend();
/** Move caret right one word, position cursor at end of word.
  */
void ScintillaWrapper::WordRightEnd();
/** Move caret right one word, position cursor at end of word, extending selection to new caret position.
  */
void ScintillaWrapper::WordRightEndExtend();
/** Set the set of characters making up whitespace for when moving or selecting by word.
  * Should be called after SetWordChars.
  */
void ScintillaWrapper::SetWhitespaceChars(str characters);
/** Reset the set of characters for whitespace and word characters to the defaults.
  */
void ScintillaWrapper::SetCharsDefault();
/** Get currently selected item position in the auto-completion list
  */
int ScintillaWrapper::AutoCGetCurrent();
/** Get currently selected item text in the auto-completion list
  * Returns the length of the item text
  */
str ScintillaWrapper::AutoCGetCurrentText();
/** Enlarge the document to a particular size of text bytes.
  */
void ScintillaWrapper::Allocate(int bytes);
/** Returns the target converted to UTF8.
  * Return the length in bytes.
  */
str ScintillaWrapper::TargetAsUTF8();
/** Set the length of the utf8 argument for calling EncodedFromUTF8.
  * Set to -1 and the string will be measured to the first nul.
  */
void ScintillaWrapper::SetLengthForEncode(int bytes);
/** Translates a UTF8 string into the document encoding.
  * Return the length of the result in bytes.
  * On error return 0.
  */
str ScintillaWrapper::EncodedFromUTF8();
/** Find the position of a column on a line taking into account tabs and
  * multi-byte characters. If beyond end of line, return line end position.
  */
int ScintillaWrapper::FindColumn(int line, int column);
/** Can the caret preferred x position only be changed by explicit movement commands?
  */
bool ScintillaWrapper::GetCaretSticky();
/** Stop the caret preferred x position changing when the user types.
  */
void ScintillaWrapper::SetCaretSticky(bool useCaretStickyBehaviour);
/** Switch between sticky and non-sticky: meant to be bound to a key.
  */
void ScintillaWrapper::ToggleCaretSticky();
/** Enable/Disable convert-on-paste for line endings
  */
void ScintillaWrapper::SetPasteConvertEndings(bool convert);
/** Get convert-on-paste setting
  */
bool ScintillaWrapper::GetPasteConvertEndings();
/** Duplicate the selection. If selection empty duplicate the line containing the caret.
  */
void ScintillaWrapper::SelectionDuplicate();
/** Set background alpha of the caret line.
  */
void ScintillaWrapper::SetCaretLineBackAlpha(int alpha);
/** Get the background alpha of the caret line.
  */
int ScintillaWrapper::GetCaretLineBackAlpha();
/** Set the style of the caret to be drawn.
  */
void ScintillaWrapper::SetCaretStyle(int caretStyle);
/** Returns the current style of the caret.
  */
int ScintillaWrapper::GetCaretStyle();
/** Set the indicator used for IndicatorFillRange and IndicatorClearRange
  */
void ScintillaWrapper::SetIndicatorCurrent(int indicator);
/** Get the current indicator
  */
int ScintillaWrapper::GetIndicatorCurrent();
/** Set the value used for IndicatorFillRange
  */
void ScintillaWrapper::SetIndicatorValue(int value);
/** Get the current indicator vaue
  */
int ScintillaWrapper::GetIndicatorValue();
/** Turn a indicator on over a range.
  */
void ScintillaWrapper::IndicatorFillRange(int position, int fillLength);
/** Turn a indicator off over a range.
  */
void ScintillaWrapper::IndicatorClearRange(int position, int clearLength);
/** Are any indicators present at position?
  */
int ScintillaWrapper::IndicatorAllOnFor(int position);
/** What value does a particular indicator have at at a position?
  */
int ScintillaWrapper::IndicatorValueAt(int indicator, int position);
/** Where does a particular indicator start?
  */
int ScintillaWrapper::IndicatorStart(int indicator, int position);
/** Where does a particular indicator end?
  */
int ScintillaWrapper::IndicatorEnd(int indicator, int position);
/** Set number of entries in position cache
  */
void ScintillaWrapper::SetPositionCache(int size);
/** How many entries are allocated to the position cache?
  */
int ScintillaWrapper::GetPositionCache();
/** Copy the selection, if selection empty copy the line with the caret
  */
void ScintillaWrapper::CopyAllowLine();
/** Compact the document buffer and return a read-only pointer to the
  * characters in the document.
  */
int ScintillaWrapper::GetCharacterPointer();
/** Always interpret keyboard input as Unicode
  */
void ScintillaWrapper::SetKeysUnicode(bool keysUnicode);
/** Are keys always interpreted as Unicode?
  */
bool ScintillaWrapper::GetKeysUnicode();
/** Set the alpha fill colour of the given indicator.
  */
void ScintillaWrapper::IndicSetAlpha(int indicator, int alpha);
/** Get the alpha fill colour of the given indicator.
  */
int ScintillaWrapper::IndicGetAlpha(int indicator);
/** Set extra ascent for each line
  */
void ScintillaWrapper::SetExtraAscent(int extraAscent);
/** Get extra ascent for each line
  */
int ScintillaWrapper::GetExtraAscent();
/** Set extra descent for each line
  */
void ScintillaWrapper::SetExtraDescent(int extraDescent);
/** Get extra descent for each line
  */
int ScintillaWrapper::GetExtraDescent();
/** Which symbol was defined for markerNumber with MarkerDefine
  */
int ScintillaWrapper::MarkerSymbolDefined(int markerNumber);
/** Set the text in the text margin for a line
  */
void ScintillaWrapper::MarginSetText(int line, str text);
/** Get the text in the text margin for a line
  */
str ScintillaWrapper::MarginGetText();
/** Set the style number for the text margin for a line
  */
void ScintillaWrapper::MarginSetStyle(int line, int style);
/** Get the style number for the text margin for a line
  */
int ScintillaWrapper::MarginGetStyle(int line);
/** Set the style in the text margin for a line
  */
void ScintillaWrapper::MarginSetStyles(int line, str styles);
/** Get the styles in the text margin for a line
  */
str ScintillaWrapper::MarginGetStyles();
/** Clear the margin text on all lines
  */
void ScintillaWrapper::MarginTextClearAll();
/** Get the start of the range of style numbers used for margin text
  */
void ScintillaWrapper::MarginSetStyleOffset(int style);
/** Get the start of the range of style numbers used for margin text
  */
int ScintillaWrapper::MarginGetStyleOffset();
/** Set the annotation text for a line
  */
void ScintillaWrapper::AnnotationSetText(int line, str text);
/** Get the annotation text for a line
  */
str ScintillaWrapper::AnnotationGetText();
/** Set the style number for the annotations for a line
  */
void ScintillaWrapper::AnnotationSetStyle(int line, int style);
/** Get the style number for the annotations for a line
  */
int ScintillaWrapper::AnnotationGetStyle(int line);
/** Set the annotation styles for a line
  */
void ScintillaWrapper::AnnotationSetStyles(int line, str styles);
/** Get the annotation styles for a line
  */
str ScintillaWrapper::AnnotationGetStyles();
/** Get the number of annotation lines for a line
  */
int ScintillaWrapper::AnnotationGetLines(int line);
/** Clear the annotations from all lines
  */
void ScintillaWrapper::AnnotationClearAll();
/** Set the visibility for the annotations for a view
  */
void ScintillaWrapper::AnnotationSetVisible(int visible);
/** Get the visibility for the annotations for a view
  */
int ScintillaWrapper::AnnotationGetVisible();
/** Get the start of the range of style numbers used for annotations
  */
void ScintillaWrapper::AnnotationSetStyleOffset(int style);
/** Get the start of the range of style numbers used for annotations
  */
int ScintillaWrapper::AnnotationGetStyleOffset();
/** Add a container action to the undo stack
  */
void ScintillaWrapper::AddUndoAction(int token, int flags);
/** Find the position of a character from a point within the window.
  */
int ScintillaWrapper::CharPositionFromPoint(int x, int y);
/** Find the position of a character from a point within the window.
  * Return INVALID_POSITION if not close to text.
  */
int ScintillaWrapper::CharPositionFromPointClose(int x, int y);
/** Set whether multiple selections can be made
  */
void ScintillaWrapper::SetMultipleSelection(bool multipleSelection);
/** Whether multiple selections can be made
  */
bool ScintillaWrapper::GetMultipleSelection();
/** Set whether typing can be performed into multiple selections
  */
void ScintillaWrapper::SetAdditionalSelectionTyping(bool additionalSelectionTyping);
/** Whether typing can be performed into multiple selections
  */
bool ScintillaWrapper::GetAdditionalSelectionTyping();
/** Set whether additional carets will blink
  */
void ScintillaWrapper::SetAdditionalCaretsBlink(bool additionalCaretsBlink);
/** Whether additional carets will blink
  */
bool ScintillaWrapper::GetAdditionalCaretsBlink();
/** Set whether additional carets are visible
  */
void ScintillaWrapper::SetAdditionalCaretsVisible(bool additionalCaretsBlink);
/** Whether additional carets are visible
  */
bool ScintillaWrapper::GetAdditionalCaretsVisible();
/** How many selections are there?
  */
int ScintillaWrapper::GetSelections();
/** Clear selections to a single empty stream selection
  */
void ScintillaWrapper::ClearSelections();
/** Set a simple selection
  */
int ScintillaWrapper::SetSelection(int caret, int anchor);
/** Add a selection
  */
int ScintillaWrapper::AddSelection(int caret, int anchor);
/** Set the main selection
  */
void ScintillaWrapper::SetMainSelection(int selection);
/** Which selection is the main selection
  */
int ScintillaWrapper::GetMainSelection();
/** Which selection is the main selection
  */
void ScintillaWrapper::SetSelectionNCaret(int selection, int pos);
/** Which selection is the main selection
  */
int ScintillaWrapper::GetSelectionNCaret(int selection);
/** Which selection is the main selection
  */
void ScintillaWrapper::SetSelectionNAnchor(int selection, int posAnchor);
/** Which selection is the main selection
  */
int ScintillaWrapper::GetSelectionNAnchor(int selection);
/** Which selection is the main selection
  */
void ScintillaWrapper::SetSelectionNCaretVirtualSpace(int selection, int space);
/** Which selection is the main selection
  */
int ScintillaWrapper::GetSelectionNCaretVirtualSpace(int selection);
/** Which selection is the main selection
  */
void ScintillaWrapper::SetSelectionNAnchorVirtualSpace(int selection, int space);
/** Which selection is the main selection
  */
int ScintillaWrapper::GetSelectionNAnchorVirtualSpace(int selection);
/** Sets the position that starts the selection - this becomes the anchor.
  */
void ScintillaWrapper::SetSelectionNStart(int selection, int pos);
/** Returns the position at the start of the selection.
  */
int ScintillaWrapper::GetSelectionNStart(int selection);
/** Sets the position that ends the selection - this becomes the currentPosition.
  */
void ScintillaWrapper::SetSelectionNEnd(int selection, int pos);
/** Returns the position at the end of the selection.
  */
int ScintillaWrapper::GetSelectionNEnd(int selection);
/** Returns the position at the end of the selection.
  */
void ScintillaWrapper::SetRectangularSelectionCaret(int pos);
/** Returns the position at the end of the selection.
  */
int ScintillaWrapper::GetRectangularSelectionCaret();
/** Returns the position at the end of the selection.
  */
void ScintillaWrapper::SetRectangularSelectionAnchor(int posAnchor);
/** Returns the position at the end of the selection.
  */
int ScintillaWrapper::GetRectangularSelectionAnchor();
/** Returns the position at the end of the selection.
  */
void ScintillaWrapper::SetRectangularSelectionCaretVirtualSpace(int space);
/** Returns the position at the end of the selection.
  */
int ScintillaWrapper::GetRectangularSelectionCaretVirtualSpace();
/** Returns the position at the end of the selection.
  */
void ScintillaWrapper::SetRectangularSelectionAnchorVirtualSpace(int space);
/** Returns the position at the end of the selection.
  */
int ScintillaWrapper::GetRectangularSelectionAnchorVirtualSpace();
/** Returns the position at the end of the selection.
  */
void ScintillaWrapper::SetVirtualSpaceOptions(int virtualSpaceOptions);
/** Returns the position at the end of the selection.
  */
int ScintillaWrapper::GetVirtualSpaceOptions();
/** On GTK+, allow selecting the modifier key to use for mouse-based
  * rectangular selection. Often the window manager requires Alt+Mouse Drag
  * for moving windows.
  * Valid values are SCMOD_CTRL(default), SCMOD_ALT, or SCMOD_SUPER.
  */
void ScintillaWrapper::SetRectangularSelectionModifier(int modifier);
/** Get the modifier key used for rectangular selection.
  */
int ScintillaWrapper::GetRectangularSelectionModifier();
/** Set the foreground colour of additional selections.
  * Must have previously called SetSelFore with non-zero first argument for this to have an effect.
  */
void ScintillaWrapper::SetAdditionalSelFore(int foreRed, int foreGreen, int foreBlue);
/** Set the background colour of additional selections.
  * Must have previously called SetSelBack with non-zero first argument for this to have an effect.
  */
void ScintillaWrapper::SetAdditionalSelBack(int backRed, int backGreen, int backBlue);
/** Set the alpha of the selection.
  */
void ScintillaWrapper::SetAdditionalSelAlpha(int alpha);
/** Get the alpha of the selection.
  */
int ScintillaWrapper::GetAdditionalSelAlpha();
/** Set the foreground colour of additional carets.
  */
void ScintillaWrapper::SetAdditionalCaretFore(int foreRed, int foreGreen, int foreBlue);
/** Get the foreground colour of additional carets.
  */
colour ScintillaWrapper::GetAdditionalCaretFore();
/** Set the main selection to the next selection.
  */
void ScintillaWrapper::RotateSelection();
/** Swap that caret and anchor of the main selection.
  */
void ScintillaWrapper::SwapMainAnchorCaret();
/** Start notifying the container of all key presses and commands.
  */
void ScintillaWrapper::StartRecord();
/** Stop notifying the container of all key presses and commands.
  */
void ScintillaWrapper::StopRecord();
/** Set the lexing language of the document.
  */
void ScintillaWrapper::SetLexer(int lexer);
/** Retrieve the lexing language of the document.
  */
int ScintillaWrapper::GetLexer();
/** Colourise a segment of the document using the current lexing language.
  */
void ScintillaWrapper::Colourise(int start, int end);
/** Set up a value that may be used by a lexer for some optional feature.
  */
void ScintillaWrapper::SetProperty(str key, str value);
/** Set up the key words used by the lexer.
  */
void ScintillaWrapper::SetKeyWords(int keywordSet, str keyWords);
/** Set the lexing language of the document based on string name.
  */
void ScintillaWrapper::SetLexerLanguage(str language);
/** Load a lexer library (dll / so).
  */
void ScintillaWrapper::LoadLexerLibrary(str path);
/** Retrieve a "property" value previously set with SetProperty.
  */
str ScintillaWrapper::GetProperty();
/** Retrieve a "property" value previously set with SetProperty,
  * with "$()" variable replacement on returned buffer.
  */
str ScintillaWrapper::GetPropertyExpanded();
/** Retrieve a "property" value previously set with SetProperty,
  * interpreted as an int AFTER any "$()" variable replacement.
  */
int ScintillaWrapper::GetPropertyInt(str key);
/** Retrieve the number of bits the current lexer needs for styling.
  */
int ScintillaWrapper::GetStyleBitsNeeded();
/** Retrieve the name of the lexer.
  * Return the length of the text.
  */
str ScintillaWrapper::GetLexerLanguage();
