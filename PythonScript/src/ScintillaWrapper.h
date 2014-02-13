#ifndef _SCINTILLAWRAPPER_H
#define _SCINTILLAWRAPPER_H

#ifndef _SCINTILLACELL_H
#include "ScintillaCells.h"
#endif

#ifndef _PYPRODUCER_H
#include "PyProducerConsumer.h"
#endif

#ifndef REPLACER_20140209_H
#include "Replacer.h"
#endif


struct SCNotification;

#define COLOUR_RED(x)    (x & 0x0000FF)
#define COLOUR_GREEN(x)  ((x & 0x00FF00) >> 8)
#define COLOUR_BLUE(x)   ((x & 0xFF0000) >> 16)

#define MAKECOLOUR(x)    RGB(boost::python::extract<int>(x[0]),boost::python::extract<int>(x[1]),boost::python::extract<int>(x[2]))

struct out_of_bounds_exception : public std::exception
{
	const char * what() const throw() { return "Out of bounds exception"; }
};
namespace PythonScript
{
void translateOutOfBounds(out_of_bounds_exception const& e);
}

namespace NppPythonScript
{
    class Match;
    class ReplaceEntry;
}

struct CallbackExecArgs
{
	std::list<PyObject*> callbacks;
	boost::python::dict params;
};

class ScintillaWrapper : public NppPythonScript::PyProducerConsumer<CallbackExecArgs>
{
public:
	explicit ScintillaWrapper(HWND handle, HWND notepadHandle);
	virtual ~ScintillaWrapper();

	void setHandle(const HWND handle) { m_handle = handle; };
	HWND getHandle() { return m_handle; };
	void invalidateHandle() { m_handle = NULL; };

	void notify(SCNotification *notifyCode);
	bool addCallback(PyObject* callback, boost::python::list events);

	void clearAllCallbacks();
	void clearCallbackFunction(PyObject* callback);
	void clearCallbackEvents(boost::python::list events);
	void clearCallback(PyObject* callback, boost::python::list events);

	/* Helper functions 
	 * These functions are designed to make life easier for scripting,
	 * but don't perform any "magic"
	 */
	void forEachLine(PyObject* function);
	void deleteLine(int lineNumber);
	void replaceLine(int lineNumber, boost::python::object newContents);
	void replaceWholeLine(int lineNumber, boost::python::object newContents);
	boost::python::tuple getUserLineSelection();
	boost::python::tuple getUserCharSelection();
	void setTarget(int start, int end);
    void replacePlain(boost::python::object searchStr, boost::python::object replaceStr);
	void replacePlainFlags(boost::python::object searchStr, boost::python::object replaceStr, int flags);
    void replaceRegex(boost::python::object searchStr, boost::python::object replaceStr);
    void replaceRegexFlags(boost::python::object searchStr, boost::python::object replaceStr, int flags);

	void replaceImpl(boost::python::object searchStr, boost::python::object replaceStr, int count, NppPythonScript::python_re_flags flags, int startPosition, int endPosition);
	

	//static const int RE_INCLUDELINEENDINGS = 65536;
	/*
	void pyreplace(boost::python::object searchExp, boost::python::object replaceStr, boost::python::object count, boost::python::object flags, boost::python::object startLine, boost::python::object endLine);
	void pyreplaceNoFlagsNoCount(boost::python::object searchExp, boost::python::object replaceStr)
					{	pyreplace(searchExp, replaceStr, boost::python::object(0), boost::python::object(0), boost::python::object(), boost::python::object()); };
	void pyreplaceNoFlags(boost::python::object searchExp, boost::python::object replaceStr, boost::python::object count)
					{	pyreplace(searchExp, replaceStr, count, boost::python::object(0), boost::python::object(), boost::python::object()); };
	void pyreplaceNoStartEnd(boost::python::object searchExp, boost::python::object replaceStr, boost::python::object count, boost::python::object flags)
					{	pyreplace(searchExp, replaceStr, count, flags, boost::python::object(), boost::python::object()); };
	void pyreplaceNoEnd(boost::python::object searchExp, boost::python::object replaceStr, boost::python::object count, boost::python::object flags, boost::python::object startLine)
					{	pyreplace(searchExp, replaceStr, count, flags, startLine, boost::python::object()); };

	
	void pymlreplace(boost::python::object searchExp, boost::python::object replaceStr, boost::python::object count, boost::python::object flags, boost::python::object startPosition, boost::python::object endPosition);
	void pymlreplaceNoFlagsNoCount(boost::python::object searchExp, boost::python::object replaceStr)
					{	pymlreplace(searchExp, replaceStr, boost::python::object(0), boost::python::object(0), boost::python::object(), boost::python::object()); };
	void pymlreplaceNoFlags(boost::python::object searchExp, boost::python::object replaceStr, boost::python::object count)
					{	pymlreplace(searchExp, replaceStr, count, boost::python::object(0), boost::python::object(), boost::python::object()); };
	void pymlreplaceNoStartEnd(boost::python::object searchExp, boost::python::object replaceStr, boost::python::object count, boost::python::object flags)
					{	pymlreplace(searchExp, replaceStr, count, flags, boost::python::object(), boost::python::object()); };
	void pymlreplaceNoEnd(boost::python::object searchExp, boost::python::object replaceStr, boost::python::object count, boost::python::object flags, boost::python::object startPosition)
					{	pymlreplace(searchExp, replaceStr, count, flags, startPosition, boost::python::object()); };

	void pysearch(boost::python::object searchExp, boost::python::object callback, boost::python::object flags, boost::python::object startLine, boost::python::object endLine);
	void pysearchNoFlags(boost::python::object searchExp, boost::python::object callback)
					{	pysearch(searchExp, callback, boost::python::object(0), boost::python::object(), boost::python::object()); };
	void pysearchNoStartEnd(boost::python::object searchExp, boost::python::object callback, boost::python::object flags)
					{	pysearch(searchExp, callback, flags, boost::python::object(), boost::python::object()); };
	void pysearchNoEnd(boost::python::object searchExp, boost::python::object callback, boost::python::object flags, boost::python::object startLine)
					{	pysearch(searchExp, callback, flags, startLine, boost::python::object()); };

	void pymlsearch(boost::python::object searchExp, boost::python::object callback, boost::python::object flags, boost::python::object startPosition, boost::python::object endPosition);
	void pymlsearchNoFlags(boost::python::object searchExp, boost::python::object callback)
					{	pymlsearch(searchExp, callback, boost::python::object(0), boost::python::object(), boost::python::object()); };
	void pymlsearchNoStartEnd(boost::python::object searchExp, boost::python::object callback, boost::python::object flags)
					{	pymlsearch(searchExp, callback, flags, boost::python::object(), boost::python::object()); };
	void pymlsearchNoEnd(boost::python::object searchExp, boost::python::object callback, boost::python::object flags, boost::python::object startPosition)
					{	pymlsearch(searchExp, callback, flags, startPosition, boost::python::object()); };
	*/

	boost::python::str getWord(boost::python::object position, boost::python::object useOnlyWordChars);
	boost::python::str getWordNoFlags(boost::python::object position)
					{ return getWord(position, boost::python::object(true)); };
	boost::python::str getCurrentWord()
					{ return getWord(boost::python::object(), boost::python::object(true)); };
	
	// This "normal" Scintilla function has been overridden, as it returns a pointer, which we can convert to a string
	boost::python::str GetCharacterPointer();

	/* ++Autogenerated ---------------------------------------------------- */
	/** Add text to the document at current position.
  */
	int AddText(boost::python::object text);

	/** Add array of cells to document.
  */
	int AddStyledText(ScintillaCells c);

	/** Insert string at a position.
  */
	void InsertText(int pos, boost::python::str text);

	/** Delete all text in the document.
  */
	void ClearAll();

	/** Set all style bytes to 0, remove all folding information.
  */
	void ClearDocumentStyle();

	/** Returns the number of bytes in the document.
  */
	int GetLength();

	/** Returns the character byte at the position.
  */
	int GetCharAt(int pos);

	/** Returns the position of the caret.
  */
	int GetCurrentPos();

	/** Returns the position of the opposite end of the selection to the caret.
  */
	int GetAnchor();

	/** Returns the style byte at the position.
  */
	int GetStyleAt(int pos);

	/** Redoes the next action on the undo history.
  */
	void Redo();

	/** Choose between collecting actions into the undo
	  * history and discarding them.
  */
	void SetUndoCollection(bool collectUndo);

	/** Select all the text in the document.
  */
	void SelectAll();

	/** Remember the current position in the undo history as the position
	  * at which the document was saved.
  */
	void SetSavePoint();

	/** Retrieve a buffer of cells.
	  * Returns the number of bytes in the buffer not including terminating NULs.
  */
	boost::python::tuple GetStyledText(int start, int end);

	/** Are there any redoable actions in the undo history?
  */
	bool CanRedo();

	/** Retrieve the line number at which a particular marker is located.
  */
	int MarkerLineFromHandle(int handle);

	/** Delete a marker.
  */
	void MarkerDeleteHandle(int handle);

	/** Is undo history being collected?
  */
	bool GetUndoCollection();

	/** Are white space characters currently visible?
	  * Returns one of SCWS_* constants.
  */
	int GetViewWS();

	/** Make white space characters invisible, always visible or visible outside indentation.
  */
	void SetViewWS(int viewWS);

	/** Find the position from a point within the window.
  */
	int PositionFromPoint(int x, int y);

	/** Find the position from a point within the window but return
	  * INVALID_POSITION if not close to text.
  */
	int PositionFromPointClose(int x, int y);

	/** Set caret to start of a line and ensure it is visible.
  */
	void GotoLine(int line);

	/** Set caret to a position and ensure it is visible.
  */
	void GotoPos(int pos);

	/** Set the selection anchor to a position. The anchor is the opposite
	  * end of the selection from the caret.
  */
	void SetAnchor(int posAnchor);

	/** Retrieve the text of the line containing the caret.
	  * Returns the index of the caret on the line.
  */
	boost::python::str GetCurLine();

	/** Retrieve the position of the last correctly styled character.
  */
	int GetEndStyled();

	/** Convert all line endings in the document to one mode.
  */
	void ConvertEOLs(int eolMode);

	/** Retrieve the current end of line mode - one of CRLF, CR, or LF.
  */
	int GetEOLMode();

	/** Set the current end of line mode.
  */
	void SetEOLMode(int eolMode);

	/** Set the current styling position to pos and the styling mask to mask.
	  * The styling mask can be used to protect some bits in each styling byte from modification.
  */
	void StartStyling(int pos, int mask);

	/** Change style from current styling position for length characters to a style
	  * and move the current styling position to after this newly styled segment.
  */
	void SetStyling(int length, int style);

	/** Is drawing done first into a buffer or direct to the screen?
  */
	bool GetBufferedDraw();

	/** If drawing is buffered then each line of text is drawn into a bitmap buffer
	  * before drawing it to the screen to avoid flicker.
  */
	void SetBufferedDraw(bool buffered);

	/** Change the visible size of a tab to be a multiple of the width of a space character.
  */
	void SetTabWidth(int tabWidth);

	/** Retrieve the visible size of a tab.
  */
	int GetTabWidth();

	/** Set the code page used to interpret the bytes of the document as characters.
	  * The SC_CP_UTF8 value can be used to enter Unicode mode.
  */
	void SetCodePage(int codePage);

	/** In palette mode, Scintilla uses the environment's palette calls to display
	  * more colours. This may lead to ugly displays.
  */
	void SetUsePalette(bool usePalette);

	/** Set the symbol used for a particular marker number.
  */
	void MarkerDefine(int markerNumber, int markerSymbol);

	/** Set the foreground colour used for a particular marker number.
  */
	void MarkerSetFore(int markerNumber, boost::python::tuple fore);

	/** Set the background colour used for a particular marker number.
  */
	void MarkerSetBack(int markerNumber, boost::python::tuple back);

	/** Add a marker to a line, returning an ID which can be used to find or delete the marker.
  */
	int MarkerAdd(int line, int markerNumber);

	/** Delete a marker from a line.
  */
	void MarkerDelete(int line, int markerNumber);

	/** Delete all markers with a particular number from all lines.
  */
	void MarkerDeleteAll(int markerNumber);

	/** Get a bit mask of all the markers set on a line.
  */
	int MarkerGet(int line);

	/** Find the next line at or after lineStart that includes a marker in mask.
	  * Return -1 when no more lines.
  */
	int MarkerNext(int lineStart, int markerMask);

	/** Find the previous line before lineStart that includes a marker in mask.
  */
	int MarkerPrevious(int lineStart, int markerMask);

	/** Define a marker from a pixmap.
  */
	void MarkerDefinePixmap(int markerNumber, boost::python::str pixmap);

	/** Add a set of markers to a line.
  */
	void MarkerAddSet(int line, int set);

	/** Set the alpha used for a marker that is drawn in the text area, not the margin.
  */
	void MarkerSetAlpha(int markerNumber, int alpha);

	/** Set a margin to be either numeric or symbolic.
  */
	void SetMarginTypeN(int margin, int marginType);

	/** Retrieve the type of a margin.
  */
	int GetMarginTypeN(int margin);

	/** Set the width of a margin to a width expressed in pixels.
  */
	void SetMarginWidthN(int margin, int pixelWidth);

	/** Retrieve the width of a margin in pixels.
  */
	int GetMarginWidthN(int margin);

	/** Set a mask that determines which markers are displayed in a margin.
  */
	void SetMarginMaskN(int margin, int mask);

	/** Retrieve the marker mask of a margin.
  */
	int GetMarginMaskN(int margin);

	/** Make a margin sensitive or insensitive to mouse clicks.
  */
	void SetMarginSensitiveN(int margin, bool sensitive);

	/** Retrieve the mouse click sensitivity of a margin.
  */
	bool GetMarginSensitiveN(int margin);

	/** Set the cursor shown when the mouse is inside a margin.
  */
	void SetMarginCursorN(int margin, int cursor);

	/** Retrieve the cursor shown in a margin.
  */
	int GetMarginCursorN(int margin);

	/** Clear all the styles and make equivalent to the global default style.
  */
	void StyleClearAll();

	/** Set the foreground colour of a style.
  */
	void StyleSetFore(int style, boost::python::tuple fore);

	/** Set the background colour of a style.
  */
	void StyleSetBack(int style, boost::python::tuple back);

	/** Set a style to be bold or not.
  */
	void StyleSetBold(int style, bool bold);

	/** Set a style to be italic or not.
  */
	void StyleSetItalic(int style, bool italic);

	/** Set the size of characters of a style.
  */
	void StyleSetSize(int style, int sizePoints);

	/** Set the font of a style.
  */
	void StyleSetFont(int style, boost::python::str fontName);

	/** Set a style to have its end of line filled or not.
  */
	void StyleSetEOLFilled(int style, bool filled);

	/** Reset the default style to its state at startup
  */
	void StyleResetDefault();

	/** Set a style to be underlined or not.
  */
	void StyleSetUnderline(int style, bool underline);

	/** Get the foreground colour of a style.
  */
	boost::python::tuple StyleGetFore(int style);

	/** Get the background colour of a style.
  */
	boost::python::tuple StyleGetBack(int style);

	/** Get is a style bold or not.
  */
	bool StyleGetBold(int style);

	/** Get is a style italic or not.
  */
	bool StyleGetItalic(int style);

	/** Get the size of characters of a style.
  */
	int StyleGetSize(int style);

	/** Get the font of a style.
	  * Returns the length of the fontName
  */
	boost::python::str StyleGetFont();

	/** Get is a style to have its end of line filled or not.
  */
	bool StyleGetEOLFilled(int style);

	/** Get is a style underlined or not.
  */
	bool StyleGetUnderline(int style);

	/** Get is a style mixed case, or to force upper or lower case.
  */
	int StyleGetCase(int style);

	/** Get the character get of the font in a style.
  */
	int StyleGetCharacterSet(int style);

	/** Get is a style visible or not.
  */
	bool StyleGetVisible(int style);

	/** Get is a style changeable or not (read only).
	  * Experimental feature, currently buggy.
  */
	bool StyleGetChangeable(int style);

	/** Get is a style a hotspot or not.
  */
	bool StyleGetHotSpot(int style);

	/** Set a style to be mixed case, or to force upper or lower case.
  */
	void StyleSetCase(int style, int caseForce);

	/** Set the character set of the font in a style.
  */
	void StyleSetCharacterSet(int style, int characterSet);

	/** Set a style to be a hotspot or not.
  */
	void StyleSetHotSpot(int style, bool hotspot);

	/** Set the foreground colour of the main and additional selections and whether to use this setting.
  */
	void SetSelFore(bool useSetting, boost::python::tuple fore);

	/** Set the background colour of the main and additional selections and whether to use this setting.
  */
	void SetSelBack(bool useSetting, boost::python::tuple back);

	/** Get the alpha of the selection.
  */
	int GetSelAlpha();

	/** Set the alpha of the selection.
  */
	void SetSelAlpha(int alpha);

	/** Is the selection end of line filled?
  */
	bool GetSelEOLFilled();

	/** Set the selection to have its end of line filled or not.
  */
	void SetSelEOLFilled(bool filled);

	/** Set the foreground colour of the caret.
  */
	void SetCaretFore(boost::python::tuple fore);

	/** When key+modifier combination km is pressed perform msg.
  */
	void AssignCmdKey(int km, int msg);

	/** When key+modifier combination km is pressed do nothing.
  */
	void ClearCmdKey(int km);

	/** Drop all key mappings.
  */
	void ClearAllCmdKeys();

	/** Set the styles for a segment of the document.
  */
	int SetStylingEx(boost::python::object styles);

	/** Set a style to be visible or not.
  */
	void StyleSetVisible(int style, bool visible);

	/** Get the time in milliseconds that the caret is on and off.
  */
	int GetCaretPeriod();

	/** Get the time in milliseconds that the caret is on and off. 0 = steady on.
  */
	void SetCaretPeriod(int periodMilliseconds);

	/** Set the set of characters making up words for when moving or selecting by word.
	  * First sets defaults like SetCharsDefault.
  */
	void SetWordChars(boost::python::str characters);

	/** Start a sequence of actions that is undone and redone as a unit.
	  * May be nested.
  */
	void BeginUndoAction();

	/** End a sequence of actions that is undone and redone as a unit.
  */
	void EndUndoAction();

	/** Set an indicator to plain, squiggle or TT.
  */
	void IndicSetStyle(int indic, int style);

	/** Retrieve the style of an indicator.
  */
	int IndicGetStyle(int indic);

	/** Set the foreground colour of an indicator.
  */
	void IndicSetFore(int indic, boost::python::tuple fore);

	/** Retrieve the foreground colour of an indicator.
  */
	boost::python::tuple IndicGetFore(int indic);

	/** Set an indicator to draw under text or over(default).
  */
	void IndicSetUnder(int indic, bool under);

	/** Retrieve whether indicator drawn under or over text.
  */
	bool IndicGetUnder(int indic);

	/** Set the foreground colour of all whitespace and whether to use this setting.
  */
	void SetWhitespaceFore(bool useSetting, boost::python::tuple fore);

	/** Set the background colour of all whitespace and whether to use this setting.
  */
	void SetWhitespaceBack(bool useSetting, boost::python::tuple back);

	/** Set the size of the dots used to mark space characters.
  */
	void SetWhitespaceSize(int size);

	/** Get the size of the dots used to mark space characters.
  */
	int GetWhitespaceSize();

	/** Divide each styling byte into lexical class bits (default: 5) and indicator
	  * bits (default: 3). If a lexer requires more than 32 lexical states, then this
	  * is used to expand the possible states.
  */
	void SetStyleBits(int bits);

	/** Retrieve number of bits in style bytes used to hold the lexical state.
  */
	int GetStyleBits();

	/** Used to hold extra styling information for each line.
  */
	void SetLineState(int line, int state);

	/** Retrieve the extra styling information for a line.
  */
	int GetLineState(int line);

	/** Retrieve the last line number that has line state.
  */
	int GetMaxLineState();

	/** Is the background of the line containing the caret in a different colour?
  */
	bool GetCaretLineVisible();

	/** Display the background of the line containing the caret in a different colour.
  */
	void SetCaretLineVisible(bool show);

	/** Get the colour of the background of the line containing the caret.
  */
	boost::python::tuple GetCaretLineBack();

	/** Set the colour of the background of the line containing the caret.
  */
	void SetCaretLineBack(boost::python::tuple back);

	/** Set a style to be changeable or not (read only).
	  * Experimental feature, currently buggy.
  */
	void StyleSetChangeable(int style, bool changeable);

	/** Display a auto-completion list.
	  * The lenEntered parameter indicates how many characters before
	  * the caret should be used to provide context.
  */
	void AutoCShow(int lenEntered, boost::python::str itemList);

	/** Remove the auto-completion list from the screen.
  */
	void AutoCCancel();

	/** Is there an auto-completion list visible?
  */
	bool AutoCActive();

	/** Retrieve the position of the caret when the auto-completion list was displayed.
  */
	int AutoCPosStart();

	/** User has selected an item so remove the list and insert the selection.
  */
	void AutoCComplete();

	/** Define a set of character that when typed cancel the auto-completion list.
  */
	void AutoCStops(boost::python::str characterSet);

	/** Change the separator character in the string setting up an auto-completion list.
	  * Default is space but can be changed if items contain space.
  */
	void AutoCSetSeparator(int separatorCharacter);

	/** Retrieve the auto-completion list separator character.
  */
	int AutoCGetSeparator();

	/** Select the item in the auto-completion list that starts with a string.
  */
	void AutoCSelect(boost::python::str text);

	/** Should the auto-completion list be cancelled if the user backspaces to a
	  * position before where the box was created.
  */
	void AutoCSetCancelAtStart(bool cancel);

	/** Retrieve whether auto-completion cancelled by backspacing before start.
  */
	bool AutoCGetCancelAtStart();

	/** Define a set of characters that when typed will cause the autocompletion to
	  * choose the selected item.
  */
	void AutoCSetFillUps(boost::python::str characterSet);

	/** Should a single item auto-completion list automatically choose the item.
  */
	void AutoCSetChooseSingle(bool chooseSingle);

	/** Retrieve whether a single item auto-completion list automatically choose the item.
  */
	bool AutoCGetChooseSingle();

	/** Set whether case is significant when performing auto-completion searches.
  */
	void AutoCSetIgnoreCase(bool ignoreCase);

	/** Retrieve state of ignore case flag.
  */
	bool AutoCGetIgnoreCase();

	/** Display a list of strings and send notification when user chooses one.
  */
	void UserListShow(int listType, boost::python::str itemList);

	/** Set whether or not autocompletion is hidden automatically when nothing matches.
  */
	void AutoCSetAutoHide(bool autoHide);

	/** Retrieve whether or not autocompletion is hidden automatically when nothing matches.
  */
	bool AutoCGetAutoHide();

	/** Set whether or not autocompletion deletes any word characters
	  * after the inserted text upon completion.
  */
	void AutoCSetDropRestOfWord(bool dropRestOfWord);

	/** Retrieve whether or not autocompletion deletes any word characters
	  * after the inserted text upon completion.
  */
	bool AutoCGetDropRestOfWord();

	/** Register an XPM image for use in autocompletion lists.
  */
	void RegisterImage(int type, boost::python::str xpmData);

	/** Clear all the registered XPM images.
  */
	void ClearRegisteredImages();

	/** Retrieve the auto-completion list type-separator character.
  */
	int AutoCGetTypeSeparator();

	/** Change the type-separator character in the string setting up an auto-completion list.
	  * Default is '?' but can be changed if items contain '?'.
  */
	void AutoCSetTypeSeparator(int separatorCharacter);

	/** Set the maximum width, in characters, of auto-completion and user lists.
	  * Set to 0 to autosize to fit longest item, which is the default.
  */
	void AutoCSetMaxWidth(int characterCount);

	/** Get the maximum width, in characters, of auto-completion and user lists.
  */
	int AutoCGetMaxWidth();

	/** Set the maximum height, in rows, of auto-completion and user lists.
	  * The default is 5 rows.
  */
	void AutoCSetMaxHeight(int rowCount);

	/** Set the maximum height, in rows, of auto-completion and user lists.
  */
	int AutoCGetMaxHeight();

	/** Set the number of spaces used for one level of indentation.
  */
	void SetIndent(int indentSize);

	/** Retrieve indentation size.
  */
	int GetIndent();

	/** Indentation will only use space characters if useTabs is false, otherwise
	  * it will use a combination of tabs and spaces.
  */
	void SetUseTabs(bool useTabs);

	/** Retrieve whether tabs will be used in indentation.
  */
	bool GetUseTabs();

	/** Change the indentation of a line to a number of columns.
  */
	void SetLineIndentation(int line, int indentSize);

	/** Retrieve the number of columns that a line is indented.
  */
	int GetLineIndentation(int line);

	/** Retrieve the position before the first non indentation character on a line.
  */
	int GetLineIndentPosition(int line);

	/** Retrieve the column number of a position, taking tab width into account.
  */
	int GetColumn(int pos);

	/** Show or hide the horizontal scroll bar.
  */
	void SetHScrollBar(bool show);

	/** Is the horizontal scroll bar visible?
  */
	bool GetHScrollBar();

	/** Show or hide indentation guides.
  */
	void SetIndentationGuides(int indentView);

	/** Are the indentation guides visible?
  */
	int GetIndentationGuides();

	/** Set the highlighted indentation guide column.
	  * 0 = no highlighted guide.
  */
	void SetHighlightGuide(int column);

	/** Get the highlighted indentation guide column.
  */
	int GetHighlightGuide();

	/** Get the position after the last visible characters on a line.
  */
	int GetLineEndPosition(int line);

	/** Get the code page used to interpret the bytes of the document as characters.
  */
	int GetCodePage();

	/** Get the foreground colour of the caret.
  */
	boost::python::tuple GetCaretFore();

	/** In palette mode?
  */
	bool GetUsePalette();

	/** In read-only mode?
  */
	bool GetReadOnly();

	/** Sets the position of the caret.
  */
	void SetCurrentPos(int pos);

	/** Sets the position that starts the selection - this becomes the anchor.
  */
	void SetSelectionStart(int pos);

	/** Returns the position at the start of the selection.
  */
	int GetSelectionStart();

	/** Sets the position that ends the selection - this becomes the currentPosition.
  */
	void SetSelectionEnd(int pos);

	/** Returns the position at the end of the selection.
  */
	int GetSelectionEnd();

	/** Sets the print magnification added to the point size of each style for printing.
  */
	void SetPrintMagnification(int magnification);

	/** Returns the print magnification.
  */
	int GetPrintMagnification();

	/** Modify colours when printing for clearer printed text.
  */
	void SetPrintColourMode(int mode);

	/** Returns the print colour mode.
  */
	int GetPrintColourMode();

	/** Find some text in the document.
  */
	boost::python::object FindText(int flags, int start, int end, boost::python::object ft);

	/** Retrieve the display line at the top of the display.
  */
	int GetFirstVisibleLine();

	/** Retrieve the contents of a line.
	  * Returns the length of the line.
  */
	boost::python::str GetLine(int line);

	/** Returns the number of lines in the document. There is always at least one.
  */
	int GetLineCount();

	/** Sets the size in pixels of the left margin.
  */
	void SetMarginLeft(int pixelWidth);

	/** Returns the size in pixels of the left margin.
  */
	int GetMarginLeft();

	/** Sets the size in pixels of the right margin.
  */
	void SetMarginRight(int pixelWidth);

	/** Returns the size in pixels of the right margin.
  */
	int GetMarginRight();

	/** Is the document different from when it was last saved?
  */
	bool GetModify();

	/** Select a range of text.
  */
	void SetSel(int start, int end);

	/** Retrieve the selected text.
	  * Return the length of the text.
  */
	boost::python::str GetSelText();

	/** Retrieve a range of text.
	  * Return the length of the text.
  */
	boost::python::str GetTextRange(int start, int end);

	/** Draw the selection in normal style or with selection highlighted.
  */
	void HideSelection(bool normal);

	/** Retrieve the x value of the point in the window where a position is displayed.
  */
	int PointXFromPosition(int pos);

	/** Retrieve the y value of the point in the window where a position is displayed.
  */
	int PointYFromPosition(int pos);

	/** Retrieve the line containing a position.
  */
	int LineFromPosition(int pos);

	/** Retrieve the position at the start of a line.
  */
	int PositionFromLine(int line);

	/** Scroll horizontally and vertically.
  */
	void LineScroll(int columns, int lines);

	/** Ensure the caret is visible.
  */
	void ScrollCaret();

	/** Replace the selected text with the argument text.
  */
	void ReplaceSel(boost::python::str text);

	/** Set to read only or read write.
  */
	void SetReadOnly(bool readOnly);

	/** Null operation.
  */
	void Null();

	/** Will a paste succeed?
  */
	bool CanPaste();

	/** Are there any undoable actions in the undo history?
  */
	bool CanUndo();

	/** Delete the undo history.
  */
	void EmptyUndoBuffer();

	/** Undo one action in the undo history.
  */
	void Undo();

	/** Cut the selection to the clipboard.
  */
	void Cut();

	/** Copy the selection to the clipboard.
  */
	void Copy();

	/** Paste the contents of the clipboard into the document replacing the selection.
  */
	void Paste();

	/** Clear the selection.
  */
	void Clear();

	/** Replace the contents of the document with the argument text.
  */
	void SetText(boost::python::str text);

	/** Retrieve all the text in the document.
	  * Returns number of characters retrieved.
  */
	boost::python::str GetText();

	/** Retrieve the number of characters in the document.
  */
	int GetTextLength();

	/** Retrieve a pointer to a function that processes messages for this Scintilla.
  */
	int GetDirectFunction();

	/** Retrieve a pointer value to use as the first argument when calling
	  * the function returned by GetDirectFunction.
  */
	int GetDirectPointer();

	/** Set to overtype (true) or insert mode.
  */
	void SetOvertype(bool overtype);

	/** Returns true if overtype mode is active otherwise false is returned.
  */
	bool GetOvertype();

	/** Set the width of the insert mode caret.
  */
	void SetCaretWidth(int pixelWidth);

	/** Returns the width of the insert mode caret.
  */
	int GetCaretWidth();

	/** Sets the position that starts the target which is used for updating the
	  * document without affecting the scroll position.
  */
	void SetTargetStart(int pos);

	/** Get the position that starts the target.
  */
	int GetTargetStart();

	/** Sets the position that ends the target which is used for updating the
	  * document without affecting the scroll position.
  */
	void SetTargetEnd(int pos);

	/** Get the position that ends the target.
  */
	int GetTargetEnd();

	/** Replace the target text with the argument text.
	  * Text is counted so it can contain NULs.
	  * Returns the length of the replacement text.
  */
	int ReplaceTarget(boost::python::object text);

	/** Replace the target text with the argument text after \d processing.
	  * Text is counted so it can contain NULs.
	  * Looks for \d where d is between 1 and 9 and replaces these with the strings
	  * matched in the last search operation which were surrounded by \( and \).
	  * Returns the length of the replacement text including any change
	  * caused by processing the \d patterns.
  */
	int ReplaceTargetRE(boost::python::object text);

	/** Search for a counted string in the target and set the target to the found
	  * range. Text is counted so it can contain NULs.
	  * Returns length of range or -1 for failure in which case target is not moved.
  */
	int SearchInTarget(boost::python::object text);

	/** Set the search flags used by SearchInTarget.
  */
	void SetSearchFlags(int flags);

	/** Get the search flags used by SearchInTarget.
  */
	int GetSearchFlags();

	/** Show a call tip containing a definition near position pos.
  */
	void CallTipShow(int pos, boost::python::str definition);

	/** Remove the call tip from the screen.
  */
	void CallTipCancel();

	/** Is there an active call tip?
  */
	bool CallTipActive();

	/** Retrieve the position where the caret was before displaying the call tip.
  */
	int CallTipPosStart();

	/** Highlight a segment of the definition.
  */
	void CallTipSetHlt(int start, int end);

	/** Set the background colour for the call tip.
  */
	void CallTipSetBack(boost::python::tuple back);

	/** Set the foreground colour for the call tip.
  */
	void CallTipSetFore(boost::python::tuple fore);

	/** Set the foreground colour for the highlighted part of the call tip.
  */
	void CallTipSetForeHlt(boost::python::tuple fore);

	/** Enable use of STYLE_CALLTIP and set call tip tab size in pixels.
  */
	void CallTipUseStyle(int tabSize);

	/** Find the display line of a document line taking hidden lines into account.
  */
	int VisibleFromDocLine(int line);

	/** Find the document line of a display line taking hidden lines into account.
  */
	int DocLineFromVisible(int lineDisplay);

	/** The number of display lines needed to wrap a document line
  */
	int WrapCount(int line);

	/** Set the fold level of a line.
	  * This encodes an integer level along with flags indicating whether the
	  * line is a header and whether it is effectively white space.
  */
	void SetFoldLevel(int line, int level);

	/** Retrieve the fold level of a line.
  */
	int GetFoldLevel(int line);

	/** Find the last child line of a header line.
  */
	int GetLastChild(int line, int level);

	/** Find the parent line of a child line.
  */
	int GetFoldParent(int line);

	/** Make a range of lines visible.
  */
	void ShowLines(int lineStart, int lineEnd);

	/** Make a range of lines invisible.
  */
	void HideLines(int lineStart, int lineEnd);

	/** Is a line visible?
  */
	bool GetLineVisible(int line);

	/** Show the children of a header line.
  */
	void SetFoldExpanded(int line, bool expanded);

	/** Is a header line expanded?
  */
	bool GetFoldExpanded(int line);

	/** Switch a header line between expanded and contracted.
  */
	void ToggleFold(int line);

	/** Ensure a particular line is visible by expanding any header line hiding it.
  */
	void EnsureVisible(int line);

	/** Set some style options for folding.
  */
	void SetFoldFlags(int flags);

	/** Ensure a particular line is visible by expanding any header line hiding it.
	  * Use the currently set visibility policy to determine which range to display.
  */
	void EnsureVisibleEnforcePolicy(int line);

	/** Sets whether a tab pressed when caret is within indentation indents.
  */
	void SetTabIndents(bool tabIndents);

	/** Does a tab pressed when caret is within indentation indent?
  */
	bool GetTabIndents();

	/** Sets whether a backspace pressed when caret is within indentation unindents.
  */
	void SetBackSpaceUnIndents(bool bsUnIndents);

	/** Does a backspace pressed when caret is within indentation unindent?
  */
	bool GetBackSpaceUnIndents();

	/** Sets the time the mouse must sit still to generate a mouse dwell event.
  */
	void SetMouseDwellTime(int periodMilliseconds);

	/** Retrieve the time the mouse must sit still to generate a mouse dwell event.
  */
	int GetMouseDwellTime();

	/** Get position of start of word.
  */
	int WordStartPosition(int pos, bool onlyWordCharacters);

	/** Get position of end of word.
  */
	int WordEndPosition(int pos, bool onlyWordCharacters);

	/** Sets whether text is word wrapped.
  */
	void SetWrapMode(int mode);

	/** Retrieve whether text is word wrapped.
  */
	int GetWrapMode();

	/** Set the display mode of visual flags for wrapped lines.
  */
	void SetWrapVisualFlags(int wrapVisualFlags);

	/** Retrive the display mode of visual flags for wrapped lines.
  */
	int GetWrapVisualFlags();

	/** Set the location of visual flags for wrapped lines.
  */
	void SetWrapVisualFlagsLocation(int wrapVisualFlagsLocation);

	/** Retrive the location of visual flags for wrapped lines.
  */
	int GetWrapVisualFlagsLocation();

	/** Set the start indent for wrapped lines.
  */
	void SetWrapStartIndent(int indent);

	/** Retrive the start indent for wrapped lines.
  */
	int GetWrapStartIndent();

	/** Sets how wrapped sublines are placed. Default is fixed.
  */
	void SetWrapIndentMode(int mode);

	/** Retrieve how wrapped sublines are placed. Default is fixed.
  */
	int GetWrapIndentMode();

	/** Sets the degree of caching of layout information.
  */
	void SetLayoutCache(int mode);

	/** Retrieve the degree of caching of layout information.
  */
	int GetLayoutCache();

	/** Sets the document width assumed for scrolling.
  */
	void SetScrollWidth(int pixelWidth);

	/** Retrieve the document width assumed for scrolling.
  */
	int GetScrollWidth();

	/** Sets whether the maximum width line displayed is used to set scroll width.
  */
	void SetScrollWidthTracking(bool tracking);

	/** Retrieve whether the scroll width tracks wide lines.
  */
	bool GetScrollWidthTracking();

	/** Measure the pixel width of some text in a particular style.
	  * NUL terminated text argument.
	  * Does not handle tab or control characters.
  */
	int TextWidth(int style, boost::python::str text);

	/** Sets the scroll range so that maximum scroll position has
	  * the last line at the bottom of the view (default).
	  * Setting this to false allows scrolling one page below the last line.
  */
	void SetEndAtLastLine(bool endAtLastLine);

	/** Retrieve whether the maximum scroll position has the last
	  * line at the bottom of the view.
  */
	bool GetEndAtLastLine();

	/** Retrieve the height of a particular line of text in pixels.
  */
	int TextHeight(int line);

	/** Show or hide the vertical scroll bar.
  */
	void SetVScrollBar(bool show);

	/** Is the vertical scroll bar visible?
  */
	bool GetVScrollBar();

	/** Append a string to the end of the document without changing the selection.
  */
	int AppendText(boost::python::object text);

	/** Is drawing done in two phases with backgrounds drawn before faoregrounds?
  */
	bool GetTwoPhaseDraw();

	/** In twoPhaseDraw mode, drawing is performed in two phases, first the background
	  * and then the foreground. This avoids chopping off characters that overlap the next run.
  */
	void SetTwoPhaseDraw(bool twoPhase);

	/** Choose the quality level for text from the FontQuality enumeration.
  */
	void SetFontQuality(int fontQuality);

	/** Retrieve the quality level for text.
  */
	int GetFontQuality();

	/** Scroll so that a display line is at the top of the display.
  */
	void SetFirstVisibleLine(int lineDisplay);

	/** Change the effect of pasting when there are multiple selections.
  */
	void SetMultiPaste(int multiPaste);

	/** Retrieve the effect of pasting when there are multiple selections..
  */
	int GetMultiPaste();

	/** Retrieve the value of a tag from a regular expression search.
  */
	boost::python::str GetTag();

	/** Make the target range start and end be the same as the selection range start and end.
  */
	void TargetFromSelection();

	/** Join the lines in the target.
  */
	void LinesJoin();

	/** Split the lines in the target into lines that are less wide than pixelWidth
	  * where possible.
  */
	void LinesSplit(int pixelWidth);

	/** Set the colours used as a chequerboard pattern in the fold margin
  */
	void SetFoldMarginColour(bool useSetting, boost::python::tuple back);

	/** Set the colours used as a chequerboard pattern in the fold margin
  */
	void SetFoldMarginHiColour(bool useSetting, boost::python::tuple fore);

	/** Move caret down one line.
  */
	void LineDown();

	/** Move caret down one line extending selection to new caret position.
  */
	void LineDownExtend();

	/** Move caret up one line.
  */
	void LineUp();

	/** Move caret up one line extending selection to new caret position.
  */
	void LineUpExtend();

	/** Move caret left one character.
  */
	void CharLeft();

	/** Move caret left one character extending selection to new caret position.
  */
	void CharLeftExtend();

	/** Move caret right one character.
  */
	void CharRight();

	/** Move caret right one character extending selection to new caret position.
  */
	void CharRightExtend();

	/** Move caret left one word.
  */
	void WordLeft();

	/** Move caret left one word extending selection to new caret position.
  */
	void WordLeftExtend();

	/** Move caret right one word.
  */
	void WordRight();

	/** Move caret right one word extending selection to new caret position.
  */
	void WordRightExtend();

	/** Move caret to first position on line.
  */
	void Home();

	/** Move caret to first position on line extending selection to new caret position.
  */
	void HomeExtend();

	/** Move caret to last position on line.
  */
	void LineEnd();

	/** Move caret to last position on line extending selection to new caret position.
  */
	void LineEndExtend();

	/** Move caret to first position in document.
  */
	void DocumentStart();

	/** Move caret to first position in document extending selection to new caret position.
  */
	void DocumentStartExtend();

	/** Move caret to last position in document.
  */
	void DocumentEnd();

	/** Move caret to last position in document extending selection to new caret position.
  */
	void DocumentEndExtend();

	/** Move caret one page up.
  */
	void PageUp();

	/** Move caret one page up extending selection to new caret position.
  */
	void PageUpExtend();

	/** Move caret one page down.
  */
	void PageDown();

	/** Move caret one page down extending selection to new caret position.
  */
	void PageDownExtend();

	/** Switch from insert to overtype mode or the reverse.
  */
	void EditToggleOvertype();

	/** Cancel any modes such as call tip or auto-completion list display.
  */
	void Cancel();

	/** Delete the selection or if no selection, the character before the caret.
  */
	void DeleteBack();

	/** If selection is empty or all on one line replace the selection with a tab character.
	  * If more than one line selected, indent the lines.
  */
	void Tab();

	/** Dedent the selected lines.
  */
	void BackTab();

	/** Insert a new line, may use a CRLF, CR or LF depending on EOL mode.
  */
	void NewLine();

	/** Insert a Form Feed character.
  */
	void FormFeed();

	/** Move caret to before first visible character on line.
	  * If already there move to first character on line.
  */
	void VCHome();

	/** Like VCHome but extending selection to new caret position.
  */
	void VCHomeExtend();

	/** Magnify the displayed text by increasing the sizes by 1 point.
  */
	void ZoomIn();

	/** Make the displayed text smaller by decreasing the sizes by 1 point.
  */
	void ZoomOut();

	/** Delete the word to the left of the caret.
  */
	void DelWordLeft();

	/** Delete the word to the right of the caret.
  */
	void DelWordRight();

	/** Delete the word to the right of the caret, but not the trailing non-word characters.
  */
	void DelWordRightEnd();

	/** Cut the line containing the caret.
  */
	void LineCut();

	/** Delete the line containing the caret.
  */
	void LineDelete();

	/** Switch the current line with the previous.
  */
	void LineTranspose();

	/** Duplicate the current line.
  */
	void LineDuplicate();

	/** Transform the selection to lower case.
  */
	void LowerCase();

	/** Transform the selection to upper case.
  */
	void UpperCase();

	/** Scroll the document down, keeping the caret visible.
  */
	void LineScrollDown();

	/** Scroll the document up, keeping the caret visible.
  */
	void LineScrollUp();

	/** Delete the selection or if no selection, the character before the caret.
	  * Will not delete the character before at the start of a line.
  */
	void DeleteBackNotLine();

	/** Move caret to first position on display line.
  */
	void HomeDisplay();

	/** Move caret to first position on display line extending selection to
	  * new caret position.
  */
	void HomeDisplayExtend();

	/** Move caret to last position on display line.
  */
	void LineEndDisplay();

	/** Move caret to last position on display line extending selection to new
	  * caret position.
  */
	void LineEndDisplayExtend();

	/** These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
	  * except they behave differently when word-wrap is enabled:
	  * They go first to the start / end of the display line, like (Home|LineEnd)Display
	  * The difference is that, the cursor is already at the point, it goes on to the start
	  * or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
  */
	void HomeWrap();

	/** These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
	  * except they behave differently when word-wrap is enabled:
	  * They go first to the start / end of the display line, like (Home|LineEnd)Display
	  * The difference is that, the cursor is already at the point, it goes on to the start
	  * or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
  */
	void HomeWrapExtend();

	/** These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
	  * except they behave differently when word-wrap is enabled:
	  * They go first to the start / end of the display line, like (Home|LineEnd)Display
	  * The difference is that, the cursor is already at the point, it goes on to the start
	  * or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
  */
	void LineEndWrap();

	/** These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
	  * except they behave differently when word-wrap is enabled:
	  * They go first to the start / end of the display line, like (Home|LineEnd)Display
	  * The difference is that, the cursor is already at the point, it goes on to the start
	  * or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
  */
	void LineEndWrapExtend();

	/** These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
	  * except they behave differently when word-wrap is enabled:
	  * They go first to the start / end of the display line, like (Home|LineEnd)Display
	  * The difference is that, the cursor is already at the point, it goes on to the start
	  * or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
  */
	void VCHomeWrap();

	/** These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
	  * except they behave differently when word-wrap is enabled:
	  * They go first to the start / end of the display line, like (Home|LineEnd)Display
	  * The difference is that, the cursor is already at the point, it goes on to the start
	  * or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
  */
	void VCHomeWrapExtend();

	/** Copy the line containing the caret.
  */
	void LineCopy();

	/** Move the caret inside current view if it's not there already.
  */
	void MoveCaretInsideView();

	/** How many characters are on a line, including end of line characters?
  */
	int LineLength(int line);

	/** Highlight the characters at two positions.
  */
	void BraceHighlight(int pos1, int pos2);

	/** Highlight the character at a position indicating there is no matching brace.
  */
	void BraceBadLight(int pos);

	/** Find the position of a matching brace or INVALID_POSITION if no match.
  */
	int BraceMatch(int pos);

	/** Are the end of line characters visible?
  */
	bool GetViewEOL();

	/** Make the end of line characters visible or invisible.
  */
	void SetViewEOL(bool visible);

	/** Retrieve a pointer to the document object.
  */
	int GetDocPointer();

	/** Change the document object used.
  */
	void SetDocPointer(int pointer);

	/** Set which document modification events are sent to the container.
  */
	void SetModEventMask(int mask);

	/** Retrieve the column number which text should be kept within.
  */
	int GetEdgeColumn();

	/** Set the column number of the edge.
	  * If text goes past the edge then it is highlighted.
  */
	void SetEdgeColumn(int column);

	/** Retrieve the edge highlight mode.
  */
	int GetEdgeMode();

	/** The edge may be displayed by a line (EDGE_LINE) or by highlighting text that
	  * goes beyond it (EDGE_BACKGROUND) or not displayed at all (EDGE_NONE).
  */
	void SetEdgeMode(int mode);

	/** Retrieve the colour used in edge indication.
  */
	boost::python::tuple GetEdgeColour();

	/** Change the colour used in edge indication.
  */
	void SetEdgeColour(boost::python::tuple edgeColour);

	/** Sets the current caret position to be the search anchor.
  */
	void SearchAnchor();

	/** Find some text starting at the search anchor.
	  * Does not ensure the selection is visible.
  */
	int SearchNext(int flags, boost::python::str text);

	/** Find some text starting at the search anchor and moving backwards.
	  * Does not ensure the selection is visible.
  */
	int SearchPrev(int flags, boost::python::str text);

	/** Retrieves the number of lines completely visible.
  */
	int LinesOnScreen();

	/** Set whether a pop up menu is displayed automatically when the user presses
	  * the wrong mouse button.
  */
	void UsePopUp(bool allowPopUp);

	/** Is the selection rectangular? The alternative is the more common stream selection.
  */
	bool SelectionIsRectangle();

	/** Set the zoom level. This number of points is added to the size of all fonts.
	  * It may be positive to magnify or negative to reduce.
  */
	void SetZoom(int zoom);

	/** Retrieve the zoom level.
  */
	int GetZoom();

	/** Create a new document object.
	  * Starts with reference count of 1 and not selected into editor.
  */
	int CreateDocument();

	/** Extend life of document.
  */
	void AddRefDocument(int doc);

	/** Release a reference to the document, deleting document if it fades to black.
  */
	void ReleaseDocument(int doc);

	/** Get which document modification events are sent to the container.
  */
	int GetModEventMask();

	/** Change internal focus flag.
  */
	void SetFocus(bool focus);

	/** Get internal focus flag.
  */
	bool GetFocus();

	/** Change error status - 0 = OK.
  */
	void SetStatus(int statusCode);

	/** Get error status.
  */
	int GetStatus();

	/** Set whether the mouse is captured when its button is pressed.
  */
	void SetMouseDownCaptures(bool captures);

	/** Get whether mouse gets captured.
  */
	bool GetMouseDownCaptures();

	/** Sets the cursor to one of the SC_CURSOR* values.
  */
	void SetCursor(int cursorType);

	/** Get cursor type.
  */
	int GetCursor();

	/** Change the way control characters are displayed:
	  * If symbol is < 32, keep the drawn way, else, use the given character.
  */
	void SetControlCharSymbol(int symbol);

	/** Get the way control characters are displayed.
  */
	int GetControlCharSymbol();

	/** Move to the previous change in capitalisation.
  */
	void WordPartLeft();

	/** Move to the previous change in capitalisation extending selection
	  * to new caret position.
  */
	void WordPartLeftExtend();

	/** Move to the change next in capitalisation.
  */
	void WordPartRight();

	/** Move to the next change in capitalisation extending selection
	  * to new caret position.
  */
	void WordPartRightExtend();

	/** Set the way the display area is determined when a particular line
	  * is to be moved to by Find, FindNext, GotoLine, etc.
  */
	void SetVisiblePolicy(int visiblePolicy, int visibleSlop);

	/** Delete back from the current position to the start of the line.
  */
	void DelLineLeft();

	/** Delete forwards from the current position to the end of the line.
  */
	void DelLineRight();

	/** Get and Set the xOffset (ie, horizonal scroll position).
  */
	void SetXOffset(int newOffset);

	/** Get and Set the xOffset (ie, horizonal scroll position).
  */
	int GetXOffset();

	/** Set the last x chosen value to be the caret x position.
  */
	void ChooseCaretX();

	/** Set the focus to this Scintilla widget.
  */
	void GrabFocus();

	/** Set the way the caret is kept visible when going sideway.
	  * The exclusion zone is given in pixels.
  */
	void SetXCaretPolicy(int caretPolicy, int caretSlop);

	/** Set the way the line the caret is on is kept visible.
	  * The exclusion zone is given in lines.
  */
	void SetYCaretPolicy(int caretPolicy, int caretSlop);

	/** Set printing to line wrapped (SC_WRAP_WORD) or not line wrapped (SC_WRAP_NONE).
  */
	void SetPrintWrapMode(int mode);

	/** Is printing line wrapped?
  */
	int GetPrintWrapMode();

	/** Set a fore colour for active hotspots.
  */
	void SetHotspotActiveFore(bool useSetting, boost::python::tuple fore);

	/** Get the fore colour for active hotspots.
  */
	boost::python::tuple GetHotspotActiveFore();

	/** Set a back colour for active hotspots.
  */
	void SetHotspotActiveBack(bool useSetting, boost::python::tuple back);

	/** Get the back colour for active hotspots.
  */
	boost::python::tuple GetHotspotActiveBack();

	/** Enable / Disable underlining active hotspots.
  */
	void SetHotspotActiveUnderline(bool underline);

	/** Get whether underlining for active hotspots.
  */
	bool GetHotspotActiveUnderline();

	/** Limit hotspots to single line so hotspots on two lines don't merge.
  */
	void SetHotspotSingleLine(bool singleLine);

	/** Get the HotspotSingleLine property
  */
	bool GetHotspotSingleLine();

	/** Move caret between paragraphs (delimited by empty lines).
  */
	void ParaDown();

	/** Move caret between paragraphs (delimited by empty lines).
  */
	void ParaDownExtend();

	/** Move caret between paragraphs (delimited by empty lines).
  */
	void ParaUp();

	/** Move caret between paragraphs (delimited by empty lines).
  */
	void ParaUpExtend();

	/** Given a valid document position, return the previous position taking code
	  * page into account. Returns 0 if passed 0.
  */
	int PositionBefore(int pos);

	/** Given a valid document position, return the next position taking code
	  * page into account. Maximum value returned is the last position in the document.
  */
	int PositionAfter(int pos);

	/** Copy a range of text to the clipboard. Positions are clipped into the document.
  */
	void CopyRange(int start, int end);

	/** Copy argument text to the clipboard.
  */
	int CopyText(boost::python::object text);

	/** Set the selection mode to stream (SC_SEL_STREAM) or rectangular (SC_SEL_RECTANGLE/SC_SEL_THIN) or
	  * by lines (SC_SEL_LINES).
  */
	void SetSelectionMode(int mode);

	/** Get the mode of the current selection.
  */
	int GetSelectionMode();

	/** Retrieve the position of the start of the selection at the given line (INVALID_POSITION if no selection on this line).
  */
	int GetLineSelStartPosition(int line);

	/** Retrieve the position of the end of the selection at the given line (INVALID_POSITION if no selection on this line).
  */
	int GetLineSelEndPosition(int line);

	/** Move caret down one line, extending rectangular selection to new caret position.
  */
	void LineDownRectExtend();

	/** Move caret up one line, extending rectangular selection to new caret position.
  */
	void LineUpRectExtend();

	/** Move caret left one character, extending rectangular selection to new caret position.
  */
	void CharLeftRectExtend();

	/** Move caret right one character, extending rectangular selection to new caret position.
  */
	void CharRightRectExtend();

	/** Move caret to first position on line, extending rectangular selection to new caret position.
  */
	void HomeRectExtend();

	/** Move caret to before first visible character on line.
	  * If already there move to first character on line.
	  * In either case, extend rectangular selection to new caret position.
  */
	void VCHomeRectExtend();

	/** Move caret to last position on line, extending rectangular selection to new caret position.
  */
	void LineEndRectExtend();

	/** Move caret one page up, extending rectangular selection to new caret position.
  */
	void PageUpRectExtend();

	/** Move caret one page down, extending rectangular selection to new caret position.
  */
	void PageDownRectExtend();

	/** Move caret to top of page, or one page up if already at top of page.
  */
	void StutteredPageUp();

	/** Move caret to top of page, or one page up if already at top of page, extending selection to new caret position.
  */
	void StutteredPageUpExtend();

	/** Move caret to bottom of page, or one page down if already at bottom of page.
  */
	void StutteredPageDown();

	/** Move caret to bottom of page, or one page down if already at bottom of page, extending selection to new caret position.
  */
	void StutteredPageDownExtend();

	/** Move caret left one word, position cursor at end of word.
  */
	void WordLeftEnd();

	/** Move caret left one word, position cursor at end of word, extending selection to new caret position.
  */
	void WordLeftEndExtend();

	/** Move caret right one word, position cursor at end of word.
  */
	void WordRightEnd();

	/** Move caret right one word, position cursor at end of word, extending selection to new caret position.
  */
	void WordRightEndExtend();

	/** Set the set of characters making up whitespace for when moving or selecting by word.
	  * Should be called after SetWordChars.
  */
	void SetWhitespaceChars(boost::python::str characters);

	/** Reset the set of characters for whitespace and word characters to the defaults.
  */
	void SetCharsDefault();

	/** Get currently selected item position in the auto-completion list
  */
	int AutoCGetCurrent();

	/** Get currently selected item text in the auto-completion list
	  * Returns the length of the item text
  */
	boost::python::str AutoCGetCurrentText();

	/** Enlarge the document to a particular size of text bytes.
  */
	void Allocate(int bytes);

	/** Returns the target converted to UTF8.
	  * Return the length in bytes.
  */
	boost::python::str TargetAsUTF8();

	/** Set the length of the utf8 argument for calling EncodedFromUTF8.
	  * Set to -1 and the string will be measured to the first nul.
  */
	void SetLengthForEncode(int bytes);

	/** Translates a UTF8 string into the document encoding.
	  * Return the length of the result in bytes.
	  * On error return 0.
  */
	boost::python::str EncodedFromUTF8();

	/** Find the position of a column on a line taking into account tabs and
	  * multi-byte characters. If beyond end of line, return line end position.
  */
	int FindColumn(int line, int column);

	/** Can the caret preferred x position only be changed by explicit movement commands?
  */
	int GetCaretSticky();

	/** Stop the caret preferred x position changing when the user types.
  */
	void SetCaretSticky(int useCaretStickyBehaviour);

	/** Switch between sticky and non-sticky: meant to be bound to a key.
  */
	void ToggleCaretSticky();

	/** Enable/Disable convert-on-paste for line endings
  */
	void SetPasteConvertEndings(bool convert);

	/** Get convert-on-paste setting
  */
	bool GetPasteConvertEndings();

	/** Duplicate the selection. If selection empty duplicate the line containing the caret.
  */
	void SelectionDuplicate();

	/** Set background alpha of the caret line.
  */
	void SetCaretLineBackAlpha(int alpha);

	/** Get the background alpha of the caret line.
  */
	int GetCaretLineBackAlpha();

	/** Set the style of the caret to be drawn.
  */
	void SetCaretStyle(int caretStyle);

	/** Returns the current style of the caret.
  */
	int GetCaretStyle();

	/** Set the indicator used for IndicatorFillRange and IndicatorClearRange
  */
	void SetIndicatorCurrent(int indicator);

	/** Get the current indicator
  */
	int GetIndicatorCurrent();

	/** Set the value used for IndicatorFillRange
  */
	void SetIndicatorValue(int value);

	/** Get the current indicator vaue
  */
	int GetIndicatorValue();

	/** Turn a indicator on over a range.
  */
	void IndicatorFillRange(int position, int fillLength);

	/** Turn a indicator off over a range.
  */
	void IndicatorClearRange(int position, int clearLength);

	/** Are any indicators present at position?
  */
	int IndicatorAllOnFor(int position);

	/** What value does a particular indicator have at at a position?
  */
	int IndicatorValueAt(int indicator, int position);

	/** Where does a particular indicator start?
  */
	int IndicatorStart(int indicator, int position);

	/** Where does a particular indicator end?
  */
	int IndicatorEnd(int indicator, int position);

	/** Set number of entries in position cache
  */
	void SetPositionCache(int size);

	/** How many entries are allocated to the position cache?
  */
	int GetPositionCache();

	/** Copy the selection, if selection empty copy the line with the caret
  */
	void CopyAllowLine();

	/** Always interpret keyboard input as Unicode
  */
	void SetKeysUnicode(bool keysUnicode);

	/** Are keys always interpreted as Unicode?
  */
	bool GetKeysUnicode();

	/** Set the alpha fill colour of the given indicator.
  */
	void IndicSetAlpha(int indicator, int alpha);

	/** Get the alpha fill colour of the given indicator.
  */
	int IndicGetAlpha(int indicator);

	/** Set extra ascent for each line
  */
	void SetExtraAscent(int extraAscent);

	/** Get extra ascent for each line
  */
	int GetExtraAscent();

	/** Set extra descent for each line
  */
	void SetExtraDescent(int extraDescent);

	/** Get extra descent for each line
  */
	int GetExtraDescent();

	/** Which symbol was defined for markerNumber with MarkerDefine
  */
	int MarkerSymbolDefined(int markerNumber);

	/** Set the text in the text margin for a line
  */
	void MarginSetText(int line, boost::python::str text);

	/** Get the text in the text margin for a line
  */
	boost::python::str MarginGetText(int line);

	/** Set the style number for the text margin for a line
  */
	void MarginSetStyle(int line, int style);

	/** Get the style number for the text margin for a line
  */
	int MarginGetStyle(int line);

	/** Set the style in the text margin for a line
  */
	void MarginSetStyles(int line, boost::python::str styles);

	/** Get the styles in the text margin for a line
  */
	boost::python::str MarginGetStyles(int line);

	/** Clear the margin text on all lines
  */
	void MarginTextClearAll();

	/** Get the start of the range of style numbers used for margin text
  */
	void MarginSetStyleOffset(int style);

	/** Get the start of the range of style numbers used for margin text
  */
	int MarginGetStyleOffset();

	/** Set the annotation text for a line
  */
	void AnnotationSetText(int line, boost::python::str text);

	/** Get the annotation text for a line
  */
	boost::python::str AnnotationGetText(int line);

	/** Set the style number for the annotations for a line
  */
	void AnnotationSetStyle(int line, int style);

	/** Get the style number for the annotations for a line
  */
	int AnnotationGetStyle(int line);

	/** Set the annotation styles for a line
  */
	void AnnotationSetStyles(int line, boost::python::str styles);

	/** Get the annotation styles for a line
  */
	boost::python::str AnnotationGetStyles(int line);

	/** Get the number of annotation lines for a line
  */
	int AnnotationGetLines(int line);

	/** Clear the annotations from all lines
  */
	void AnnotationClearAll();

	/** Set the visibility for the annotations for a view
  */
	void AnnotationSetVisible(int visible);

	/** Get the visibility for the annotations for a view
  */
	int AnnotationGetVisible();

	/** Get the start of the range of style numbers used for annotations
  */
	void AnnotationSetStyleOffset(int style);

	/** Get the start of the range of style numbers used for annotations
  */
	int AnnotationGetStyleOffset();

	/** Add a container action to the undo stack
  */
	void AddUndoAction(int token, int flags);

	/** Find the position of a character from a point within the window.
  */
	int CharPositionFromPoint(int x, int y);

	/** Find the position of a character from a point within the window.
	  * Return INVALID_POSITION if not close to text.
  */
	int CharPositionFromPointClose(int x, int y);

	/** Set whether multiple selections can be made
  */
	void SetMultipleSelection(bool multipleSelection);

	/** Whether multiple selections can be made
  */
	bool GetMultipleSelection();

	/** Set whether typing can be performed into multiple selections
  */
	void SetAdditionalSelectionTyping(bool additionalSelectionTyping);

	/** Whether typing can be performed into multiple selections
  */
	bool GetAdditionalSelectionTyping();

	/** Set whether additional carets will blink
  */
	void SetAdditionalCaretsBlink(bool additionalCaretsBlink);

	/** Whether additional carets will blink
  */
	bool GetAdditionalCaretsBlink();

	/** Set whether additional carets are visible
  */
	void SetAdditionalCaretsVisible(bool additionalCaretsBlink);

	/** Whether additional carets are visible
  */
	bool GetAdditionalCaretsVisible();

	/** How many selections are there?
  */
	int GetSelections();

	/** Clear selections to a single empty stream selection
  */
	void ClearSelections();

	/** Set a simple selection
  */
	int SetSelection(int caret, int anchor);

	/** Add a selection
  */
	int AddSelection(int caret, int anchor);

	/** Set the main selection
  */
	void SetMainSelection(int selection);

	/** Which selection is the main selection
  */
	int GetMainSelection();

	/** Which selection is the main selection
  */
	void SetSelectionNCaret(int selection, int pos);

	/** Which selection is the main selection
  */
	int GetSelectionNCaret(int selection);

	/** Which selection is the main selection
  */
	void SetSelectionNAnchor(int selection, int posAnchor);

	/** Which selection is the main selection
  */
	int GetSelectionNAnchor(int selection);

	/** Which selection is the main selection
  */
	void SetSelectionNCaretVirtualSpace(int selection, int space);

	/** Which selection is the main selection
  */
	int GetSelectionNCaretVirtualSpace(int selection);

	/** Which selection is the main selection
  */
	void SetSelectionNAnchorVirtualSpace(int selection, int space);

	/** Which selection is the main selection
  */
	int GetSelectionNAnchorVirtualSpace(int selection);

	/** Sets the position that starts the selection - this becomes the anchor.
  */
	void SetSelectionNStart(int selection, int pos);

	/** Returns the position at the start of the selection.
  */
	int GetSelectionNStart(int selection);

	/** Sets the position that ends the selection - this becomes the currentPosition.
  */
	void SetSelectionNEnd(int selection, int pos);

	/** Returns the position at the end of the selection.
  */
	int GetSelectionNEnd(int selection);

	/** Returns the position at the end of the selection.
  */
	void SetRectangularSelectionCaret(int pos);

	/** Returns the position at the end of the selection.
  */
	int GetRectangularSelectionCaret();

	/** Returns the position at the end of the selection.
  */
	void SetRectangularSelectionAnchor(int posAnchor);

	/** Returns the position at the end of the selection.
  */
	int GetRectangularSelectionAnchor();

	/** Returns the position at the end of the selection.
  */
	void SetRectangularSelectionCaretVirtualSpace(int space);

	/** Returns the position at the end of the selection.
  */
	int GetRectangularSelectionCaretVirtualSpace();

	/** Returns the position at the end of the selection.
  */
	void SetRectangularSelectionAnchorVirtualSpace(int space);

	/** Returns the position at the end of the selection.
  */
	int GetRectangularSelectionAnchorVirtualSpace();

	/** Returns the position at the end of the selection.
  */
	void SetVirtualSpaceOptions(int virtualSpaceOptions);

	/** Returns the position at the end of the selection.
  */
	int GetVirtualSpaceOptions();

	/** On GTK+, allow selecting the modifier key to use for mouse-based
	  * rectangular selection. Often the window manager requires Alt+Mouse Drag
	  * for moving windows.
	  * Valid values are SCMOD_CTRL(default), SCMOD_ALT, or SCMOD_SUPER.
  */
	void SetRectangularSelectionModifier(int modifier);

	/** Get the modifier key used for rectangular selection.
  */
	int GetRectangularSelectionModifier();

	/** Set the foreground colour of additional selections.
	  * Must have previously called SetSelFore with non-zero first argument for this to have an effect.
  */
	void SetAdditionalSelFore(boost::python::tuple fore);

	/** Set the background colour of additional selections.
	  * Must have previously called SetSelBack with non-zero first argument for this to have an effect.
  */
	void SetAdditionalSelBack(boost::python::tuple back);

	/** Set the alpha of the selection.
  */
	void SetAdditionalSelAlpha(int alpha);

	/** Get the alpha of the selection.
  */
	int GetAdditionalSelAlpha();

	/** Set the foreground colour of additional carets.
  */
	void SetAdditionalCaretFore(boost::python::tuple fore);

	/** Get the foreground colour of additional carets.
  */
	boost::python::tuple GetAdditionalCaretFore();

	/** Set the main selection to the next selection.
  */
	void RotateSelection();

	/** Swap that caret and anchor of the main selection.
  */
	void SwapMainAnchorCaret();

	/** Indicate that the internal state of a lexer has changed over a range and therefore
	  * there may be a need to redraw.
  */
	int ChangeLexerState(int start, int end);

	/** Find the next line at or after lineStart that is a contracted fold header line.
	  * Return -1 when no more lines.
  */
	int ContractedFoldNext(int lineStart);

	/** Centre current line in window.
  */
	void VerticalCentreCaret();

	/** Start notifying the container of all key presses and commands.
  */
	void StartRecord();

	/** Stop notifying the container of all key presses and commands.
  */
	void StopRecord();

	/** Set the lexing language of the document.
  */
	void SetLexer(int lexer);

	/** Retrieve the lexing language of the document.
  */
	int GetLexer();

	/** Colourise a segment of the document using the current lexing language.
  */
	void Colourise(int start, int end);

	/** Set up a value that may be used by a lexer for some optional feature.
  */
	void SetProperty(boost::python::str key, boost::python::str value);

	/** Set up the key words used by the lexer.
  */
	void SetKeyWords(int keywordSet, boost::python::str keyWords);

	/** Set the lexing language of the document based on string name.
  */
	void SetLexerLanguage(boost::python::str language);

	/** Load a lexer library (dll / so).
  */
	void LoadLexerLibrary(boost::python::str path);

	/** Retrieve a "property" value previously set with SetProperty.
  */
	boost::python::str GetProperty();

	/** Retrieve a "property" value previously set with SetProperty,
	  * with "$()" variable replacement on returned buffer.
  */
	boost::python::str GetPropertyExpanded();

	/** Retrieve a "property" value previously set with SetProperty,
	  * interpreted as an int AFTER any "$()" variable replacement.
  */
	int GetPropertyInt(boost::python::str key);

	/** Retrieve the number of bits the current lexer needs for styling.
  */
	int GetStyleBitsNeeded();

	/** Retrieve the name of the lexer.
	  * Return the length of the text.
  */
	boost::python::str GetLexerLanguage();

	/** For private communication between an application and a known lexer.
  */
	int PrivateLexerCall(int operation, int pointer);

	/** Retrieve a '\n' separated list of properties understood by the current lexer.
  */
	boost::python::str PropertyNames();

	/** Retrieve the type of a property.
  */
	int PropertyType(boost::python::str name);

	/** Describe a property.
  */
	boost::python::str DescribeProperty();

	/** Retrieve a '\n' separated list of descriptions of the keyword sets understood by the current lexer.
  */
	boost::python::str DescribeKeyWordSets();

/* --Autogenerated ---------------------------------------------------- */

	typedef std::multimap<idx_t, PyObject*> callbackT;

	// Call wrapper
	LRESULT callScintilla(UINT message, WPARAM wParam = 0, LPARAM lParam = 0)
	{
		return SendMessage(m_handle, message, wParam, lParam);
	};

protected:
	void consume(const std::shared_ptr<CallbackExecArgs>& args);

	
private:
	ScintillaWrapper(); // default constructor disabled

	// Active Scintilla handle
	HWND m_handle;

    // Notepad++ handle (used for replace)
    HWND m_hNotepad;
	
	// Callbacks
	
	callbackT m_callbacks;
	
	bool m_notificationsEnabled;
	
	static void runCallbacks(CallbackExecArgs *args);


    std::string extractEncodedString(boost::python::object str, int toCodePage);
    static NppPythonScript::ReplaceEntry *convertWithPython(const char *text, NppPythonScript::Match *match, void *state);
    boost::python::object m_pythonReplaceFunction;

    const char *getCurrentAnsiCodePageName();
};

#endif
