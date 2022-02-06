#include "stdafx.h"
#include "ScintillaWrapper.h"
#include "Scintilla.h"
#include "GILManager.h"

namespace NppPythonScript
{
// Helper class
class PythonCompatibleStrBuffer
{
public:
	inline explicit PythonCompatibleStrBuffer(size_t length) :
		m_bufferLen(length + 1),
		m_bufferPtr(new char[m_bufferLen])
	{
		if (m_bufferPtr && m_bufferLen > 0) m_bufferPtr[m_bufferLen-1] = '\0';
	}
	inline explicit PythonCompatibleStrBuffer(int length) :
		m_bufferLen(length >= 0 ? (size_t)(length+1) : 0),
		m_bufferPtr(new char[m_bufferLen])
	{
		if (m_bufferPtr && m_bufferLen > 0) m_bufferPtr[m_bufferLen-1] = '\0';
	}
	inline explicit PythonCompatibleStrBuffer(LRESULT length) :
		m_bufferLen(length >= 0 ? (size_t)(length+1) : 0),
		m_bufferPtr(new char[m_bufferLen])
	{
		if (m_bufferPtr && m_bufferLen > 0) m_bufferPtr[m_bufferLen-1] = '\0';
	}
	inline ~PythonCompatibleStrBuffer() { delete [] m_bufferPtr; }
	inline char* operator*() { return m_bufferPtr; }
	inline const char* c_str() const { return m_bufferPtr; }
	inline size_t size() const { return m_bufferLen; }
private:
	PythonCompatibleStrBuffer() = delete;  // default constructor disabled
	PythonCompatibleStrBuffer(const PythonCompatibleStrBuffer&) = delete; // copy constructor disabled
	PythonCompatibleStrBuffer& operator = (const PythonCompatibleStrBuffer&) = delete; // Disable assignment operator disabled
	size_t m_bufferLen;
	char* m_bufferPtr;
};

/** Add text to the document at current position.
  */
intptr_t ScintillaWrapper::AddText(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::AddText\n");
	std::string s = getStringFromObject(text);
	return callScintilla(SCI_ADDTEXT, s.size(), reinterpret_cast<LPARAM>(s.c_str()));
}

/** Add array of cells to document.
  */
intptr_t ScintillaWrapper::AddStyledText(ScintillaCells c)
{
	DEBUG_TRACE(L"ScintillaWrapper::AddStyledText\n");
	return callScintilla(SCI_ADDSTYLEDTEXT, c.length(), reinterpret_cast<LPARAM>(c.cells()));
}

/** Insert string at a position.
  */
void ScintillaWrapper::InsertText(intptr_t pos, boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::InsertText\n");
	std::string stringtext = getStringFromObject(text);
	callScintilla(SCI_INSERTTEXT, pos, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Change the text that is being inserted in response to SC_MOD_INSERTCHECK
  */
intptr_t ScintillaWrapper::ChangeInsertion(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::ChangeInsertion\n");
	std::string s = getStringFromObject(text);
	return callScintilla(SCI_CHANGEINSERTION, s.size(), reinterpret_cast<LPARAM>(s.c_str()));
}

/** Delete all text in the document.
  */
void ScintillaWrapper::ClearAll()
{
	DEBUG_TRACE(L"ScintillaWrapper::ClearAll\n");
	callScintilla(SCI_CLEARALL);
}

/** Delete a range of text in the document.
  */
void ScintillaWrapper::DeleteRange(intptr_t start, intptr_t lengthDelete)
{
	DEBUG_TRACE(L"ScintillaWrapper::DeleteRange\n");
	callScintilla(SCI_DELETERANGE, start, lengthDelete);
}

/** Set all style bytes to 0, remove all folding information.
  */
void ScintillaWrapper::ClearDocumentStyle()
{
	DEBUG_TRACE(L"ScintillaWrapper::ClearDocumentStyle\n");
	callScintilla(SCI_CLEARDOCUMENTSTYLE);
}

/** Returns the number of bytes in the document.
  */
intptr_t ScintillaWrapper::GetLength()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLength\n");
	return callScintilla(SCI_GETLENGTH);
}

/** Returns the character byte at the position.
  */
intptr_t ScintillaWrapper::GetCharAt(intptr_t pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCharAt\n");
	return callScintilla(SCI_GETCHARAT, pos);
}

/** Returns the position of the caret.
  */
intptr_t ScintillaWrapper::GetCurrentPos()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCurrentPos\n");
	return callScintilla(SCI_GETCURRENTPOS);
}

/** Returns the position of the opposite end of the selection to the caret.
  */
intptr_t ScintillaWrapper::GetAnchor()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetAnchor\n");
	return callScintilla(SCI_GETANCHOR);
}

/** Returns the style byte at the position.
  */
intptr_t ScintillaWrapper::GetStyleAt(intptr_t pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetStyleAt\n");
	return callScintilla(SCI_GETSTYLEAT, pos);
}

/** Redoes the next action on the undo history.
  */
void ScintillaWrapper::Redo()
{
	DEBUG_TRACE(L"ScintillaWrapper::Redo\n");
	callScintilla(SCI_REDO);
}

/** Choose between collecting actions into the undo
  * history and discarding them.
  */
void ScintillaWrapper::SetUndoCollection(bool collectUndo)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetUndoCollection\n");
	callScintilla(SCI_SETUNDOCOLLECTION, collectUndo);
}

/** Select all the text in the document.
  */
void ScintillaWrapper::SelectAll()
{
	DEBUG_TRACE(L"ScintillaWrapper::SelectAll\n");
	callScintilla(SCI_SELECTALL);
}

/** Remember the current position in the undo history as the position
  * at which the document was saved.
  */
void ScintillaWrapper::SetSavePoint()
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSavePoint\n");
	callScintilla(SCI_SETSAVEPOINT);
}

/** Retrieve a buffer of cells.
  * Returns the number of bytes in the buffer not including terminating NULs.
  */
boost::python::tuple ScintillaWrapper::GetStyledText(Sci_PositionCR start, Sci_PositionCR end)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetStyledText\n");
	Sci_TextRange src;
	if (end < start)
	{
		Sci_PositionCR temp = start;
		start = end;
		end = temp;
	}
	src.chrg.cpMin = start;
	src.chrg.cpMax = end;
	src.lpstrText = new char[size_t(((end-start) * 2) + 2)];
	callScintilla(SCI_GETSTYLEDTEXT, 0, reinterpret_cast<LPARAM>(&src));
	boost::python::list styles;
	PythonCompatibleStrBuffer result(end-start);
	for(idx_t pos = 0; pos < result.size() - 1; pos++)
	{
		(*result)[pos] = src.lpstrText[pos * 2];
		styles.append((int)(src.lpstrText[(pos * 2) + 1]));
	}
	boost::python::str resultStr(result.c_str());
	delete [] src.lpstrText;
	return boost::python::make_tuple(resultStr, styles);
}

/** Are there any redoable actions in the undo history?
  */
bool ScintillaWrapper::CanRedo()
{
	DEBUG_TRACE(L"ScintillaWrapper::CanRedo\n");
	return 0 != (callScintilla(SCI_CANREDO));
}

/** Retrieve the line number at which a particular marker is located.
  */
intptr_t ScintillaWrapper::MarkerLineFromHandle(int markerHandle)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerLineFromHandle\n");
	return callScintilla(SCI_MARKERLINEFROMHANDLE, markerHandle);
}

/** Delete a marker.
  */
void ScintillaWrapper::MarkerDeleteHandle(int markerHandle)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerDeleteHandle\n");
	callScintilla(SCI_MARKERDELETEHANDLE, markerHandle);
}

/** Retrieve marker handles of a line
  */
intptr_t ScintillaWrapper::MarkerHandleFromLine(intptr_t line, int which)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerHandleFromLine\n");
	return callScintilla(SCI_MARKERHANDLEFROMLINE, line, which);
}

/** Retrieve marker number of a marker handle
  */
intptr_t ScintillaWrapper::MarkerNumberFromLine(intptr_t line, int which)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerNumberFromLine\n");
	return callScintilla(SCI_MARKERNUMBERFROMLINE, line, which);
}

/** Is undo history being collected?
  */
bool ScintillaWrapper::GetUndoCollection()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetUndoCollection\n");
	return 0 != (callScintilla(SCI_GETUNDOCOLLECTION));
}

/** Are white space characters currently visible?
  * Returns one of SCWS_* constants.
  */
int ScintillaWrapper::GetViewWS()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetViewWS\n");
	return callScintilla(SCI_GETVIEWWS);
}

/** Make white space characters invisible, always visible or visible outside indentation.
  */
void ScintillaWrapper::SetViewWS(int viewWS)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetViewWS\n");
	callScintilla(SCI_SETVIEWWS, viewWS);
}

/** Retrieve the current tab draw mode.
  * Returns one of SCTD_* constants.
  */
int ScintillaWrapper::GetTabDrawMode()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTabDrawMode\n");
	return callScintilla(SCI_GETTABDRAWMODE);
}

/** Set how tabs are drawn when visible.
  */
void ScintillaWrapper::SetTabDrawMode(int tabDrawMode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetTabDrawMode\n");
	callScintilla(SCI_SETTABDRAWMODE, tabDrawMode);
}

/** Find the position from a point within the window.
  */
intptr_t ScintillaWrapper::PositionFromPoint(int x, int y)
{
	DEBUG_TRACE(L"ScintillaWrapper::PositionFromPoint\n");
	return callScintilla(SCI_POSITIONFROMPOINT, x, y);
}

/** Find the position from a point within the window but return
  * INVALID_POSITION if not close to text.
  */
intptr_t ScintillaWrapper::PositionFromPointClose(int x, int y)
{
	DEBUG_TRACE(L"ScintillaWrapper::PositionFromPointClose\n");
	return callScintilla(SCI_POSITIONFROMPOINTCLOSE, x, y);
}

/** Set caret to start of a line and ensure it is visible.
  */
void ScintillaWrapper::GotoLine(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GotoLine\n");
	callScintilla(SCI_GOTOLINE, line);
}

/** Set caret to a position and ensure it is visible.
  */
void ScintillaWrapper::GotoPos(intptr_t caret)
{
	DEBUG_TRACE(L"ScintillaWrapper::GotoPos\n");
	callScintilla(SCI_GOTOPOS, caret);
}

/** Set the selection anchor to a position. The anchor is the opposite
  * end of the selection from the caret.
  */
void ScintillaWrapper::SetAnchor(intptr_t anchor)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetAnchor\n");
	callScintilla(SCI_SETANCHOR, anchor);
}

/** Retrieve the text of the line containing the caret.
  * Returns the index of the caret on the line.
  * Result is NUL-terminated.
  */
boost::python::str ScintillaWrapper::GetCurLine()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCurLine\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETCURLINE) + 1);
	// result.size() does not depend on the order of evaluation here
	//lint -e{864}
	callScintilla(SCI_GETCURLINE, result.size(), reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Retrieve the position of the last correctly styled character.
  */
intptr_t ScintillaWrapper::GetEndStyled()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetEndStyled\n");
	return callScintilla(SCI_GETENDSTYLED);
}

/** Convert all line endings in the document to one mode.
  */
void ScintillaWrapper::ConvertEOLs(int eolMode)
{
	DEBUG_TRACE(L"ScintillaWrapper::ConvertEOLs\n");
	callScintilla(SCI_CONVERTEOLS, eolMode);
}

/** Retrieve the current end of line mode - one of CRLF, CR, or LF.
  */
int ScintillaWrapper::GetEOLMode()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetEOLMode\n");
	return callScintilla(SCI_GETEOLMODE);
}

/** Set the current end of line mode.
  */
void ScintillaWrapper::SetEOLMode(int eolMode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetEOLMode\n");
	callScintilla(SCI_SETEOLMODE, eolMode);
}

/** Set the current styling position to start.
  * The unused parameter is no longer used and should be set to 0.
  */
void ScintillaWrapper::StartStyling(intptr_t start, int unused)
{
	DEBUG_TRACE(L"ScintillaWrapper::StartStyling\n");
	callScintilla(SCI_STARTSTYLING, start, unused);
}

/** Change style from current styling position for length characters to a style
  * and move the current styling position to after this newly styled segment.
  */
void ScintillaWrapper::SetStyling(intptr_t length, int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetStyling\n");
	callScintilla(SCI_SETSTYLING, length, style);
}

/** Is drawing done first into a buffer or direct to the screen?
  */
bool ScintillaWrapper::GetBufferedDraw()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetBufferedDraw\n");
	return 0 != (callScintilla(SCI_GETBUFFEREDDRAW));
}

/** If drawing is buffered then each line of text is drawn into a bitmap buffer
  * before drawing it to the screen to avoid flicker.
  */
void ScintillaWrapper::SetBufferedDraw(bool buffered)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetBufferedDraw\n");
	callScintilla(SCI_SETBUFFEREDDRAW, buffered);
}

/** Change the visible size of a tab to be a multiple of the width of a space character.
  */
void ScintillaWrapper::SetTabWidth(int tabWidth)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetTabWidth\n");
	callScintilla(SCI_SETTABWIDTH, tabWidth);
}

/** Retrieve the visible size of a tab.
  */
intptr_t ScintillaWrapper::GetTabWidth()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTabWidth\n");
	return callScintilla(SCI_GETTABWIDTH);
}

/** Set the minimum visual width of a tab.
  */
void ScintillaWrapper::SetTabMinimumWidth(int pixels)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetTabMinimumWidth\n");
	callScintilla(SCI_SETTABMINIMUMWIDTH, pixels);
}

/** Get the minimum visual width of a tab.
  */
intptr_t ScintillaWrapper::GetTabMinimumWidth()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTabMinimumWidth\n");
	return callScintilla(SCI_GETTABMINIMUMWIDTH);
}

/** Clear explicit tabstops on a line.
  */
void ScintillaWrapper::ClearTabStops(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::ClearTabStops\n");
	callScintilla(SCI_CLEARTABSTOPS, line);
}

/** Add an explicit tab stop for a line.
  */
void ScintillaWrapper::AddTabStop(intptr_t line, int x)
{
	DEBUG_TRACE(L"ScintillaWrapper::AddTabStop\n");
	callScintilla(SCI_ADDTABSTOP, line, x);
}

/** Find the next explicit tab stop position on a line after a position.
  */
intptr_t ScintillaWrapper::GetNextTabStop(intptr_t line, int x)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetNextTabStop\n");
	return callScintilla(SCI_GETNEXTTABSTOP, line, x);
}

/** Set the code page used to interpret the bytes of the document as characters.
  * The SC_CP_UTF8 value can be used to enter Unicode mode.
  */
void ScintillaWrapper::SetCodePage(int codePage)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCodePage\n");
	callScintilla(SCI_SETCODEPAGE, codePage);
}

/** Is the IME displayed in a window or inline?
  */
int ScintillaWrapper::GetIMEInteraction()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetIMEInteraction\n");
	return callScintilla(SCI_GETIMEINTERACTION);
}

/** Choose to display the IME in a window or inline.
  */
void ScintillaWrapper::SetIMEInteraction(int imeInteraction)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetIMEInteraction\n");
	callScintilla(SCI_SETIMEINTERACTION, imeInteraction);
}

/** Set the symbol used for a particular marker number.
  */
void ScintillaWrapper::MarkerDefine(int markerNumber, int markerSymbol)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerDefine\n");
	callScintilla(SCI_MARKERDEFINE, markerNumber, markerSymbol);
}

/** Set the foreground colour used for a particular marker number.
  */
void ScintillaWrapper::MarkerSetFore(int markerNumber, boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerSetFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	callScintilla(SCI_MARKERSETFORE, markerNumber, static_cast<LPARAM>(rgbfore));
}

/** Set the background colour used for a particular marker number.
  */
void ScintillaWrapper::MarkerSetBack(int markerNumber, boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerSetBack\n");
	COLORREF rgbback = MAKECOLOUR(back);
	callScintilla(SCI_MARKERSETBACK, markerNumber, static_cast<LPARAM>(rgbback));
}

/** Set the background colour used for a particular marker number when its folding block is selected.
  */
void ScintillaWrapper::MarkerSetBackSelected(int markerNumber, boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerSetBackSelected\n");
	COLORREF rgbback = MAKECOLOUR(back);
	callScintilla(SCI_MARKERSETBACKSELECTED, markerNumber, static_cast<LPARAM>(rgbback));
}

/** Enable/disable highlight for current folding block (smallest one that contains the caret)
  */
void ScintillaWrapper::MarkerEnableHighlight(bool enabled)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerEnableHighlight\n");
	callScintilla(SCI_MARKERENABLEHIGHLIGHT, enabled);
}

/** Add a marker to a line, returning an ID which can be used to find or delete the marker.
  */
intptr_t ScintillaWrapper::MarkerAdd(intptr_t line, int markerNumber)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerAdd\n");
	return callScintilla(SCI_MARKERADD, line, markerNumber);
}

/** Delete a marker from a line.
  */
void ScintillaWrapper::MarkerDelete(intptr_t line, int markerNumber)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerDelete\n");
	callScintilla(SCI_MARKERDELETE, line, markerNumber);
}

/** Delete all markers with a particular number from all lines.
  */
void ScintillaWrapper::MarkerDeleteAll(int markerNumber)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerDeleteAll\n");
	callScintilla(SCI_MARKERDELETEALL, markerNumber);
}

/** Get a bit mask of all the markers set on a line.
  */
intptr_t ScintillaWrapper::MarkerGet(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerGet\n");
	return callScintilla(SCI_MARKERGET, line);
}

/** Find the next line at or after lineStart that includes a marker in mask.
  * Return -1 when no more lines.
  */
intptr_t ScintillaWrapper::MarkerNext(intptr_t lineStart, int markerMask)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerNext\n");
	return callScintilla(SCI_MARKERNEXT, lineStart, markerMask);
}

/** Find the previous line before lineStart that includes a marker in mask.
  */
intptr_t ScintillaWrapper::MarkerPrevious(intptr_t lineStart, int markerMask)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerPrevious\n");
	return callScintilla(SCI_MARKERPREVIOUS, lineStart, markerMask);
}

/** Define a marker from a pixmap.
  */
void ScintillaWrapper::MarkerDefinePixmap(int markerNumber, boost::python::object pixmap)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerDefinePixmap\n");
	std::string stringpixmap = getStringFromObject(pixmap);
	callScintilla(SCI_MARKERDEFINEPIXMAP, markerNumber, reinterpret_cast<LPARAM>(stringpixmap.c_str()));
}

/** Add a set of markers to a line.
  */
void ScintillaWrapper::MarkerAddSet(intptr_t line, int markerSet)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerAddSet\n");
	callScintilla(SCI_MARKERADDSET, line, markerSet);
}

/** Set the alpha used for a marker that is drawn in the text area, not the margin.
  */
void ScintillaWrapper::MarkerSetAlpha(int markerNumber, int alpha)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerSetAlpha\n");
	callScintilla(SCI_MARKERSETALPHA, markerNumber, alpha);
}

/** Set a margin to be either numeric or symbolic.
  */
void ScintillaWrapper::SetMarginTypeN(int margin, int marginType)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMarginTypeN\n");
	callScintilla(SCI_SETMARGINTYPEN, margin, marginType);
}

/** Retrieve the type of a margin.
  */
int ScintillaWrapper::GetMarginTypeN(int margin)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMarginTypeN\n");
	return callScintilla(SCI_GETMARGINTYPEN, margin);
}

/** Set the width of a margin to a width expressed in pixels.
  */
void ScintillaWrapper::SetMarginWidthN(int margin, int pixelWidth)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMarginWidthN\n");
	callScintilla(SCI_SETMARGINWIDTHN, margin, pixelWidth);
}

/** Retrieve the width of a margin in pixels.
  */
intptr_t ScintillaWrapper::GetMarginWidthN(int margin)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMarginWidthN\n");
	return callScintilla(SCI_GETMARGINWIDTHN, margin);
}

/** Set a mask that determines which markers are displayed in a margin.
  */
void ScintillaWrapper::SetMarginMaskN(int margin, int mask)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMarginMaskN\n");
	callScintilla(SCI_SETMARGINMASKN, margin, mask);
}

/** Retrieve the marker mask of a margin.
  */
intptr_t ScintillaWrapper::GetMarginMaskN(int margin)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMarginMaskN\n");
	return callScintilla(SCI_GETMARGINMASKN, margin);
}

/** Make a margin sensitive or insensitive to mouse clicks.
  */
void ScintillaWrapper::SetMarginSensitiveN(int margin, bool sensitive)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMarginSensitiveN\n");
	callScintilla(SCI_SETMARGINSENSITIVEN, margin, sensitive);
}

/** Retrieve the mouse click sensitivity of a margin.
  */
bool ScintillaWrapper::GetMarginSensitiveN(int margin)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMarginSensitiveN\n");
	return 0 != (callScintilla(SCI_GETMARGINSENSITIVEN, margin));
}

/** Set the cursor shown when the mouse is inside a margin.
  */
void ScintillaWrapper::SetMarginCursorN(int margin, int cursor)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMarginCursorN\n");
	callScintilla(SCI_SETMARGINCURSORN, margin, cursor);
}

/** Retrieve the cursor shown in a margin.
  */
int ScintillaWrapper::GetMarginCursorN(int margin)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMarginCursorN\n");
	return callScintilla(SCI_GETMARGINCURSORN, margin);
}

/** Set the background colour of a margin. Only visible for SC_MARGIN_COLOUR.
  */
void ScintillaWrapper::SetMarginBackN(int margin, boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMarginBackN\n");
	COLORREF rgbback = MAKECOLOUR(back);
	callScintilla(SCI_SETMARGINBACKN, margin, static_cast<LPARAM>(rgbback));
}

/** Retrieve the background colour of a margin
  */
boost::python::tuple ScintillaWrapper::GetMarginBackN(int margin)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMarginBackN\n");
	int retVal = (int)callScintilla(SCI_GETMARGINBACKN, margin);
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** Allocate a non-standard number of margins.
  */
void ScintillaWrapper::SetMargins(int margins)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMargins\n");
	callScintilla(SCI_SETMARGINS, margins);
}

/** How many margins are there?.
  */
intptr_t ScintillaWrapper::GetMargins()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMargins\n");
	return callScintilla(SCI_GETMARGINS);
}

/** Clear all the styles and make equivalent to the global default style.
  */
void ScintillaWrapper::StyleClearAll()
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleClearAll\n");
	callScintilla(SCI_STYLECLEARALL);
}

/** Set the foreground colour of a style.
  */
void ScintillaWrapper::StyleSetFore(int style, boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	callScintilla(SCI_STYLESETFORE, style, static_cast<LPARAM>(rgbfore));
}

/** Set the background colour of a style.
  */
void ScintillaWrapper::StyleSetBack(int style, boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetBack\n");
	COLORREF rgbback = MAKECOLOUR(back);
	callScintilla(SCI_STYLESETBACK, style, static_cast<LPARAM>(rgbback));
}

/** Set a style to be bold or not.
  */
void ScintillaWrapper::StyleSetBold(int style, bool bold)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetBold\n");
	callScintilla(SCI_STYLESETBOLD, style, bold);
}

/** Set a style to be italic or not.
  */
void ScintillaWrapper::StyleSetItalic(int style, bool italic)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetItalic\n");
	callScintilla(SCI_STYLESETITALIC, style, italic);
}

/** Set the size of characters of a style.
  */
void ScintillaWrapper::StyleSetSize(int style, int sizePoints)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetSize\n");
	callScintilla(SCI_STYLESETSIZE, style, sizePoints);
}

/** Set the font of a style.
  */
void ScintillaWrapper::StyleSetFont(int style, boost::python::object fontName)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetFont\n");
	std::string stringfontName = getStringFromObject(fontName);
	callScintilla(SCI_STYLESETFONT, style, reinterpret_cast<LPARAM>(stringfontName.c_str()));
}

/** Set a style to have its end of line filled or not.
  */
void ScintillaWrapper::StyleSetEOLFilled(int style, bool eolFilled)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetEOLFilled\n");
	callScintilla(SCI_STYLESETEOLFILLED, style, eolFilled);
}

/** Reset the default style to its state at startup
  */
void ScintillaWrapper::StyleResetDefault()
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleResetDefault\n");
	callScintilla(SCI_STYLERESETDEFAULT);
}

/** Set a style to be underlined or not.
  */
void ScintillaWrapper::StyleSetUnderline(int style, bool underline)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetUnderline\n");
	callScintilla(SCI_STYLESETUNDERLINE, style, underline);
}

/** Get the foreground colour of a style.
  */
boost::python::tuple ScintillaWrapper::StyleGetFore(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetFore\n");
	int retVal = (int)callScintilla(SCI_STYLEGETFORE, style);
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** Get the background colour of a style.
  */
boost::python::tuple ScintillaWrapper::StyleGetBack(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetBack\n");
	int retVal = (int)callScintilla(SCI_STYLEGETBACK, style);
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** Get is a style bold or not.
  */
bool ScintillaWrapper::StyleGetBold(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetBold\n");
	return 0 != (callScintilla(SCI_STYLEGETBOLD, style));
}

/** Get is a style italic or not.
  */
bool ScintillaWrapper::StyleGetItalic(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetItalic\n");
	return 0 != (callScintilla(SCI_STYLEGETITALIC, style));
}

/** Get the size of characters of a style.
  */
intptr_t ScintillaWrapper::StyleGetSize(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetSize\n");
	return callScintilla(SCI_STYLEGETSIZE, style);
}

/** Get the font of a style.
  * Returns the length of the fontName
  * Result is NUL-terminated.
  */
boost::python::str ScintillaWrapper::StyleGetFont(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetFont\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_STYLEGETFONT, style));
	callScintilla(SCI_STYLEGETFONT, style, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Get is a style to have its end of line filled or not.
  */
bool ScintillaWrapper::StyleGetEOLFilled(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetEOLFilled\n");
	return 0 != (callScintilla(SCI_STYLEGETEOLFILLED, style));
}

/** Get is a style underlined or not.
  */
bool ScintillaWrapper::StyleGetUnderline(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetUnderline\n");
	return 0 != (callScintilla(SCI_STYLEGETUNDERLINE, style));
}

/** Get is a style mixed case, or to force upper or lower case.
  */
int ScintillaWrapper::StyleGetCase(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetCase\n");
	return callScintilla(SCI_STYLEGETCASE, style);
}

/** Get the character get of the font in a style.
  */
int ScintillaWrapper::StyleGetCharacterSet(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetCharacterSet\n");
	return callScintilla(SCI_STYLEGETCHARACTERSET, style);
}

/** Get is a style visible or not.
  */
bool ScintillaWrapper::StyleGetVisible(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetVisible\n");
	return 0 != (callScintilla(SCI_STYLEGETVISIBLE, style));
}

/** Get is a style changeable or not (read only).
  * Experimental feature, currently buggy.
  */
bool ScintillaWrapper::StyleGetChangeable(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetChangeable\n");
	return 0 != (callScintilla(SCI_STYLEGETCHANGEABLE, style));
}

/** Get is a style a hotspot or not.
  */
bool ScintillaWrapper::StyleGetHotSpot(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetHotSpot\n");
	return 0 != (callScintilla(SCI_STYLEGETHOTSPOT, style));
}

/** Set a style to be mixed case, or to force upper or lower case.
  */
void ScintillaWrapper::StyleSetCase(int style, int caseVisible)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetCase\n");
	callScintilla(SCI_STYLESETCASE, style, caseVisible);
}

/** Set the size of characters of a style. Size is in points multiplied by 100.
  */
void ScintillaWrapper::StyleSetSizeFractional(int style, int sizeHundredthPoints)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetSizeFractional\n");
	callScintilla(SCI_STYLESETSIZEFRACTIONAL, style, sizeHundredthPoints);
}

/** Get the size of characters of a style in points multiplied by 100
  */
intptr_t ScintillaWrapper::StyleGetSizeFractional(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetSizeFractional\n");
	return callScintilla(SCI_STYLEGETSIZEFRACTIONAL, style);
}

/** Set the weight of characters of a style.
  */
void ScintillaWrapper::StyleSetWeight(int style, int weight)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetWeight\n");
	callScintilla(SCI_STYLESETWEIGHT, style, weight);
}

/** Get the weight of characters of a style.
  */
int ScintillaWrapper::StyleGetWeight(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetWeight\n");
	return callScintilla(SCI_STYLEGETWEIGHT, style);
}

/** Set the character set of the font in a style.
  */
void ScintillaWrapper::StyleSetCharacterSet(int style, int characterSet)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetCharacterSet\n");
	callScintilla(SCI_STYLESETCHARACTERSET, style, characterSet);
}

/** Set a style to be a hotspot or not.
  */
void ScintillaWrapper::StyleSetHotSpot(int style, bool hotspot)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetHotSpot\n");
	callScintilla(SCI_STYLESETHOTSPOT, style, hotspot);
}

/** Set the foreground colour of the main and additional selections and whether to use this setting.
  */
void ScintillaWrapper::SetSelFore(bool useSetting, boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	callScintilla(SCI_SETSELFORE, useSetting, static_cast<LPARAM>(rgbfore));
}

/** Set the background colour of the main and additional selections and whether to use this setting.
  */
void ScintillaWrapper::SetSelBack(bool useSetting, boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelBack\n");
	COLORREF rgbback = MAKECOLOUR(back);
	callScintilla(SCI_SETSELBACK, useSetting, static_cast<LPARAM>(rgbback));
}

/** Get the alpha of the selection.
  */
int ScintillaWrapper::GetSelAlpha()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelAlpha\n");
	return callScintilla(SCI_GETSELALPHA);
}

/** Set the alpha of the selection.
  */
void ScintillaWrapper::SetSelAlpha(int alpha)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelAlpha\n");
	callScintilla(SCI_SETSELALPHA, alpha);
}

/** Is the selection end of line filled?
  */
bool ScintillaWrapper::GetSelEOLFilled()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelEOLFilled\n");
	return 0 != (callScintilla(SCI_GETSELEOLFILLED));
}

/** Set the selection to have its end of line filled or not.
  */
void ScintillaWrapper::SetSelEOLFilled(bool filled)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelEOLFilled\n");
	callScintilla(SCI_SETSELEOLFILLED, filled);
}

/** Set the foreground colour of the caret.
  */
void ScintillaWrapper::SetCaretFore(boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCaretFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	callScintilla(SCI_SETCARETFORE, static_cast<WPARAM>(rgbfore));
}

/** When key+modifier combination keyDefinition is pressed perform sciCommand.
  */
void ScintillaWrapper::AssignCmdKey(int keyDefinition, int sciCommand)
{
	DEBUG_TRACE(L"ScintillaWrapper::AssignCmdKey\n");
	callScintilla(SCI_ASSIGNCMDKEY, keyDefinition, sciCommand);
}

/** When key+modifier combination keyDefinition is pressed do nothing.
  */
void ScintillaWrapper::ClearCmdKey(int keyDefinition)
{
	DEBUG_TRACE(L"ScintillaWrapper::ClearCmdKey\n");
	callScintilla(SCI_CLEARCMDKEY, keyDefinition);
}

/** Drop all key mappings.
  */
void ScintillaWrapper::ClearAllCmdKeys()
{
	DEBUG_TRACE(L"ScintillaWrapper::ClearAllCmdKeys\n");
	callScintilla(SCI_CLEARALLCMDKEYS);
}

/** Set the styles for a segment of the document.
  */
intptr_t ScintillaWrapper::SetStylingEx(boost::python::object styles)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetStylingEx\n");
	std::string s = getStringFromObject(styles);
	return callScintilla(SCI_SETSTYLINGEX, s.size(), reinterpret_cast<LPARAM>(s.c_str()));
}

/** Set a style to be visible or not.
  */
void ScintillaWrapper::StyleSetVisible(int style, bool visible)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetVisible\n");
	callScintilla(SCI_STYLESETVISIBLE, style, visible);
}

/** Get the time in milliseconds that the caret is on and off.
  */
intptr_t ScintillaWrapper::GetCaretPeriod()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCaretPeriod\n");
	return callScintilla(SCI_GETCARETPERIOD);
}

/** Get the time in milliseconds that the caret is on and off. 0 = steady on.
  */
void ScintillaWrapper::SetCaretPeriod(int periodMilliseconds)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCaretPeriod\n");
	callScintilla(SCI_SETCARETPERIOD, periodMilliseconds);
}

/** Set the set of characters making up words for when moving or selecting by word.
  * First sets defaults like SetCharsDefault.
  */
void ScintillaWrapper::SetWordChars(boost::python::object characters)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWordChars\n");
	std::string stringcharacters = getStringFromObject(characters);
	callScintilla(SCI_SETWORDCHARS, 0, reinterpret_cast<LPARAM>(stringcharacters.c_str()));
}

/** Get the set of characters making up words for when moving or selecting by word.
  * Returns the number of characters
  */
boost::python::str ScintillaWrapper::GetWordChars()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetWordChars\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETWORDCHARS));
	callScintilla(SCI_GETWORDCHARS, 0, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Set the number of characters to have directly indexed categories
  */
void ScintillaWrapper::SetCharacterCategoryOptimization(int countCharacters)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCharacterCategoryOptimization\n");
	callScintilla(SCI_SETCHARACTERCATEGORYOPTIMIZATION, countCharacters);
}

/** Get the number of characters to have directly indexed categories
  */
intptr_t ScintillaWrapper::GetCharacterCategoryOptimization()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCharacterCategoryOptimization\n");
	return callScintilla(SCI_GETCHARACTERCATEGORYOPTIMIZATION);
}

/** Start a sequence of actions that is undone and redone as a unit.
  * May be nested.
  */
void ScintillaWrapper::BeginUndoAction()
{
	DEBUG_TRACE(L"ScintillaWrapper::BeginUndoAction\n");
	callScintilla(SCI_BEGINUNDOACTION);
}

/** End a sequence of actions that is undone and redone as a unit.
  */
void ScintillaWrapper::EndUndoAction()
{
	DEBUG_TRACE(L"ScintillaWrapper::EndUndoAction\n");
	callScintilla(SCI_ENDUNDOACTION);
}

/** Set an indicator to plain, squiggle or TT.
  */
void ScintillaWrapper::IndicSetStyle(int indicator, int indicatorStyle)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicSetStyle\n");
	callScintilla(SCI_INDICSETSTYLE, indicator, indicatorStyle);
}

/** Retrieve the style of an indicator.
  */
int ScintillaWrapper::IndicGetStyle(int indicator)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicGetStyle\n");
	return callScintilla(SCI_INDICGETSTYLE, indicator);
}

/** Set the foreground colour of an indicator.
  */
void ScintillaWrapper::IndicSetFore(int indicator, boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicSetFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	callScintilla(SCI_INDICSETFORE, indicator, static_cast<LPARAM>(rgbfore));
}

/** Retrieve the foreground colour of an indicator.
  */
boost::python::tuple ScintillaWrapper::IndicGetFore(int indicator)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicGetFore\n");
	int retVal = (int)callScintilla(SCI_INDICGETFORE, indicator);
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** Set an indicator to draw under text or over(default).
  */
void ScintillaWrapper::IndicSetUnder(int indicator, bool under)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicSetUnder\n");
	callScintilla(SCI_INDICSETUNDER, indicator, under);
}

/** Retrieve whether indicator drawn under or over text.
  */
bool ScintillaWrapper::IndicGetUnder(int indicator)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicGetUnder\n");
	return 0 != (callScintilla(SCI_INDICGETUNDER, indicator));
}

/** Set a hover indicator to plain, squiggle or TT.
  */
void ScintillaWrapper::IndicSetHoverStyle(int indicator, int indicatorStyle)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicSetHoverStyle\n");
	callScintilla(SCI_INDICSETHOVERSTYLE, indicator, indicatorStyle);
}

/** Retrieve the hover style of an indicator.
  */
int ScintillaWrapper::IndicGetHoverStyle(int indicator)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicGetHoverStyle\n");
	return callScintilla(SCI_INDICGETHOVERSTYLE, indicator);
}

/** Set the foreground hover colour of an indicator.
  */
void ScintillaWrapper::IndicSetHoverFore(int indicator, boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicSetHoverFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	callScintilla(SCI_INDICSETHOVERFORE, indicator, static_cast<LPARAM>(rgbfore));
}

/** Retrieve the foreground hover colour of an indicator.
  */
boost::python::tuple ScintillaWrapper::IndicGetHoverFore(int indicator)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicGetHoverFore\n");
	int retVal = (int)callScintilla(SCI_INDICGETHOVERFORE, indicator);
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** Set the attributes of an indicator.
  */
void ScintillaWrapper::IndicSetFlags(int indicator, int flags)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicSetFlags\n");
	callScintilla(SCI_INDICSETFLAGS, indicator, flags);
}

/** Retrieve the attributes of an indicator.
  */
int ScintillaWrapper::IndicGetFlags(int indicator)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicGetFlags\n");
	return callScintilla(SCI_INDICGETFLAGS, indicator);
}

/** Set the foreground colour of all whitespace and whether to use this setting.
  */
void ScintillaWrapper::SetWhitespaceFore(bool useSetting, boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWhitespaceFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	callScintilla(SCI_SETWHITESPACEFORE, useSetting, static_cast<LPARAM>(rgbfore));
}

/** Set the background colour of all whitespace and whether to use this setting.
  */
void ScintillaWrapper::SetWhitespaceBack(bool useSetting, boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWhitespaceBack\n");
	COLORREF rgbback = MAKECOLOUR(back);
	callScintilla(SCI_SETWHITESPACEBACK, useSetting, static_cast<LPARAM>(rgbback));
}

/** Set the size of the dots used to mark space characters.
  */
void ScintillaWrapper::SetWhitespaceSize(int size)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWhitespaceSize\n");
	callScintilla(SCI_SETWHITESPACESIZE, size);
}

/** Get the size of the dots used to mark space characters.
  */
intptr_t ScintillaWrapper::GetWhitespaceSize()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetWhitespaceSize\n");
	return callScintilla(SCI_GETWHITESPACESIZE);
}

/** Used to hold extra styling information for each line.
  */
void ScintillaWrapper::SetLineState(intptr_t line, int state)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetLineState\n");
	callScintilla(SCI_SETLINESTATE, line, state);
}

/** Retrieve the extra styling information for a line.
  */
intptr_t ScintillaWrapper::GetLineState(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineState\n");
	return callScintilla(SCI_GETLINESTATE, line);
}

/** Retrieve the last line number that has line state.
  */
intptr_t ScintillaWrapper::GetMaxLineState()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMaxLineState\n");
	return callScintilla(SCI_GETMAXLINESTATE);
}

/** Is the background of the line containing the caret in a different colour?
  */
bool ScintillaWrapper::GetCaretLineVisible()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCaretLineVisible\n");
	return 0 != (callScintilla(SCI_GETCARETLINEVISIBLE));
}

/** Display the background of the line containing the caret in a different colour.
  */
void ScintillaWrapper::SetCaretLineVisible(bool show)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCaretLineVisible\n");
	callScintilla(SCI_SETCARETLINEVISIBLE, show);
}

/** Get the colour of the background of the line containing the caret.
  */
boost::python::tuple ScintillaWrapper::GetCaretLineBack()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCaretLineBack\n");
	int retVal = (int)callScintilla(SCI_GETCARETLINEBACK);
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** Set the colour of the background of the line containing the caret.
  */
void ScintillaWrapper::SetCaretLineBack(boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCaretLineBack\n");
	COLORREF rgbback = MAKECOLOUR(back);
	callScintilla(SCI_SETCARETLINEBACK, static_cast<WPARAM>(rgbback));
}

/** Retrieve the caret line frame width.
  * Width = 0 means this option is disabled.
  */
intptr_t ScintillaWrapper::GetCaretLineFrame()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCaretLineFrame\n");
	return callScintilla(SCI_GETCARETLINEFRAME);
}

/** Display the caret line framed.
  * Set width != 0 to enable this option and width = 0 to disable it.
  */
void ScintillaWrapper::SetCaretLineFrame(int width)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCaretLineFrame\n");
	callScintilla(SCI_SETCARETLINEFRAME, width);
}

/** Set a style to be changeable or not (read only).
  * Experimental feature, currently buggy.
  */
void ScintillaWrapper::StyleSetChangeable(int style, bool changeable)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetChangeable\n");
	callScintilla(SCI_STYLESETCHANGEABLE, style, changeable);
}

/** Display a auto-completion list.
  * The lengthEntered parameter indicates how many characters before
  * the caret should be used to provide context.
  */
void ScintillaWrapper::AutoCShow(intptr_t lengthEntered, boost::python::object itemList)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCShow\n");
	std::string stringitemList = getStringFromObject(itemList);
	callScintilla(SCI_AUTOCSHOW, lengthEntered, reinterpret_cast<LPARAM>(stringitemList.c_str()));
}

/** Remove the auto-completion list from the screen.
  */
void ScintillaWrapper::AutoCCancel()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCCancel\n");
	callScintilla(SCI_AUTOCCANCEL);
}

/** Is there an auto-completion list visible?
  */
bool ScintillaWrapper::AutoCActive()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCActive\n");
	return 0 != (callScintilla(SCI_AUTOCACTIVE));
}

/** Retrieve the position of the caret when the auto-completion list was displayed.
  */
intptr_t ScintillaWrapper::AutoCPosStart()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCPosStart\n");
	return callScintilla(SCI_AUTOCPOSSTART);
}

/** User has selected an item so remove the list and insert the selection.
  */
void ScintillaWrapper::AutoCComplete()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCComplete\n");
	callScintilla(SCI_AUTOCCOMPLETE);
}

/** Define a set of character that when typed cancel the auto-completion list.
  */
void ScintillaWrapper::AutoCStops(boost::python::object characterSet)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCStops\n");
	std::string stringcharacterSet = getStringFromObject(characterSet);
	callScintilla(SCI_AUTOCSTOPS, 0, reinterpret_cast<LPARAM>(stringcharacterSet.c_str()));
}

/** Change the separator character in the string setting up an auto-completion list.
  * Default is space but can be changed if items contain space.
  */
void ScintillaWrapper::AutoCSetSeparator(int separatorCharacter)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetSeparator\n");
	callScintilla(SCI_AUTOCSETSEPARATOR, separatorCharacter);
}

/** Retrieve the auto-completion list separator character.
  */
intptr_t ScintillaWrapper::AutoCGetSeparator()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetSeparator\n");
	return callScintilla(SCI_AUTOCGETSEPARATOR);
}

/** Select the item in the auto-completion list that starts with a string.
  */
void ScintillaWrapper::AutoCSelect(boost::python::object select)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSelect\n");
	std::string stringselect = getStringFromObject(select);
	callScintilla(SCI_AUTOCSELECT, 0, reinterpret_cast<LPARAM>(stringselect.c_str()));
}

/** Should the auto-completion list be cancelled if the user backspaces to a
  * position before where the box was created.
  */
void ScintillaWrapper::AutoCSetCancelAtStart(bool cancel)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetCancelAtStart\n");
	callScintilla(SCI_AUTOCSETCANCELATSTART, cancel);
}

/** Retrieve whether auto-completion cancelled by backspacing before start.
  */
bool ScintillaWrapper::AutoCGetCancelAtStart()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetCancelAtStart\n");
	return 0 != (callScintilla(SCI_AUTOCGETCANCELATSTART));
}

/** Define a set of characters that when typed will cause the autocompletion to
  * choose the selected item.
  */
void ScintillaWrapper::AutoCSetFillUps(boost::python::object characterSet)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetFillUps\n");
	std::string stringcharacterSet = getStringFromObject(characterSet);
	callScintilla(SCI_AUTOCSETFILLUPS, 0, reinterpret_cast<LPARAM>(stringcharacterSet.c_str()));
}

/** Should a single item auto-completion list automatically choose the item.
  */
void ScintillaWrapper::AutoCSetChooseSingle(bool chooseSingle)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetChooseSingle\n");
	callScintilla(SCI_AUTOCSETCHOOSESINGLE, chooseSingle);
}

/** Retrieve whether a single item auto-completion list automatically choose the item.
  */
bool ScintillaWrapper::AutoCGetChooseSingle()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetChooseSingle\n");
	return 0 != (callScintilla(SCI_AUTOCGETCHOOSESINGLE));
}

/** Set whether case is significant when performing auto-completion searches.
  */
void ScintillaWrapper::AutoCSetIgnoreCase(bool ignoreCase)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetIgnoreCase\n");
	callScintilla(SCI_AUTOCSETIGNORECASE, ignoreCase);
}

/** Retrieve state of ignore case flag.
  */
bool ScintillaWrapper::AutoCGetIgnoreCase()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetIgnoreCase\n");
	return 0 != (callScintilla(SCI_AUTOCGETIGNORECASE));
}

/** Display a list of strings and send notification when user chooses one.
  */
void ScintillaWrapper::UserListShow(int listType, boost::python::object itemList)
{
	DEBUG_TRACE(L"ScintillaWrapper::UserListShow\n");
	std::string stringitemList = getStringFromObject(itemList);
	callScintilla(SCI_USERLISTSHOW, listType, reinterpret_cast<LPARAM>(stringitemList.c_str()));
}

/** Set whether or not autocompletion is hidden automatically when nothing matches.
  */
void ScintillaWrapper::AutoCSetAutoHide(bool autoHide)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetAutoHide\n");
	callScintilla(SCI_AUTOCSETAUTOHIDE, autoHide);
}

/** Retrieve whether or not autocompletion is hidden automatically when nothing matches.
  */
bool ScintillaWrapper::AutoCGetAutoHide()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetAutoHide\n");
	return 0 != (callScintilla(SCI_AUTOCGETAUTOHIDE));
}

/** Set whether or not autocompletion deletes any word characters
  * after the inserted text upon completion.
  */
void ScintillaWrapper::AutoCSetDropRestOfWord(bool dropRestOfWord)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetDropRestOfWord\n");
	callScintilla(SCI_AUTOCSETDROPRESTOFWORD, dropRestOfWord);
}

/** Retrieve whether or not autocompletion deletes any word characters
  * after the inserted text upon completion.
  */
bool ScintillaWrapper::AutoCGetDropRestOfWord()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetDropRestOfWord\n");
	return 0 != (callScintilla(SCI_AUTOCGETDROPRESTOFWORD));
}

/** Register an XPM image for use in autocompletion lists.
  */
void ScintillaWrapper::RegisterImage(int type, boost::python::object xpmData)
{
	DEBUG_TRACE(L"ScintillaWrapper::RegisterImage\n");
	std::string stringxpmData = getStringFromObject(xpmData);
	callScintilla(SCI_REGISTERIMAGE, type, reinterpret_cast<LPARAM>(stringxpmData.c_str()));
}

/** Clear all the registered XPM images.
  */
void ScintillaWrapper::ClearRegisteredImages()
{
	DEBUG_TRACE(L"ScintillaWrapper::ClearRegisteredImages\n");
	callScintilla(SCI_CLEARREGISTEREDIMAGES);
}

/** Retrieve the auto-completion list type-separator character.
  */
intptr_t ScintillaWrapper::AutoCGetTypeSeparator()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetTypeSeparator\n");
	return callScintilla(SCI_AUTOCGETTYPESEPARATOR);
}

/** Change the type-separator character in the string setting up an auto-completion list.
  * Default is '?' but can be changed if items contain '?'.
  */
void ScintillaWrapper::AutoCSetTypeSeparator(int separatorCharacter)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetTypeSeparator\n");
	callScintilla(SCI_AUTOCSETTYPESEPARATOR, separatorCharacter);
}

/** Set the maximum width, in characters, of auto-completion and user lists.
  * Set to 0 to autosize to fit longest item, which is the default.
  */
void ScintillaWrapper::AutoCSetMaxWidth(int characterCount)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetMaxWidth\n");
	callScintilla(SCI_AUTOCSETMAXWIDTH, characterCount);
}

/** Get the maximum width, in characters, of auto-completion and user lists.
  */
intptr_t ScintillaWrapper::AutoCGetMaxWidth()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetMaxWidth\n");
	return callScintilla(SCI_AUTOCGETMAXWIDTH);
}

/** Set the maximum height, in rows, of auto-completion and user lists.
  * The default is 5 rows.
  */
void ScintillaWrapper::AutoCSetMaxHeight(int rowCount)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetMaxHeight\n");
	callScintilla(SCI_AUTOCSETMAXHEIGHT, rowCount);
}

/** Set the maximum height, in rows, of auto-completion and user lists.
  */
intptr_t ScintillaWrapper::AutoCGetMaxHeight()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetMaxHeight\n");
	return callScintilla(SCI_AUTOCGETMAXHEIGHT);
}

/** Set the number of spaces used for one level of indentation.
  */
void ScintillaWrapper::SetIndent(int indentSize)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetIndent\n");
	callScintilla(SCI_SETINDENT, indentSize);
}

/** Retrieve indentation size.
  */
intptr_t ScintillaWrapper::GetIndent()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetIndent\n");
	return callScintilla(SCI_GETINDENT);
}

/** Indentation will only use space characters if useTabs is false, otherwise
  * it will use a combination of tabs and spaces.
  */
void ScintillaWrapper::SetUseTabs(bool useTabs)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetUseTabs\n");
	callScintilla(SCI_SETUSETABS, useTabs);
}

/** Retrieve whether tabs will be used in indentation.
  */
bool ScintillaWrapper::GetUseTabs()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetUseTabs\n");
	return 0 != (callScintilla(SCI_GETUSETABS));
}

/** Change the indentation of a line to a number of columns.
  */
void ScintillaWrapper::SetLineIndentation(intptr_t line, int indentation)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetLineIndentation\n");
	callScintilla(SCI_SETLINEINDENTATION, line, indentation);
}

/** Retrieve the number of columns that a line is indented.
  */
intptr_t ScintillaWrapper::GetLineIndentation(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineIndentation\n");
	return callScintilla(SCI_GETLINEINDENTATION, line);
}

/** Retrieve the position before the first non indentation character on a line.
  */
intptr_t ScintillaWrapper::GetLineIndentPosition(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineIndentPosition\n");
	return callScintilla(SCI_GETLINEINDENTPOSITION, line);
}

/** Retrieve the column number of a position, taking tab width into account.
  */
intptr_t ScintillaWrapper::GetColumn(intptr_t pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetColumn\n");
	return callScintilla(SCI_GETCOLUMN, pos);
}

/** Count characters between two positions.
  */
intptr_t ScintillaWrapper::CountCharacters(intptr_t start, intptr_t end)
{
	DEBUG_TRACE(L"ScintillaWrapper::CountCharacters\n");
	return callScintilla(SCI_COUNTCHARACTERS, start, end);
}

/** Count code units between two positions.
  */
intptr_t ScintillaWrapper::CountCodeUnits(intptr_t start, intptr_t end)
{
	DEBUG_TRACE(L"ScintillaWrapper::CountCodeUnits\n");
	return callScintilla(SCI_COUNTCODEUNITS, start, end);
}

/** Show or hide the horizontal scroll bar.
  */
void ScintillaWrapper::SetHScrollBar(bool visible)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetHScrollBar\n");
	callScintilla(SCI_SETHSCROLLBAR, visible);
}

/** Is the horizontal scroll bar visible?
  */
bool ScintillaWrapper::GetHScrollBar()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetHScrollBar\n");
	return 0 != (callScintilla(SCI_GETHSCROLLBAR));
}

/** Show or hide indentation guides.
  */
void ScintillaWrapper::SetIndentationGuides(int indentView)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetIndentationGuides\n");
	callScintilla(SCI_SETINDENTATIONGUIDES, indentView);
}

/** Are the indentation guides visible?
  */
int ScintillaWrapper::GetIndentationGuides()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetIndentationGuides\n");
	return callScintilla(SCI_GETINDENTATIONGUIDES);
}

/** Set the highlighted indentation guide column.
  * 0 = no highlighted guide.
  */
void ScintillaWrapper::SetHighlightGuide(intptr_t column)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetHighlightGuide\n");
	callScintilla(SCI_SETHIGHLIGHTGUIDE, column);
}

/** Get the highlighted indentation guide column.
  */
intptr_t ScintillaWrapper::GetHighlightGuide()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetHighlightGuide\n");
	return callScintilla(SCI_GETHIGHLIGHTGUIDE);
}

/** Get the position after the last visible characters on a line.
  */
intptr_t ScintillaWrapper::GetLineEndPosition(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineEndPosition\n");
	return callScintilla(SCI_GETLINEENDPOSITION, line);
}

/** Get the code page used to interpret the bytes of the document as characters.
  */
intptr_t ScintillaWrapper::GetCodePage()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCodePage\n");
	return callScintilla(SCI_GETCODEPAGE);
}

/** Get the foreground colour of the caret.
  */
boost::python::tuple ScintillaWrapper::GetCaretFore()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCaretFore\n");
	int retVal = (int)callScintilla(SCI_GETCARETFORE);
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** In read-only mode?
  */
bool ScintillaWrapper::GetReadOnly()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetReadOnly\n");
	return 0 != (callScintilla(SCI_GETREADONLY));
}

/** Sets the position of the caret.
  */
void ScintillaWrapper::SetCurrentPos(intptr_t caret)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCurrentPos\n");
	callScintilla(SCI_SETCURRENTPOS, caret);
}

/** Sets the position that starts the selection - this becomes the anchor.
  */
void ScintillaWrapper::SetSelectionStart(intptr_t anchor)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelectionStart\n");
	callScintilla(SCI_SETSELECTIONSTART, anchor);
}

/** Returns the position at the start of the selection.
  */
intptr_t ScintillaWrapper::GetSelectionStart()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionStart\n");
	return callScintilla(SCI_GETSELECTIONSTART);
}

/** Sets the position that ends the selection - this becomes the caret.
  */
void ScintillaWrapper::SetSelectionEnd(intptr_t caret)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelectionEnd\n");
	callScintilla(SCI_SETSELECTIONEND, caret);
}

/** Returns the position at the end of the selection.
  */
intptr_t ScintillaWrapper::GetSelectionEnd()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionEnd\n");
	return callScintilla(SCI_GETSELECTIONEND);
}

/** Set caret to a position, while removing any existing selection.
  */
void ScintillaWrapper::SetEmptySelection(intptr_t caret)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetEmptySelection\n");
	callScintilla(SCI_SETEMPTYSELECTION, caret);
}

/** Sets the print magnification added to the point size of each style for printing.
  */
void ScintillaWrapper::SetPrintMagnification(int magnification)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetPrintMagnification\n");
	callScintilla(SCI_SETPRINTMAGNIFICATION, magnification);
}

/** Returns the print magnification.
  */
intptr_t ScintillaWrapper::GetPrintMagnification()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetPrintMagnification\n");
	return callScintilla(SCI_GETPRINTMAGNIFICATION);
}

/** Modify colours when printing for clearer printed text.
  */
void ScintillaWrapper::SetPrintColourMode(int mode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetPrintColourMode\n");
	callScintilla(SCI_SETPRINTCOLOURMODE, mode);
}

/** Returns the print colour mode.
  */
int ScintillaWrapper::GetPrintColourMode()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetPrintColourMode\n");
	return callScintilla(SCI_GETPRINTCOLOURMODE);
}

/** Find some text in the document.
  */
boost::python::object ScintillaWrapper::FindText(int searchFlags, Sci_PositionCR start, Sci_PositionCR end, boost::python::object ft)
{
	DEBUG_TRACE(L"ScintillaWrapper::FindText\n");
	notAllowedInCallback("findText is not allowed in a synchronous callback. Use an asynchronous callback or one of the editor.search(), editor.research(), editor.replace(), editor.rereplace() methods.");
	std::string search = getStringFromObject(ft);
	Sci_TextToFind src;
	src.chrg.cpMin = start;
	src.chrg.cpMax = end;
	// We assume  findText won't write to this buffer - it should be const
	src.lpstrText = const_cast<char*>(search.c_str());
	intptr_t result = callScintilla(SCI_FINDTEXT, searchFlags, reinterpret_cast<LPARAM>(&src));
	if (-1 == result)
	{
		return boost::python::object();
	}
	else
	{
		return boost::python::make_tuple(src.chrgText.cpMin, src.chrgText.cpMax);
	}
}

/** Retrieve the display line at the top of the display.
  */
intptr_t ScintillaWrapper::GetFirstVisibleLine()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetFirstVisibleLine\n");
	return callScintilla(SCI_GETFIRSTVISIBLELINE);
}

/** Retrieve the contents of a line.
  * Returns the length of the line.
  */
boost::python::str ScintillaWrapper::GetLine(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLine\n");
	intptr_t lineCount = callScintilla(SCI_GETLINECOUNT);
	if (line >= lineCount)
	{
		throw out_of_bounds_exception();
	}
	else
	{
		PythonCompatibleStrBuffer result(callScintilla(SCI_LINELENGTH, line));
		callScintilla(SCI_GETLINE, line, reinterpret_cast<LPARAM>(*result));
		return boost::python::str(result.c_str());
	}
}

/** Returns the number of lines in the document. There is always at least one.
  */
intptr_t ScintillaWrapper::GetLineCount()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineCount\n");
	return callScintilla(SCI_GETLINECOUNT);
}

/** Sets the size in pixels of the left margin.
  */
void ScintillaWrapper::SetMarginLeft(int pixelWidth)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMarginLeft\n");
	callScintilla(SCI_SETMARGINLEFT, 0, pixelWidth);
}

/** Returns the size in pixels of the left margin.
  */
intptr_t ScintillaWrapper::GetMarginLeft()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMarginLeft\n");
	return callScintilla(SCI_GETMARGINLEFT);
}

/** Sets the size in pixels of the right margin.
  */
void ScintillaWrapper::SetMarginRight(int pixelWidth)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMarginRight\n");
	callScintilla(SCI_SETMARGINRIGHT, 0, pixelWidth);
}

/** Returns the size in pixels of the right margin.
  */
intptr_t ScintillaWrapper::GetMarginRight()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMarginRight\n");
	return callScintilla(SCI_GETMARGINRIGHT);
}

/** Is the document different from when it was last saved?
  */
bool ScintillaWrapper::GetModify()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetModify\n");
	return 0 != (callScintilla(SCI_GETMODIFY));
}

/** Select a range of text.
  */
void ScintillaWrapper::SetSel(intptr_t anchor, intptr_t caret)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSel\n");
	callScintilla(SCI_SETSEL, anchor, caret);
}

/** Retrieve the selected text.
  * Return the length of the text.
  * Result is NUL-terminated.
  */
boost::python::str ScintillaWrapper::GetSelText()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelText\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETSELTEXT));
	callScintilla(SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Retrieve a range of text.
  * Return the length of the text.
  */
boost::python::str ScintillaWrapper::GetTextRange(Sci_PositionCR start, Sci_PositionCR end)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTextRange\n");
	Sci_TextRange src;
	if (end == -1)
	{
		end = GetLength();
	}

	if (end < start)
	{
		Sci_PositionCR temp = start;
		start = end;
		end = temp;
	}
	PythonCompatibleStrBuffer result((end-start) + 1);
	src.chrg.cpMin = start;
	src.chrg.cpMax = end;
	src.lpstrText = *result;
	callScintilla(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&src));
	return boost::python::str(result.c_str());
}

/** Draw the selection either highlighted or in normal (non-highlighted) style.
  */
void ScintillaWrapper::HideSelection(bool hide)
{
	DEBUG_TRACE(L"ScintillaWrapper::HideSelection\n");
	callScintilla(SCI_HIDESELECTION, hide);
}

/** Retrieve the x value of the point in the window where a position is displayed.
  */
intptr_t ScintillaWrapper::PointXFromPosition(intptr_t pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::PointXFromPosition\n");
	return callScintilla(SCI_POINTXFROMPOSITION, 0, pos);
}

/** Retrieve the y value of the point in the window where a position is displayed.
  */
intptr_t ScintillaWrapper::PointYFromPosition(intptr_t pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::PointYFromPosition\n");
	return callScintilla(SCI_POINTYFROMPOSITION, 0, pos);
}

/** Retrieve the line containing a position.
  */
intptr_t ScintillaWrapper::LineFromPosition(intptr_t pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::LineFromPosition\n");
	return callScintilla(SCI_LINEFROMPOSITION, pos);
}

/** Retrieve the position at the start of a line.
  */
intptr_t ScintillaWrapper::PositionFromLine(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::PositionFromLine\n");
	return callScintilla(SCI_POSITIONFROMLINE, line);
}

/** Scroll horizontally and vertically.
  */
void ScintillaWrapper::LineScroll(intptr_t columns, intptr_t lines)
{
	DEBUG_TRACE(L"ScintillaWrapper::LineScroll\n");
	callScintilla(SCI_LINESCROLL, columns, lines);
}

/** Ensure the caret is visible.
  */
void ScintillaWrapper::ScrollCaret()
{
	DEBUG_TRACE(L"ScintillaWrapper::ScrollCaret\n");
	callScintilla(SCI_SCROLLCARET);
}

/** Scroll the argument positions and the range between them into view giving
  * priority to the primary position then the secondary position.
  * This may be used to make a search match visible.
  */
void ScintillaWrapper::ScrollRange(intptr_t secondary, intptr_t primary)
{
	DEBUG_TRACE(L"ScintillaWrapper::ScrollRange\n");
	callScintilla(SCI_SCROLLRANGE, secondary, primary);
}

/** Replace the selected text with the argument text.
  */
void ScintillaWrapper::ReplaceSel(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::ReplaceSel\n");
	std::string stringtext = getStringFromObject(text);
	callScintilla(SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Set to read only or read write.
  */
void ScintillaWrapper::SetReadOnly(bool readOnly)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetReadOnly\n");
	callScintilla(SCI_SETREADONLY, readOnly);
}

/** Null operation.
  */
void ScintillaWrapper::Null()
{
	DEBUG_TRACE(L"ScintillaWrapper::Null\n");
	callScintilla(SCI_NULL);
}

/** Will a paste succeed?
  */
bool ScintillaWrapper::CanPaste()
{
	DEBUG_TRACE(L"ScintillaWrapper::CanPaste\n");
	return 0 != (callScintilla(SCI_CANPASTE));
}

/** Are there any undoable actions in the undo history?
  */
bool ScintillaWrapper::CanUndo()
{
	DEBUG_TRACE(L"ScintillaWrapper::CanUndo\n");
	return 0 != (callScintilla(SCI_CANUNDO));
}

/** Delete the undo history.
  */
void ScintillaWrapper::EmptyUndoBuffer()
{
	DEBUG_TRACE(L"ScintillaWrapper::EmptyUndoBuffer\n");
	callScintilla(SCI_EMPTYUNDOBUFFER);
}

/** Undo one action in the undo history.
  */
void ScintillaWrapper::Undo()
{
	DEBUG_TRACE(L"ScintillaWrapper::Undo\n");
	callScintilla(SCI_UNDO);
}

/** Cut the selection to the clipboard.
  */
void ScintillaWrapper::Cut()
{
	DEBUG_TRACE(L"ScintillaWrapper::Cut\n");
	callScintilla(SCI_CUT);
}

/** Copy the selection to the clipboard.
  */
void ScintillaWrapper::Copy()
{
	DEBUG_TRACE(L"ScintillaWrapper::Copy\n");
	callScintilla(SCI_COPY);
}

/** Paste the contents of the clipboard into the document replacing the selection.
  */
void ScintillaWrapper::Paste()
{
	DEBUG_TRACE(L"ScintillaWrapper::Paste\n");
	callScintilla(SCI_PASTE);
}

/** Clear the selection.
  */
void ScintillaWrapper::Clear()
{
	DEBUG_TRACE(L"ScintillaWrapper::Clear\n");
	callScintilla(SCI_CLEAR);
}

/** Replace the contents of the document with the argument text.
  */
void ScintillaWrapper::SetText(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetText\n");
	std::string stringtext = getStringFromObject(text);
	callScintilla(SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Retrieve all the text in the document.
  * Returns number of characters retrieved.
  * Result is NUL-terminated.
  */
boost::python::str ScintillaWrapper::GetText()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetText\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETTEXT) + 1);
	// result.size() does not depend on the order of evaluation here
	//lint -e{864}
	callScintilla(SCI_GETTEXT, result.size(), reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Retrieve the number of characters in the document.
  */
intptr_t ScintillaWrapper::GetTextLength()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTextLength\n");
	return callScintilla(SCI_GETTEXTLENGTH);
}

/** Retrieve a pointer to a function that processes messages for this Scintilla.
  */
intptr_t ScintillaWrapper::GetDirectFunction()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetDirectFunction\n");
	return callScintilla(SCI_GETDIRECTFUNCTION);
}

/** Retrieve a pointer value to use as the first argument when calling
  * the function returned by GetDirectFunction.
  */
intptr_t ScintillaWrapper::GetDirectPointer()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetDirectPointer\n");
	return callScintilla(SCI_GETDIRECTPOINTER);
}

/** Set to overtype (true) or insert mode.
  */
void ScintillaWrapper::SetOvertype(bool overType)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetOvertype\n");
	callScintilla(SCI_SETOVERTYPE, overType);
}

/** Returns true if overtype mode is active otherwise false is returned.
  */
bool ScintillaWrapper::GetOvertype()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetOvertype\n");
	return 0 != (callScintilla(SCI_GETOVERTYPE));
}

/** Set the width of the insert mode caret.
  */
void ScintillaWrapper::SetCaretWidth(int pixelWidth)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCaretWidth\n");
	callScintilla(SCI_SETCARETWIDTH, pixelWidth);
}

/** Returns the width of the insert mode caret.
  */
intptr_t ScintillaWrapper::GetCaretWidth()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCaretWidth\n");
	return callScintilla(SCI_GETCARETWIDTH);
}

/** Sets the position that starts the target which is used for updating the
  * document without affecting the scroll position.
  */
void ScintillaWrapper::SetTargetStart(intptr_t start)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetTargetStart\n");
	callScintilla(SCI_SETTARGETSTART, start);
}

/** Get the position that starts the target.
  */
intptr_t ScintillaWrapper::GetTargetStart()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTargetStart\n");
	return callScintilla(SCI_GETTARGETSTART);
}

/** Sets the virtual space of the target start
  */
void ScintillaWrapper::SetTargetStartVirtualSpace(intptr_t space)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetTargetStartVirtualSpace\n");
	callScintilla(SCI_SETTARGETSTARTVIRTUALSPACE, space);
}

/** Get the virtual space of the target start
  */
intptr_t ScintillaWrapper::GetTargetStartVirtualSpace()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTargetStartVirtualSpace\n");
	return callScintilla(SCI_GETTARGETSTARTVIRTUALSPACE);
}

/** Sets the position that ends the target which is used for updating the
  * document without affecting the scroll position.
  */
void ScintillaWrapper::SetTargetEnd(intptr_t end)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetTargetEnd\n");
	callScintilla(SCI_SETTARGETEND, end);
}

/** Get the position that ends the target.
  */
intptr_t ScintillaWrapper::GetTargetEnd()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTargetEnd\n");
	return callScintilla(SCI_GETTARGETEND);
}

/** Sets the virtual space of the target end
  */
void ScintillaWrapper::SetTargetEndVirtualSpace(intptr_t space)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetTargetEndVirtualSpace\n");
	callScintilla(SCI_SETTARGETENDVIRTUALSPACE, space);
}

/** Get the virtual space of the target end
  */
intptr_t ScintillaWrapper::GetTargetEndVirtualSpace()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTargetEndVirtualSpace\n");
	return callScintilla(SCI_GETTARGETENDVIRTUALSPACE);
}

/** Sets both the start and end of the target in one call.
  */
void ScintillaWrapper::SetTargetRange(intptr_t start, intptr_t end)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetTargetRange\n");
	callScintilla(SCI_SETTARGETRANGE, start, end);
}

/** Retrieve the text in the target.
  */
boost::python::str ScintillaWrapper::GetTargetText()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTargetText\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETTARGETTEXT));
	callScintilla(SCI_GETTARGETTEXT, 0, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Make the target range start and end be the same as the selection range start and end.
  */
void ScintillaWrapper::TargetFromSelection()
{
	DEBUG_TRACE(L"ScintillaWrapper::TargetFromSelection\n");
	callScintilla(SCI_TARGETFROMSELECTION);
}

/** Sets the target to the whole document.
  */
void ScintillaWrapper::TargetWholeDocument()
{
	DEBUG_TRACE(L"ScintillaWrapper::TargetWholeDocument\n");
	callScintilla(SCI_TARGETWHOLEDOCUMENT);
}

/** Replace the target text with the argument text.
  * Text is counted so it can contain NULs.
  * Returns the length of the replacement text.
  */
intptr_t ScintillaWrapper::ReplaceTarget(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::ReplaceTarget\n");
	std::string s = getStringFromObject(text);
	return callScintilla(SCI_REPLACETARGET, s.size(), reinterpret_cast<LPARAM>(s.c_str()));
}

/** Replace the target text with the argument text after \d processing.
  * Text is counted so it can contain NULs.
  * Looks for \d where d is between 1 and 9 and replaces these with the strings
  * matched in the last search operation which were surrounded by \( and \).
  * Returns the length of the replacement text including any change
  * caused by processing the \d patterns.
  */
intptr_t ScintillaWrapper::ReplaceTargetRE(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::ReplaceTargetRE\n");
	std::string s = getStringFromObject(text);
	return callScintilla(SCI_REPLACETARGETRE, s.size(), reinterpret_cast<LPARAM>(s.c_str()));
}

/** Search for a counted string in the target and set the target to the found
  * range. Text is counted so it can contain NULs.
  * Returns start of found range or -1 for failure in which case target is not moved.
  */
intptr_t ScintillaWrapper::SearchInTarget(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::SearchInTarget\n");
	notAllowedInCallback("searchInTarget is not allowed in a synchronous callback. Use an asynchronous callback or one of the editor.search(), editor.research(), editor.replace(), editor.rereplace() methods.");
	std::string s = getStringFromObject(text);
	return callScintilla(SCI_SEARCHINTARGET, s.size(), reinterpret_cast<LPARAM>(s.c_str()));
}

/** Set the search flags used by SearchInTarget.
  */
void ScintillaWrapper::SetSearchFlags(int searchFlags)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSearchFlags\n");
	callScintilla(SCI_SETSEARCHFLAGS, searchFlags);
}

/** Get the search flags used by SearchInTarget.
  */
int ScintillaWrapper::GetSearchFlags()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSearchFlags\n");
	return callScintilla(SCI_GETSEARCHFLAGS);
}

/** Show a call tip containing a definition near position pos.
  */
void ScintillaWrapper::CallTipShow(intptr_t pos, boost::python::object definition)
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipShow\n");
	std::string stringdefinition = getStringFromObject(definition);
	callScintilla(SCI_CALLTIPSHOW, pos, reinterpret_cast<LPARAM>(stringdefinition.c_str()));
}

/** Remove the call tip from the screen.
  */
void ScintillaWrapper::CallTipCancel()
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipCancel\n");
	callScintilla(SCI_CALLTIPCANCEL);
}

/** Is there an active call tip?
  */
bool ScintillaWrapper::CallTipActive()
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipActive\n");
	return 0 != (callScintilla(SCI_CALLTIPACTIVE));
}

/** Retrieve the position where the caret was before displaying the call tip.
  */
intptr_t ScintillaWrapper::CallTipPosStart()
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipPosStart\n");
	return callScintilla(SCI_CALLTIPPOSSTART);
}

/** Set the start position in order to change when backspacing removes the calltip.
  */
void ScintillaWrapper::CallTipSetPosStart(intptr_t posStart)
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipSetPosStart\n");
	callScintilla(SCI_CALLTIPSETPOSSTART, posStart);
}

/** Highlight a segment of the definition.
  */
void ScintillaWrapper::CallTipSetHlt(intptr_t highlightStart, intptr_t highlightEnd)
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipSetHlt\n");
	callScintilla(SCI_CALLTIPSETHLT, highlightStart, highlightEnd);
}

/** Set the background colour for the call tip.
  */
void ScintillaWrapper::CallTipSetBack(boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipSetBack\n");
	COLORREF rgbback = MAKECOLOUR(back);
	callScintilla(SCI_CALLTIPSETBACK, static_cast<WPARAM>(rgbback));
}

/** Set the foreground colour for the call tip.
  */
void ScintillaWrapper::CallTipSetFore(boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipSetFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	callScintilla(SCI_CALLTIPSETFORE, static_cast<WPARAM>(rgbfore));
}

/** Set the foreground colour for the highlighted part of the call tip.
  */
void ScintillaWrapper::CallTipSetForeHlt(boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipSetForeHlt\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	callScintilla(SCI_CALLTIPSETFOREHLT, static_cast<WPARAM>(rgbfore));
}

/** Enable use of STYLE_CALLTIP and set call tip tab size in pixels.
  */
void ScintillaWrapper::CallTipUseStyle(int tabSize)
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipUseStyle\n");
	callScintilla(SCI_CALLTIPUSESTYLE, tabSize);
}

/** Set position of calltip, above or below text.
  */
void ScintillaWrapper::CallTipSetPosition(bool above)
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipSetPosition\n");
	callScintilla(SCI_CALLTIPSETPOSITION, above);
}

/** Find the display line of a document line taking hidden lines into account.
  */
intptr_t ScintillaWrapper::VisibleFromDocLine(intptr_t docLine)
{
	DEBUG_TRACE(L"ScintillaWrapper::VisibleFromDocLine\n");
	return callScintilla(SCI_VISIBLEFROMDOCLINE, docLine);
}

/** Find the document line of a display line taking hidden lines into account.
  */
intptr_t ScintillaWrapper::DocLineFromVisible(intptr_t displayLine)
{
	DEBUG_TRACE(L"ScintillaWrapper::DocLineFromVisible\n");
	return callScintilla(SCI_DOCLINEFROMVISIBLE, displayLine);
}

/** The number of display lines needed to wrap a document line
  */
intptr_t ScintillaWrapper::WrapCount(intptr_t docLine)
{
	DEBUG_TRACE(L"ScintillaWrapper::WrapCount\n");
	return callScintilla(SCI_WRAPCOUNT, docLine);
}

/** Set the fold level of a line.
  * This encodes an integer level along with flags indicating whether the
  * line is a header and whether it is effectively white space.
  */
void ScintillaWrapper::SetFoldLevel(intptr_t line, int level)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetFoldLevel\n");
	callScintilla(SCI_SETFOLDLEVEL, line, level);
}

/** Retrieve the fold level of a line.
  */
int ScintillaWrapper::GetFoldLevel(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetFoldLevel\n");
	return callScintilla(SCI_GETFOLDLEVEL, line);
}

/** Find the last child line of a header line.
  */
intptr_t ScintillaWrapper::GetLastChild(intptr_t line, int level)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLastChild\n");
	return callScintilla(SCI_GETLASTCHILD, line, level);
}

/** Find the parent line of a child line.
  */
intptr_t ScintillaWrapper::GetFoldParent(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetFoldParent\n");
	return callScintilla(SCI_GETFOLDPARENT, line);
}

/** Make a range of lines visible.
  */
void ScintillaWrapper::ShowLines(intptr_t lineStart, intptr_t lineEnd)
{
	DEBUG_TRACE(L"ScintillaWrapper::ShowLines\n");
	callScintilla(SCI_SHOWLINES, lineStart, lineEnd);
}

/** Make a range of lines invisible.
  */
void ScintillaWrapper::HideLines(intptr_t lineStart, intptr_t lineEnd)
{
	DEBUG_TRACE(L"ScintillaWrapper::HideLines\n");
	callScintilla(SCI_HIDELINES, lineStart, lineEnd);
}

/** Is a line visible?
  */
bool ScintillaWrapper::GetLineVisible(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineVisible\n");
	return 0 != (callScintilla(SCI_GETLINEVISIBLE, line));
}

/** Are all lines visible?
  */
bool ScintillaWrapper::GetAllLinesVisible()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetAllLinesVisible\n");
	return 0 != (callScintilla(SCI_GETALLLINESVISIBLE));
}

/** Show the children of a header line.
  */
void ScintillaWrapper::SetFoldExpanded(intptr_t line, bool expanded)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetFoldExpanded\n");
	callScintilla(SCI_SETFOLDEXPANDED, line, expanded);
}

/** Is a header line expanded?
  */
bool ScintillaWrapper::GetFoldExpanded(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetFoldExpanded\n");
	return 0 != (callScintilla(SCI_GETFOLDEXPANDED, line));
}

/** Switch a header line between expanded and contracted.
  */
void ScintillaWrapper::ToggleFold(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::ToggleFold\n");
	callScintilla(SCI_TOGGLEFOLD, line);
}

/** Switch a header line between expanded and contracted and show some text after the line.
  */
void ScintillaWrapper::ToggleFoldShowText(intptr_t line, boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::ToggleFoldShowText\n");
	std::string stringtext = getStringFromObject(text);
	callScintilla(SCI_TOGGLEFOLDSHOWTEXT, line, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Set the style of fold display text.
  */
void ScintillaWrapper::FoldDisplayTextSetStyle(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::FoldDisplayTextSetStyle\n");
	callScintilla(SCI_FOLDDISPLAYTEXTSETSTYLE, style);
}

/** Get the style of fold display text.
  */
int ScintillaWrapper::FoldDisplayTextGetStyle()
{
	DEBUG_TRACE(L"ScintillaWrapper::FoldDisplayTextGetStyle\n");
	return callScintilla(SCI_FOLDDISPLAYTEXTGETSTYLE);
}

/** Set the default fold display text.
  */
void ScintillaWrapper::SetDefaultFoldDisplayText(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetDefaultFoldDisplayText\n");
	std::string stringtext = getStringFromObject(text);
	callScintilla(SCI_SETDEFAULTFOLDDISPLAYTEXT, 0, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Get the default fold display text.
  */
boost::python::str ScintillaWrapper::GetDefaultFoldDisplayText()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetDefaultFoldDisplayText\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETDEFAULTFOLDDISPLAYTEXT));
	callScintilla(SCI_GETDEFAULTFOLDDISPLAYTEXT, 0, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Expand or contract a fold header.
  */
void ScintillaWrapper::FoldLine(intptr_t line, int action)
{
	DEBUG_TRACE(L"ScintillaWrapper::FoldLine\n");
	callScintilla(SCI_FOLDLINE, line, action);
}

/** Expand or contract a fold header and its children.
  */
void ScintillaWrapper::FoldChildren(intptr_t line, int action)
{
	DEBUG_TRACE(L"ScintillaWrapper::FoldChildren\n");
	callScintilla(SCI_FOLDCHILDREN, line, action);
}

/** Expand a fold header and all children. Use the level argument instead of the line's current level.
  */
void ScintillaWrapper::ExpandChildren(intptr_t line, int level)
{
	DEBUG_TRACE(L"ScintillaWrapper::ExpandChildren\n");
	callScintilla(SCI_EXPANDCHILDREN, line, level);
}

/** Expand or contract all fold headers.
  */
void ScintillaWrapper::FoldAll(int action)
{
	DEBUG_TRACE(L"ScintillaWrapper::FoldAll\n");
	callScintilla(SCI_FOLDALL, action);
}

/** Ensure a particular line is visible by expanding any header line hiding it.
  */
void ScintillaWrapper::EnsureVisible(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::EnsureVisible\n");
	callScintilla(SCI_ENSUREVISIBLE, line);
}

/** Set automatic folding behaviours.
  */
void ScintillaWrapper::SetAutomaticFold(int automaticFold)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetAutomaticFold\n");
	callScintilla(SCI_SETAUTOMATICFOLD, automaticFold);
}

/** Get automatic folding behaviours.
  */
int ScintillaWrapper::GetAutomaticFold()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetAutomaticFold\n");
	return callScintilla(SCI_GETAUTOMATICFOLD);
}

/** Set some style options for folding.
  */
void ScintillaWrapper::SetFoldFlags(int flags)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetFoldFlags\n");
	callScintilla(SCI_SETFOLDFLAGS, flags);
}

/** Ensure a particular line is visible by expanding any header line hiding it.
  * Use the currently set visibility policy to determine which range to display.
  */
void ScintillaWrapper::EnsureVisibleEnforcePolicy(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::EnsureVisibleEnforcePolicy\n");
	callScintilla(SCI_ENSUREVISIBLEENFORCEPOLICY, line);
}

/** Sets whether a tab pressed when caret is within indentation indents.
  */
void ScintillaWrapper::SetTabIndents(bool tabIndents)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetTabIndents\n");
	callScintilla(SCI_SETTABINDENTS, tabIndents);
}

/** Does a tab pressed when caret is within indentation indent?
  */
bool ScintillaWrapper::GetTabIndents()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTabIndents\n");
	return 0 != (callScintilla(SCI_GETTABINDENTS));
}

/** Sets whether a backspace pressed when caret is within indentation unindents.
  */
void ScintillaWrapper::SetBackSpaceUnIndents(bool bsUnIndents)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetBackSpaceUnIndents\n");
	callScintilla(SCI_SETBACKSPACEUNINDENTS, bsUnIndents);
}

/** Does a backspace pressed when caret is within indentation unindent?
  */
bool ScintillaWrapper::GetBackSpaceUnIndents()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetBackSpaceUnIndents\n");
	return 0 != (callScintilla(SCI_GETBACKSPACEUNINDENTS));
}

/** Sets the time the mouse must sit still to generate a mouse dwell event.
  */
void ScintillaWrapper::SetMouseDwellTime(int periodMilliseconds)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMouseDwellTime\n");
	callScintilla(SCI_SETMOUSEDWELLTIME, periodMilliseconds);
}

/** Retrieve the time the mouse must sit still to generate a mouse dwell event.
  */
intptr_t ScintillaWrapper::GetMouseDwellTime()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMouseDwellTime\n");
	return callScintilla(SCI_GETMOUSEDWELLTIME);
}

/** Get position of start of word.
  */
intptr_t ScintillaWrapper::WordStartPosition(intptr_t pos, bool onlyWordCharacters)
{
	DEBUG_TRACE(L"ScintillaWrapper::WordStartPosition\n");
	return callScintilla(SCI_WORDSTARTPOSITION, pos, onlyWordCharacters);
}

/** Get position of end of word.
  */
intptr_t ScintillaWrapper::WordEndPosition(intptr_t pos, bool onlyWordCharacters)
{
	DEBUG_TRACE(L"ScintillaWrapper::WordEndPosition\n");
	return callScintilla(SCI_WORDENDPOSITION, pos, onlyWordCharacters);
}

/** Is the range start..end considered a word?
  */
bool ScintillaWrapper::IsRangeWord(intptr_t start, intptr_t end)
{
	DEBUG_TRACE(L"ScintillaWrapper::IsRangeWord\n");
	return 0 != (callScintilla(SCI_ISRANGEWORD, start, end));
}

/** Sets limits to idle styling.
  */
void ScintillaWrapper::SetIdleStyling(int idleStyling)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetIdleStyling\n");
	callScintilla(SCI_SETIDLESTYLING, idleStyling);
}

/** Retrieve the limits to idle styling.
  */
int ScintillaWrapper::GetIdleStyling()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetIdleStyling\n");
	return callScintilla(SCI_GETIDLESTYLING);
}

/** Sets whether text is word wrapped.
  */
void ScintillaWrapper::SetWrapMode(int wrapMode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWrapMode\n");
	callScintilla(SCI_SETWRAPMODE, wrapMode);
}

/** Retrieve whether text is word wrapped.
  */
int ScintillaWrapper::GetWrapMode()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetWrapMode\n");
	return callScintilla(SCI_GETWRAPMODE);
}

/** Set the display mode of visual flags for wrapped lines.
  */
void ScintillaWrapper::SetWrapVisualFlags(int wrapVisualFlags)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWrapVisualFlags\n");
	callScintilla(SCI_SETWRAPVISUALFLAGS, wrapVisualFlags);
}

/** Retrive the display mode of visual flags for wrapped lines.
  */
int ScintillaWrapper::GetWrapVisualFlags()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetWrapVisualFlags\n");
	return callScintilla(SCI_GETWRAPVISUALFLAGS);
}

/** Set the location of visual flags for wrapped lines.
  */
void ScintillaWrapper::SetWrapVisualFlagsLocation(int wrapVisualFlagsLocation)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWrapVisualFlagsLocation\n");
	callScintilla(SCI_SETWRAPVISUALFLAGSLOCATION, wrapVisualFlagsLocation);
}

/** Retrive the location of visual flags for wrapped lines.
  */
int ScintillaWrapper::GetWrapVisualFlagsLocation()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetWrapVisualFlagsLocation\n");
	return callScintilla(SCI_GETWRAPVISUALFLAGSLOCATION);
}

/** Set the start indent for wrapped lines.
  */
void ScintillaWrapper::SetWrapStartIndent(int indent)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWrapStartIndent\n");
	callScintilla(SCI_SETWRAPSTARTINDENT, indent);
}

/** Retrive the start indent for wrapped lines.
  */
intptr_t ScintillaWrapper::GetWrapStartIndent()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetWrapStartIndent\n");
	return callScintilla(SCI_GETWRAPSTARTINDENT);
}

/** Sets how wrapped sublines are placed. Default is fixed.
  */
void ScintillaWrapper::SetWrapIndentMode(int wrapIndentMode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWrapIndentMode\n");
	callScintilla(SCI_SETWRAPINDENTMODE, wrapIndentMode);
}

/** Retrieve how wrapped sublines are placed. Default is fixed.
  */
int ScintillaWrapper::GetWrapIndentMode()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetWrapIndentMode\n");
	return callScintilla(SCI_GETWRAPINDENTMODE);
}

/** Sets the degree of caching of layout information.
  */
void ScintillaWrapper::SetLayoutCache(int cacheMode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetLayoutCache\n");
	callScintilla(SCI_SETLAYOUTCACHE, cacheMode);
}

/** Retrieve the degree of caching of layout information.
  */
int ScintillaWrapper::GetLayoutCache()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLayoutCache\n");
	return callScintilla(SCI_GETLAYOUTCACHE);
}

/** Sets the document width assumed for scrolling.
  */
void ScintillaWrapper::SetScrollWidth(int pixelWidth)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetScrollWidth\n");
	callScintilla(SCI_SETSCROLLWIDTH, pixelWidth);
}

/** Retrieve the document width assumed for scrolling.
  */
intptr_t ScintillaWrapper::GetScrollWidth()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetScrollWidth\n");
	return callScintilla(SCI_GETSCROLLWIDTH);
}

/** Sets whether the maximum width line displayed is used to set scroll width.
  */
void ScintillaWrapper::SetScrollWidthTracking(bool tracking)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetScrollWidthTracking\n");
	callScintilla(SCI_SETSCROLLWIDTHTRACKING, tracking);
}

/** Retrieve whether the scroll width tracks wide lines.
  */
bool ScintillaWrapper::GetScrollWidthTracking()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetScrollWidthTracking\n");
	return 0 != (callScintilla(SCI_GETSCROLLWIDTHTRACKING));
}

/** Measure the pixel width of some text in a particular style.
  * NUL terminated text argument.
  * Does not handle tab or control characters.
  */
intptr_t ScintillaWrapper::TextWidth(int style, boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::TextWidth\n");
	std::string stringtext = getStringFromObject(text);
	return callScintilla(SCI_TEXTWIDTH, style, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Sets the scroll range so that maximum scroll position has
  * the last line at the bottom of the view (default).
  * Setting this to false allows scrolling one page below the last line.
  */
void ScintillaWrapper::SetEndAtLastLine(bool endAtLastLine)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetEndAtLastLine\n");
	callScintilla(SCI_SETENDATLASTLINE, endAtLastLine);
}

/** Retrieve whether the maximum scroll position has the last
  * line at the bottom of the view.
  */
bool ScintillaWrapper::GetEndAtLastLine()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetEndAtLastLine\n");
	return 0 != (callScintilla(SCI_GETENDATLASTLINE));
}

/** Retrieve the height of a particular line of text in pixels.
  */
intptr_t ScintillaWrapper::TextHeight(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::TextHeight\n");
	return callScintilla(SCI_TEXTHEIGHT, line);
}

/** Show or hide the vertical scroll bar.
  */
void ScintillaWrapper::SetVScrollBar(bool visible)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetVScrollBar\n");
	callScintilla(SCI_SETVSCROLLBAR, visible);
}

/** Is the vertical scroll bar visible?
  */
bool ScintillaWrapper::GetVScrollBar()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetVScrollBar\n");
	return 0 != (callScintilla(SCI_GETVSCROLLBAR));
}

/** Append a string to the end of the document without changing the selection.
  */
intptr_t ScintillaWrapper::AppendText(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::AppendText\n");
	std::string s = getStringFromObject(text);
	return callScintilla(SCI_APPENDTEXT, s.size(), reinterpret_cast<LPARAM>(s.c_str()));
}

/** How many phases is drawing done in?
  */
int ScintillaWrapper::GetPhasesDraw()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetPhasesDraw\n");
	return callScintilla(SCI_GETPHASESDRAW);
}

/** In one phase draw, text is drawn in a series of rectangular blocks with no overlap.
  * In two phase draw, text is drawn in a series of lines allowing runs to overlap horizontally.
  * In multiple phase draw, each element is drawn over the whole drawing area, allowing text
  * to overlap from one line to the next.
  */
void ScintillaWrapper::SetPhasesDraw(int phases)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetPhasesDraw\n");
	callScintilla(SCI_SETPHASESDRAW, phases);
}

/** Choose the quality level for text from the FontQuality enumeration.
  */
void ScintillaWrapper::SetFontQuality(int fontQuality)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetFontQuality\n");
	callScintilla(SCI_SETFONTQUALITY, fontQuality);
}

/** Retrieve the quality level for text.
  */
int ScintillaWrapper::GetFontQuality()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetFontQuality\n");
	return callScintilla(SCI_GETFONTQUALITY);
}

/** Scroll so that a display line is at the top of the display.
  */
void ScintillaWrapper::SetFirstVisibleLine(intptr_t displayLine)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetFirstVisibleLine\n");
	callScintilla(SCI_SETFIRSTVISIBLELINE, displayLine);
}

/** Change the effect of pasting when there are multiple selections.
  */
void ScintillaWrapper::SetMultiPaste(int multiPaste)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMultiPaste\n");
	callScintilla(SCI_SETMULTIPASTE, multiPaste);
}

/** Retrieve the effect of pasting when there are multiple selections.
  */
int ScintillaWrapper::GetMultiPaste()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMultiPaste\n");
	return callScintilla(SCI_GETMULTIPASTE);
}

/** Retrieve the value of a tag from a regular expression search.
  * Result is NUL-terminated.
  */
boost::python::str ScintillaWrapper::GetTag(int tagNumber)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTag\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETTAG, tagNumber));
	callScintilla(SCI_GETTAG, tagNumber, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Join the lines in the target.
  */
void ScintillaWrapper::LinesJoin()
{
	DEBUG_TRACE(L"ScintillaWrapper::LinesJoin\n");
	callScintilla(SCI_LINESJOIN);
}

/** Split the lines in the target into lines that are less wide than pixelWidth
  * where possible.
  */
void ScintillaWrapper::LinesSplit(int pixelWidth)
{
	DEBUG_TRACE(L"ScintillaWrapper::LinesSplit\n");
	callScintilla(SCI_LINESSPLIT, pixelWidth);
}

/** Set one of the colours used as a chequerboard pattern in the fold margin
  */
void ScintillaWrapper::SetFoldMarginColour(bool useSetting, boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetFoldMarginColour\n");
	COLORREF rgbback = MAKECOLOUR(back);
	callScintilla(SCI_SETFOLDMARGINCOLOUR, useSetting, static_cast<LPARAM>(rgbback));
}

/** Set the other colour used as a chequerboard pattern in the fold margin
  */
void ScintillaWrapper::SetFoldMarginHiColour(bool useSetting, boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetFoldMarginHiColour\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	callScintilla(SCI_SETFOLDMARGINHICOLOUR, useSetting, static_cast<LPARAM>(rgbfore));
}

/** Enable or disable accessibility.
  */
void ScintillaWrapper::SetAccessibility(int accessibility)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetAccessibility\n");
	callScintilla(SCI_SETACCESSIBILITY, accessibility);
}

/** Report accessibility status.
  */
int ScintillaWrapper::GetAccessibility()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetAccessibility\n");
	return callScintilla(SCI_GETACCESSIBILITY);
}

/** Move caret down one line.
  */
void ScintillaWrapper::LineDown()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineDown\n");
	callScintilla(SCI_LINEDOWN);
}

/** Move caret down one line extending selection to new caret position.
  */
void ScintillaWrapper::LineDownExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineDownExtend\n");
	callScintilla(SCI_LINEDOWNEXTEND);
}

/** Move caret up one line.
  */
void ScintillaWrapper::LineUp()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineUp\n");
	callScintilla(SCI_LINEUP);
}

/** Move caret up one line extending selection to new caret position.
  */
void ScintillaWrapper::LineUpExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineUpExtend\n");
	callScintilla(SCI_LINEUPEXTEND);
}

/** Move caret left one character.
  */
void ScintillaWrapper::CharLeft()
{
	DEBUG_TRACE(L"ScintillaWrapper::CharLeft\n");
	callScintilla(SCI_CHARLEFT);
}

/** Move caret left one character extending selection to new caret position.
  */
void ScintillaWrapper::CharLeftExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::CharLeftExtend\n");
	callScintilla(SCI_CHARLEFTEXTEND);
}

/** Move caret right one character.
  */
void ScintillaWrapper::CharRight()
{
	DEBUG_TRACE(L"ScintillaWrapper::CharRight\n");
	callScintilla(SCI_CHARRIGHT);
}

/** Move caret right one character extending selection to new caret position.
  */
void ScintillaWrapper::CharRightExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::CharRightExtend\n");
	callScintilla(SCI_CHARRIGHTEXTEND);
}

/** Move caret left one word.
  */
void ScintillaWrapper::WordLeft()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordLeft\n");
	callScintilla(SCI_WORDLEFT);
}

/** Move caret left one word extending selection to new caret position.
  */
void ScintillaWrapper::WordLeftExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordLeftExtend\n");
	callScintilla(SCI_WORDLEFTEXTEND);
}

/** Move caret right one word.
  */
void ScintillaWrapper::WordRight()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordRight\n");
	callScintilla(SCI_WORDRIGHT);
}

/** Move caret right one word extending selection to new caret position.
  */
void ScintillaWrapper::WordRightExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordRightExtend\n");
	callScintilla(SCI_WORDRIGHTEXTEND);
}

/** Move caret to first position on line.
  */
void ScintillaWrapper::Home()
{
	DEBUG_TRACE(L"ScintillaWrapper::Home\n");
	callScintilla(SCI_HOME);
}

/** Move caret to first position on line extending selection to new caret position.
  */
void ScintillaWrapper::HomeExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::HomeExtend\n");
	callScintilla(SCI_HOMEEXTEND);
}

/** Move caret to last position on line.
  */
void ScintillaWrapper::LineEnd()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineEnd\n");
	callScintilla(SCI_LINEEND);
}

/** Move caret to last position on line extending selection to new caret position.
  */
void ScintillaWrapper::LineEndExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineEndExtend\n");
	callScintilla(SCI_LINEENDEXTEND);
}

/** Move caret to first position in document.
  */
void ScintillaWrapper::DocumentStart()
{
	DEBUG_TRACE(L"ScintillaWrapper::DocumentStart\n");
	callScintilla(SCI_DOCUMENTSTART);
}

/** Move caret to first position in document extending selection to new caret position.
  */
void ScintillaWrapper::DocumentStartExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::DocumentStartExtend\n");
	callScintilla(SCI_DOCUMENTSTARTEXTEND);
}

/** Move caret to last position in document.
  */
void ScintillaWrapper::DocumentEnd()
{
	DEBUG_TRACE(L"ScintillaWrapper::DocumentEnd\n");
	callScintilla(SCI_DOCUMENTEND);
}

/** Move caret to last position in document extending selection to new caret position.
  */
void ScintillaWrapper::DocumentEndExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::DocumentEndExtend\n");
	callScintilla(SCI_DOCUMENTENDEXTEND);
}

/** Move caret one page up.
  */
void ScintillaWrapper::PageUp()
{
	DEBUG_TRACE(L"ScintillaWrapper::PageUp\n");
	callScintilla(SCI_PAGEUP);
}

/** Move caret one page up extending selection to new caret position.
  */
void ScintillaWrapper::PageUpExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::PageUpExtend\n");
	callScintilla(SCI_PAGEUPEXTEND);
}

/** Move caret one page down.
  */
void ScintillaWrapper::PageDown()
{
	DEBUG_TRACE(L"ScintillaWrapper::PageDown\n");
	callScintilla(SCI_PAGEDOWN);
}

/** Move caret one page down extending selection to new caret position.
  */
void ScintillaWrapper::PageDownExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::PageDownExtend\n");
	callScintilla(SCI_PAGEDOWNEXTEND);
}

/** Switch from insert to overtype mode or the reverse.
  */
void ScintillaWrapper::EditToggleOvertype()
{
	DEBUG_TRACE(L"ScintillaWrapper::EditToggleOvertype\n");
	callScintilla(SCI_EDITTOGGLEOVERTYPE);
}

/** Cancel any modes such as call tip or auto-completion list display.
  */
void ScintillaWrapper::Cancel()
{
	DEBUG_TRACE(L"ScintillaWrapper::Cancel\n");
	callScintilla(SCI_CANCEL);
}

/** Delete the selection or if no selection, the character before the caret.
  */
void ScintillaWrapper::DeleteBack()
{
	DEBUG_TRACE(L"ScintillaWrapper::DeleteBack\n");
	callScintilla(SCI_DELETEBACK);
}

/** If selection is empty or all on one line replace the selection with a tab character.
  * If more than one line selected, indent the lines.
  */
void ScintillaWrapper::Tab()
{
	DEBUG_TRACE(L"ScintillaWrapper::Tab\n");
	callScintilla(SCI_TAB);
}

/** Dedent the selected lines.
  */
void ScintillaWrapper::BackTab()
{
	DEBUG_TRACE(L"ScintillaWrapper::BackTab\n");
	callScintilla(SCI_BACKTAB);
}

/** Insert a new line, may use a CRLF, CR or LF depending on EOL mode.
  */
void ScintillaWrapper::NewLine()
{
	DEBUG_TRACE(L"ScintillaWrapper::NewLine\n");
	callScintilla(SCI_NEWLINE);
}

/** Insert a Form Feed character.
  */
void ScintillaWrapper::FormFeed()
{
	DEBUG_TRACE(L"ScintillaWrapper::FormFeed\n");
	callScintilla(SCI_FORMFEED);
}

/** Move caret to before first visible character on line.
  * If already there move to first character on line.
  */
void ScintillaWrapper::VCHome()
{
	DEBUG_TRACE(L"ScintillaWrapper::VCHome\n");
	callScintilla(SCI_VCHOME);
}

/** Like VCHome but extending selection to new caret position.
  */
void ScintillaWrapper::VCHomeExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::VCHomeExtend\n");
	callScintilla(SCI_VCHOMEEXTEND);
}

/** Magnify the displayed text by increasing the sizes by 1 point.
  */
void ScintillaWrapper::ZoomIn()
{
	DEBUG_TRACE(L"ScintillaWrapper::ZoomIn\n");
	callScintilla(SCI_ZOOMIN);
}

/** Make the displayed text smaller by decreasing the sizes by 1 point.
  */
void ScintillaWrapper::ZoomOut()
{
	DEBUG_TRACE(L"ScintillaWrapper::ZoomOut\n");
	callScintilla(SCI_ZOOMOUT);
}

/** Delete the word to the left of the caret.
  */
void ScintillaWrapper::DelWordLeft()
{
	DEBUG_TRACE(L"ScintillaWrapper::DelWordLeft\n");
	callScintilla(SCI_DELWORDLEFT);
}

/** Delete the word to the right of the caret.
  */
void ScintillaWrapper::DelWordRight()
{
	DEBUG_TRACE(L"ScintillaWrapper::DelWordRight\n");
	callScintilla(SCI_DELWORDRIGHT);
}

/** Delete the word to the right of the caret, but not the trailing non-word characters.
  */
void ScintillaWrapper::DelWordRightEnd()
{
	DEBUG_TRACE(L"ScintillaWrapper::DelWordRightEnd\n");
	callScintilla(SCI_DELWORDRIGHTEND);
}

/** Cut the line containing the caret.
  */
void ScintillaWrapper::LineCut()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineCut\n");
	callScintilla(SCI_LINECUT);
}

/** Delete the line containing the caret.
  */
void ScintillaWrapper::LineDelete()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineDelete\n");
	callScintilla(SCI_LINEDELETE);
}

/** Switch the current line with the previous.
  */
void ScintillaWrapper::LineTranspose()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineTranspose\n");
	callScintilla(SCI_LINETRANSPOSE);
}

/** Reverse order of selected lines.
  */
void ScintillaWrapper::LineReverse()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineReverse\n");
	callScintilla(SCI_LINEREVERSE);
}

/** Duplicate the current line.
  */
void ScintillaWrapper::LineDuplicate()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineDuplicate\n");
	callScintilla(SCI_LINEDUPLICATE);
}

/** Transform the selection to lower case.
  */
void ScintillaWrapper::LowerCase()
{
	DEBUG_TRACE(L"ScintillaWrapper::LowerCase\n");
	callScintilla(SCI_LOWERCASE);
}

/** Transform the selection to upper case.
  */
void ScintillaWrapper::UpperCase()
{
	DEBUG_TRACE(L"ScintillaWrapper::UpperCase\n");
	callScintilla(SCI_UPPERCASE);
}

/** Scroll the document down, keeping the caret visible.
  */
void ScintillaWrapper::LineScrollDown()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineScrollDown\n");
	callScintilla(SCI_LINESCROLLDOWN);
}

/** Scroll the document up, keeping the caret visible.
  */
void ScintillaWrapper::LineScrollUp()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineScrollUp\n");
	callScintilla(SCI_LINESCROLLUP);
}

/** Delete the selection or if no selection, the character before the caret.
  * Will not delete the character before at the start of a line.
  */
void ScintillaWrapper::DeleteBackNotLine()
{
	DEBUG_TRACE(L"ScintillaWrapper::DeleteBackNotLine\n");
	callScintilla(SCI_DELETEBACKNOTLINE);
}

/** Move caret to first position on display line.
  */
void ScintillaWrapper::HomeDisplay()
{
	DEBUG_TRACE(L"ScintillaWrapper::HomeDisplay\n");
	callScintilla(SCI_HOMEDISPLAY);
}

/** Move caret to first position on display line extending selection to
  * new caret position.
  */
void ScintillaWrapper::HomeDisplayExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::HomeDisplayExtend\n");
	callScintilla(SCI_HOMEDISPLAYEXTEND);
}

/** Move caret to last position on display line.
  */
void ScintillaWrapper::LineEndDisplay()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineEndDisplay\n");
	callScintilla(SCI_LINEENDDISPLAY);
}

/** Move caret to last position on display line extending selection to new
  * caret position.
  */
void ScintillaWrapper::LineEndDisplayExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineEndDisplayExtend\n");
	callScintilla(SCI_LINEENDDISPLAYEXTEND);
}

/** Like Home but when word-wrap is enabled goes first to start of display line
  * HomeDisplay, then to start of document line Home.
  */
void ScintillaWrapper::HomeWrap()
{
	DEBUG_TRACE(L"ScintillaWrapper::HomeWrap\n");
	callScintilla(SCI_HOMEWRAP);
}

/** Like HomeExtend but when word-wrap is enabled extends first to start of display line
  * HomeDisplayExtend, then to start of document line HomeExtend.
  */
void ScintillaWrapper::HomeWrapExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::HomeWrapExtend\n");
	callScintilla(SCI_HOMEWRAPEXTEND);
}

/** Like LineEnd but when word-wrap is enabled goes first to end of display line
  * LineEndDisplay, then to start of document line LineEnd.
  */
void ScintillaWrapper::LineEndWrap()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineEndWrap\n");
	callScintilla(SCI_LINEENDWRAP);
}

/** Like LineEndExtend but when word-wrap is enabled extends first to end of display line
  * LineEndDisplayExtend, then to start of document line LineEndExtend.
  */
void ScintillaWrapper::LineEndWrapExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineEndWrapExtend\n");
	callScintilla(SCI_LINEENDWRAPEXTEND);
}

/** Like VCHome but when word-wrap is enabled goes first to start of display line
  * VCHomeDisplay, then behaves like VCHome.
  */
void ScintillaWrapper::VCHomeWrap()
{
	DEBUG_TRACE(L"ScintillaWrapper::VCHomeWrap\n");
	callScintilla(SCI_VCHOMEWRAP);
}

/** Like VCHomeExtend but when word-wrap is enabled extends first to start of display line
  * VCHomeDisplayExtend, then behaves like VCHomeExtend.
  */
void ScintillaWrapper::VCHomeWrapExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::VCHomeWrapExtend\n");
	callScintilla(SCI_VCHOMEWRAPEXTEND);
}

/** Copy the line containing the caret.
  */
void ScintillaWrapper::LineCopy()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineCopy\n");
	callScintilla(SCI_LINECOPY);
}

/** Move the caret inside current view if it's not there already.
  */
void ScintillaWrapper::MoveCaretInsideView()
{
	DEBUG_TRACE(L"ScintillaWrapper::MoveCaretInsideView\n");
	callScintilla(SCI_MOVECARETINSIDEVIEW);
}

/** How many characters are on a line, including end of line characters?
  */
intptr_t ScintillaWrapper::LineLength(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::LineLength\n");
	return callScintilla(SCI_LINELENGTH, line);
}

/** Highlight the characters at two positions.
  */
void ScintillaWrapper::BraceHighlight(intptr_t posA, intptr_t posB)
{
	DEBUG_TRACE(L"ScintillaWrapper::BraceHighlight\n");
	callScintilla(SCI_BRACEHIGHLIGHT, posA, posB);
}

/** Use specified indicator to highlight matching braces instead of changing their style.
  */
void ScintillaWrapper::BraceHighlightIndicator(bool useSetting, int indicator)
{
	DEBUG_TRACE(L"ScintillaWrapper::BraceHighlightIndicator\n");
	callScintilla(SCI_BRACEHIGHLIGHTINDICATOR, useSetting, indicator);
}

/** Highlight the character at a position indicating there is no matching brace.
  */
void ScintillaWrapper::BraceBadLight(intptr_t pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::BraceBadLight\n");
	callScintilla(SCI_BRACEBADLIGHT, pos);
}

/** Use specified indicator to highlight non matching brace instead of changing its style.
  */
void ScintillaWrapper::BraceBadLightIndicator(bool useSetting, int indicator)
{
	DEBUG_TRACE(L"ScintillaWrapper::BraceBadLightIndicator\n");
	callScintilla(SCI_BRACEBADLIGHTINDICATOR, useSetting, indicator);
}

/** Find the position of a matching brace or INVALID_POSITION if no match.
  * The maxReStyle must be 0 for now. It may be defined in a future release.
  */
intptr_t ScintillaWrapper::BraceMatch(intptr_t pos, int maxReStyle)
{
	DEBUG_TRACE(L"ScintillaWrapper::BraceMatch\n");
	return callScintilla(SCI_BRACEMATCH, pos, maxReStyle);
}

/** Similar to BraceMatch, but matching starts at the explicit start position.
  */
intptr_t ScintillaWrapper::BraceMatchNext(intptr_t pos, intptr_t startPos)
{
	DEBUG_TRACE(L"ScintillaWrapper::BraceMatchNext\n");
	return callScintilla(SCI_BRACEMATCHNEXT, pos, startPos);
}

/** Are the end of line characters visible?
  */
bool ScintillaWrapper::GetViewEOL()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetViewEOL\n");
	return 0 != (callScintilla(SCI_GETVIEWEOL));
}

/** Make the end of line characters visible or invisible.
  */
void ScintillaWrapper::SetViewEOL(bool visible)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetViewEOL\n");
	callScintilla(SCI_SETVIEWEOL, visible);
}

/** Retrieve a pointer to the document object.
  */
intptr_t ScintillaWrapper::GetDocPointer()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetDocPointer\n");
	return callScintilla(SCI_GETDOCPOINTER);
}

/** Change the document object used.
  */
void ScintillaWrapper::SetDocPointer(intptr_t pointer)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetDocPointer\n");
	notAllowedInCallback("setDocPointer is not allowed in a synchronous callback. Use an asynchronous callback, or avoid using SetDocPointer in the callback.");
	callScintilla(SCI_SETDOCPOINTER, 0, pointer);
}

/** Set which document modification events are sent to the container.
  */
void ScintillaWrapper::SetModEventMask(int eventMask)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetModEventMask\n");
	callScintilla(SCI_SETMODEVENTMASK, eventMask);
}

/** Retrieve the column number which text should be kept within.
  */
intptr_t ScintillaWrapper::GetEdgeColumn()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetEdgeColumn\n");
	return callScintilla(SCI_GETEDGECOLUMN);
}

/** Set the column number of the edge.
  * If text goes past the edge then it is highlighted.
  */
void ScintillaWrapper::SetEdgeColumn(intptr_t column)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetEdgeColumn\n");
	callScintilla(SCI_SETEDGECOLUMN, column);
}

/** Retrieve the edge highlight mode.
  */
int ScintillaWrapper::GetEdgeMode()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetEdgeMode\n");
	return callScintilla(SCI_GETEDGEMODE);
}

/** The edge may be displayed by a line (EDGE_LINE/EDGE_MULTILINE) or by highlighting text that
  * goes beyond it (EDGE_BACKGROUND) or not displayed at all (EDGE_NONE).
  */
void ScintillaWrapper::SetEdgeMode(int edgeMode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetEdgeMode\n");
	callScintilla(SCI_SETEDGEMODE, edgeMode);
}

/** Retrieve the colour used in edge indication.
  */
boost::python::tuple ScintillaWrapper::GetEdgeColour()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetEdgeColour\n");
	int retVal = (int)callScintilla(SCI_GETEDGECOLOUR);
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** Change the colour used in edge indication.
  */
void ScintillaWrapper::SetEdgeColour(boost::python::tuple edgeColour)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetEdgeColour\n");
	COLORREF rgbedgeColour = MAKECOLOUR(edgeColour);
	callScintilla(SCI_SETEDGECOLOUR, static_cast<WPARAM>(rgbedgeColour));
}

/** Add a new vertical edge to the view.
  */
void ScintillaWrapper::MultiEdgeAddLine(intptr_t column, boost::python::tuple edgeColour)
{
	DEBUG_TRACE(L"ScintillaWrapper::MultiEdgeAddLine\n");
	COLORREF rgbedgeColour = MAKECOLOUR(edgeColour);
	callScintilla(SCI_MULTIEDGEADDLINE, column, static_cast<LPARAM>(rgbedgeColour));
}

/** Clear all vertical edges.
  */
void ScintillaWrapper::MultiEdgeClearAll()
{
	DEBUG_TRACE(L"ScintillaWrapper::MultiEdgeClearAll\n");
	callScintilla(SCI_MULTIEDGECLEARALL);
}

/** Get multi edge positions.
  */
intptr_t ScintillaWrapper::GetMultiEdgeColumn(int which)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMultiEdgeColumn\n");
	return callScintilla(SCI_GETMULTIEDGECOLUMN, which);
}

/** Sets the current caret position to be the search anchor.
  */
void ScintillaWrapper::SearchAnchor()
{
	DEBUG_TRACE(L"ScintillaWrapper::SearchAnchor\n");
	callScintilla(SCI_SEARCHANCHOR);
}

/** Find some text starting at the search anchor.
  * Does not ensure the selection is visible.
  */
intptr_t ScintillaWrapper::SearchNext(int searchFlags, boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::SearchNext\n");
	std::string stringtext = getStringFromObject(text);
	return callScintilla(SCI_SEARCHNEXT, searchFlags, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Find some text starting at the search anchor and moving backwards.
  * Does not ensure the selection is visible.
  */
intptr_t ScintillaWrapper::SearchPrev(int searchFlags, boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::SearchPrev\n");
	std::string stringtext = getStringFromObject(text);
	return callScintilla(SCI_SEARCHPREV, searchFlags, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Retrieves the number of lines completely visible.
  */
intptr_t ScintillaWrapper::LinesOnScreen()
{
	DEBUG_TRACE(L"ScintillaWrapper::LinesOnScreen\n");
	return callScintilla(SCI_LINESONSCREEN);
}

/** Set whether a pop up menu is displayed automatically when the user presses
  * the wrong mouse button on certain areas.
  */
void ScintillaWrapper::UsePopUp(int popUpMode)
{
	DEBUG_TRACE(L"ScintillaWrapper::UsePopUp\n");
	callScintilla(SCI_USEPOPUP, popUpMode);
}

/** Is the selection rectangular? The alternative is the more common stream selection.
  */
bool ScintillaWrapper::SelectionIsRectangle()
{
	DEBUG_TRACE(L"ScintillaWrapper::SelectionIsRectangle\n");
	return 0 != (callScintilla(SCI_SELECTIONISRECTANGLE));
}

/** Set the zoom level. This number of points is added to the size of all fonts.
  * It may be positive to magnify or negative to reduce.
  */
void ScintillaWrapper::SetZoom(int zoomInPoints)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetZoom\n");
	callScintilla(SCI_SETZOOM, zoomInPoints);
}

/** Retrieve the zoom level.
  */
intptr_t ScintillaWrapper::GetZoom()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetZoom\n");
	return callScintilla(SCI_GETZOOM);
}

/** Create a new document object.
  * Starts with reference count of 1 and not selected into editor.
  */
intptr_t ScintillaWrapper::CreateDocument(intptr_t bytes, int documentOptions)
{
	DEBUG_TRACE(L"ScintillaWrapper::CreateDocument\n");
	return callScintilla(SCI_CREATEDOCUMENT, bytes, documentOptions);
}

/** Extend life of document.
  */
void ScintillaWrapper::AddRefDocument(intptr_t doc)
{
	DEBUG_TRACE(L"ScintillaWrapper::AddRefDocument\n");
	callScintilla(SCI_ADDREFDOCUMENT, 0, doc);
}

/** Release a reference to the document, deleting document if it fades to black.
  */
void ScintillaWrapper::ReleaseDocument(intptr_t doc)
{
	DEBUG_TRACE(L"ScintillaWrapper::ReleaseDocument\n");
	callScintilla(SCI_RELEASEDOCUMENT, 0, doc);
}

/** Get which document options are set.
  */
int ScintillaWrapper::GetDocumentOptions()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetDocumentOptions\n");
	return callScintilla(SCI_GETDOCUMENTOPTIONS);
}

/** Get which document modification events are sent to the container.
  */
int ScintillaWrapper::GetModEventMask()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetModEventMask\n");
	return callScintilla(SCI_GETMODEVENTMASK);
}

/** Set whether command events are sent to the container.
  */
void ScintillaWrapper::SetCommandEvents(bool commandEvents)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCommandEvents\n");
	callScintilla(SCI_SETCOMMANDEVENTS, commandEvents);
}

/** Get whether command events are sent to the container.
  */
bool ScintillaWrapper::GetCommandEvents()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCommandEvents\n");
	return 0 != (callScintilla(SCI_GETCOMMANDEVENTS));
}

/** Change internal focus flag.
  */
void ScintillaWrapper::SetFocus(bool focus)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetFocus\n");
	callScintilla(SCI_SETFOCUS, focus);
}

/** Get internal focus flag.
  */
bool ScintillaWrapper::GetFocus()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetFocus\n");
	return 0 != (callScintilla(SCI_GETFOCUS));
}

/** Change error status - 0 = OK.
  */
void ScintillaWrapper::SetStatus(int status)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetStatus\n");
	callScintilla(SCI_SETSTATUS, status);
}

/** Get error status.
  */
int ScintillaWrapper::GetStatus()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetStatus\n");
	return callScintilla(SCI_GETSTATUS);
}

/** Set whether the mouse is captured when its button is pressed.
  */
void ScintillaWrapper::SetMouseDownCaptures(bool captures)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMouseDownCaptures\n");
	callScintilla(SCI_SETMOUSEDOWNCAPTURES, captures);
}

/** Get whether mouse gets captured.
  */
bool ScintillaWrapper::GetMouseDownCaptures()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMouseDownCaptures\n");
	return 0 != (callScintilla(SCI_GETMOUSEDOWNCAPTURES));
}

/** Set whether the mouse wheel can be active outside the window.
  */
void ScintillaWrapper::SetMouseWheelCaptures(bool captures)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMouseWheelCaptures\n");
	callScintilla(SCI_SETMOUSEWHEELCAPTURES, captures);
}

/** Get whether mouse wheel can be active outside the window.
  */
bool ScintillaWrapper::GetMouseWheelCaptures()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMouseWheelCaptures\n");
	return 0 != (callScintilla(SCI_GETMOUSEWHEELCAPTURES));
}

/** Sets the cursor to one of the SC_CURSOR* values.
  */
void ScintillaWrapper::SetCursor(int cursorType)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCursor\n");
	callScintilla(SCI_SETCURSOR, cursorType);
}

/** Get cursor type.
  */
int ScintillaWrapper::GetCursor()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCursor\n");
	return callScintilla(SCI_GETCURSOR);
}

/** Change the way control characters are displayed:
  * If symbol is < 32, keep the drawn way, else, use the given character.
  */
void ScintillaWrapper::SetControlCharSymbol(int symbol)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetControlCharSymbol\n");
	callScintilla(SCI_SETCONTROLCHARSYMBOL, symbol);
}

/** Get the way control characters are displayed.
  */
intptr_t ScintillaWrapper::GetControlCharSymbol()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetControlCharSymbol\n");
	return callScintilla(SCI_GETCONTROLCHARSYMBOL);
}

/** Move to the previous change in capitalisation.
  */
void ScintillaWrapper::WordPartLeft()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordPartLeft\n");
	callScintilla(SCI_WORDPARTLEFT);
}

/** Move to the previous change in capitalisation extending selection
  * to new caret position.
  */
void ScintillaWrapper::WordPartLeftExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordPartLeftExtend\n");
	callScintilla(SCI_WORDPARTLEFTEXTEND);
}

/** Move to the change next in capitalisation.
  */
void ScintillaWrapper::WordPartRight()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordPartRight\n");
	callScintilla(SCI_WORDPARTRIGHT);
}

/** Move to the next change in capitalisation extending selection
  * to new caret position.
  */
void ScintillaWrapper::WordPartRightExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordPartRightExtend\n");
	callScintilla(SCI_WORDPARTRIGHTEXTEND);
}

/** Set the way the display area is determined when a particular line
  * is to be moved to by Find, FindNext, GotoLine, etc.
  */
void ScintillaWrapper::SetVisiblePolicy(int visiblePolicy, int visibleSlop)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetVisiblePolicy\n");
	callScintilla(SCI_SETVISIBLEPOLICY, visiblePolicy, visibleSlop);
}

/** Delete back from the current position to the start of the line.
  */
void ScintillaWrapper::DelLineLeft()
{
	DEBUG_TRACE(L"ScintillaWrapper::DelLineLeft\n");
	callScintilla(SCI_DELLINELEFT);
}

/** Delete forwards from the current position to the end of the line.
  */
void ScintillaWrapper::DelLineRight()
{
	DEBUG_TRACE(L"ScintillaWrapper::DelLineRight\n");
	callScintilla(SCI_DELLINERIGHT);
}

/** Set the xOffset (ie, horizontal scroll position).
  */
void ScintillaWrapper::SetXOffset(int xOffset)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetXOffset\n");
	callScintilla(SCI_SETXOFFSET, xOffset);
}

/** Get the xOffset (ie, horizontal scroll position).
  */
intptr_t ScintillaWrapper::GetXOffset()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetXOffset\n");
	return callScintilla(SCI_GETXOFFSET);
}

/** Set the last x chosen value to be the caret x position.
  */
void ScintillaWrapper::ChooseCaretX()
{
	DEBUG_TRACE(L"ScintillaWrapper::ChooseCaretX\n");
	callScintilla(SCI_CHOOSECARETX);
}

/** Set the focus to this Scintilla widget.
  */
void ScintillaWrapper::GrabFocus()
{
	DEBUG_TRACE(L"ScintillaWrapper::GrabFocus\n");
	callScintilla(SCI_GRABFOCUS);
}

/** Set the way the caret is kept visible when going sideways.
  * The exclusion zone is given in pixels.
  */
void ScintillaWrapper::SetXCaretPolicy(int caretPolicy, int caretSlop)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetXCaretPolicy\n");
	callScintilla(SCI_SETXCARETPOLICY, caretPolicy, caretSlop);
}

/** Set the way the line the caret is on is kept visible.
  * The exclusion zone is given in lines.
  */
void ScintillaWrapper::SetYCaretPolicy(int caretPolicy, int caretSlop)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetYCaretPolicy\n");
	callScintilla(SCI_SETYCARETPOLICY, caretPolicy, caretSlop);
}

/** Set printing to line wrapped (SC_WRAP_WORD) or not line wrapped (SC_WRAP_NONE).
  */
void ScintillaWrapper::SetPrintWrapMode(int wrapMode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetPrintWrapMode\n");
	callScintilla(SCI_SETPRINTWRAPMODE, wrapMode);
}

/** Is printing line wrapped?
  */
int ScintillaWrapper::GetPrintWrapMode()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetPrintWrapMode\n");
	return callScintilla(SCI_GETPRINTWRAPMODE);
}

/** Set a fore colour for active hotspots.
  */
void ScintillaWrapper::SetHotspotActiveFore(bool useSetting, boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetHotspotActiveFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	callScintilla(SCI_SETHOTSPOTACTIVEFORE, useSetting, static_cast<LPARAM>(rgbfore));
}

/** Get the fore colour for active hotspots.
  */
boost::python::tuple ScintillaWrapper::GetHotspotActiveFore()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetHotspotActiveFore\n");
	int retVal = (int)callScintilla(SCI_GETHOTSPOTACTIVEFORE);
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** Set a back colour for active hotspots.
  */
void ScintillaWrapper::SetHotspotActiveBack(bool useSetting, boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetHotspotActiveBack\n");
	COLORREF rgbback = MAKECOLOUR(back);
	callScintilla(SCI_SETHOTSPOTACTIVEBACK, useSetting, static_cast<LPARAM>(rgbback));
}

/** Get the back colour for active hotspots.
  */
boost::python::tuple ScintillaWrapper::GetHotspotActiveBack()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetHotspotActiveBack\n");
	int retVal = (int)callScintilla(SCI_GETHOTSPOTACTIVEBACK);
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** Enable / Disable underlining active hotspots.
  */
void ScintillaWrapper::SetHotspotActiveUnderline(bool underline)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetHotspotActiveUnderline\n");
	callScintilla(SCI_SETHOTSPOTACTIVEUNDERLINE, underline);
}

/** Get whether underlining for active hotspots.
  */
bool ScintillaWrapper::GetHotspotActiveUnderline()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetHotspotActiveUnderline\n");
	return 0 != (callScintilla(SCI_GETHOTSPOTACTIVEUNDERLINE));
}

/** Limit hotspots to single line so hotspots on two lines don't merge.
  */
void ScintillaWrapper::SetHotspotSingleLine(bool singleLine)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetHotspotSingleLine\n");
	callScintilla(SCI_SETHOTSPOTSINGLELINE, singleLine);
}

/** Get the HotspotSingleLine property
  */
bool ScintillaWrapper::GetHotspotSingleLine()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetHotspotSingleLine\n");
	return 0 != (callScintilla(SCI_GETHOTSPOTSINGLELINE));
}

/** Move caret down one paragraph (delimited by empty lines).
  */
void ScintillaWrapper::ParaDown()
{
	DEBUG_TRACE(L"ScintillaWrapper::ParaDown\n");
	callScintilla(SCI_PARADOWN);
}

/** Extend selection down one paragraph (delimited by empty lines).
  */
void ScintillaWrapper::ParaDownExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::ParaDownExtend\n");
	callScintilla(SCI_PARADOWNEXTEND);
}

/** Move caret up one paragraph (delimited by empty lines).
  */
void ScintillaWrapper::ParaUp()
{
	DEBUG_TRACE(L"ScintillaWrapper::ParaUp\n");
	callScintilla(SCI_PARAUP);
}

/** Extend selection up one paragraph (delimited by empty lines).
  */
void ScintillaWrapper::ParaUpExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::ParaUpExtend\n");
	callScintilla(SCI_PARAUPEXTEND);
}

/** Given a valid document position, return the previous position taking code
  * page into account. Returns 0 if passed 0.
  */
intptr_t ScintillaWrapper::PositionBefore(intptr_t pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::PositionBefore\n");
	return callScintilla(SCI_POSITIONBEFORE, pos);
}

/** Given a valid document position, return the next position taking code
  * page into account. Maximum value returned is the last position in the document.
  */
intptr_t ScintillaWrapper::PositionAfter(intptr_t pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::PositionAfter\n");
	return callScintilla(SCI_POSITIONAFTER, pos);
}

/** Given a valid document position, return a position that differs in a number
  * of characters. Returned value is always between 0 and last position in document.
  */
intptr_t ScintillaWrapper::PositionRelative(intptr_t pos, intptr_t relative)
{
	DEBUG_TRACE(L"ScintillaWrapper::PositionRelative\n");
	return callScintilla(SCI_POSITIONRELATIVE, pos, relative);
}

/** Given a valid document position, return a position that differs in a number
  * of UTF-16 code units. Returned value is always between 0 and last position in document.
  * The result may point half way (2 bytes) inside a non-BMP character.
  */
intptr_t ScintillaWrapper::PositionRelativeCodeUnits(intptr_t pos, intptr_t relative)
{
	DEBUG_TRACE(L"ScintillaWrapper::PositionRelativeCodeUnits\n");
	return callScintilla(SCI_POSITIONRELATIVECODEUNITS, pos, relative);
}

/** Copy a range of text to the clipboard. Positions are clipped into the document.
  */
void ScintillaWrapper::CopyRange(intptr_t start, intptr_t end)
{
	DEBUG_TRACE(L"ScintillaWrapper::CopyRange\n");
	callScintilla(SCI_COPYRANGE, start, end);
}

/** Copy argument text to the clipboard.
  */
intptr_t ScintillaWrapper::CopyText(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::CopyText\n");
	std::string s = getStringFromObject(text);
	return callScintilla(SCI_COPYTEXT, s.size(), reinterpret_cast<LPARAM>(s.c_str()));
}

/** Set the selection mode to stream (SC_SEL_STREAM) or rectangular (SC_SEL_RECTANGLE/SC_SEL_THIN) or
  * by lines (SC_SEL_LINES).
  */
void ScintillaWrapper::SetSelectionMode(int selectionMode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelectionMode\n");
	callScintilla(SCI_SETSELECTIONMODE, selectionMode);
}

/** Get the mode of the current selection.
  */
int ScintillaWrapper::GetSelectionMode()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionMode\n");
	return callScintilla(SCI_GETSELECTIONMODE);
}

/** Get whether or not regular caret moves will extend or reduce the selection.
  */
bool ScintillaWrapper::GetMoveExtendsSelection()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMoveExtendsSelection\n");
	return 0 != (callScintilla(SCI_GETMOVEEXTENDSSELECTION));
}

/** Retrieve the position of the start of the selection at the given line (INVALID_POSITION if no selection on this line).
  */
intptr_t ScintillaWrapper::GetLineSelStartPosition(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineSelStartPosition\n");
	return callScintilla(SCI_GETLINESELSTARTPOSITION, line);
}

/** Retrieve the position of the end of the selection at the given line (INVALID_POSITION if no selection on this line).
  */
intptr_t ScintillaWrapper::GetLineSelEndPosition(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineSelEndPosition\n");
	return callScintilla(SCI_GETLINESELENDPOSITION, line);
}

/** Move caret down one line, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::LineDownRectExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineDownRectExtend\n");
	callScintilla(SCI_LINEDOWNRECTEXTEND);
}

/** Move caret up one line, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::LineUpRectExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineUpRectExtend\n");
	callScintilla(SCI_LINEUPRECTEXTEND);
}

/** Move caret left one character, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::CharLeftRectExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::CharLeftRectExtend\n");
	callScintilla(SCI_CHARLEFTRECTEXTEND);
}

/** Move caret right one character, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::CharRightRectExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::CharRightRectExtend\n");
	callScintilla(SCI_CHARRIGHTRECTEXTEND);
}

/** Move caret to first position on line, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::HomeRectExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::HomeRectExtend\n");
	callScintilla(SCI_HOMERECTEXTEND);
}

/** Move caret to before first visible character on line.
  * If already there move to first character on line.
  * In either case, extend rectangular selection to new caret position.
  */
void ScintillaWrapper::VCHomeRectExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::VCHomeRectExtend\n");
	callScintilla(SCI_VCHOMERECTEXTEND);
}

/** Move caret to last position on line, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::LineEndRectExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineEndRectExtend\n");
	callScintilla(SCI_LINEENDRECTEXTEND);
}

/** Move caret one page up, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::PageUpRectExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::PageUpRectExtend\n");
	callScintilla(SCI_PAGEUPRECTEXTEND);
}

/** Move caret one page down, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::PageDownRectExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::PageDownRectExtend\n");
	callScintilla(SCI_PAGEDOWNRECTEXTEND);
}

/** Move caret to top of page, or one page up if already at top of page.
  */
void ScintillaWrapper::StutteredPageUp()
{
	DEBUG_TRACE(L"ScintillaWrapper::StutteredPageUp\n");
	callScintilla(SCI_STUTTEREDPAGEUP);
}

/** Move caret to top of page, or one page up if already at top of page, extending selection to new caret position.
  */
void ScintillaWrapper::StutteredPageUpExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::StutteredPageUpExtend\n");
	callScintilla(SCI_STUTTEREDPAGEUPEXTEND);
}

/** Move caret to bottom of page, or one page down if already at bottom of page.
  */
void ScintillaWrapper::StutteredPageDown()
{
	DEBUG_TRACE(L"ScintillaWrapper::StutteredPageDown\n");
	callScintilla(SCI_STUTTEREDPAGEDOWN);
}

/** Move caret to bottom of page, or one page down if already at bottom of page, extending selection to new caret position.
  */
void ScintillaWrapper::StutteredPageDownExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::StutteredPageDownExtend\n");
	callScintilla(SCI_STUTTEREDPAGEDOWNEXTEND);
}

/** Move caret left one word, position cursor at end of word.
  */
void ScintillaWrapper::WordLeftEnd()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordLeftEnd\n");
	callScintilla(SCI_WORDLEFTEND);
}

/** Move caret left one word, position cursor at end of word, extending selection to new caret position.
  */
void ScintillaWrapper::WordLeftEndExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordLeftEndExtend\n");
	callScintilla(SCI_WORDLEFTENDEXTEND);
}

/** Move caret right one word, position cursor at end of word.
  */
void ScintillaWrapper::WordRightEnd()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordRightEnd\n");
	callScintilla(SCI_WORDRIGHTEND);
}

/** Move caret right one word, position cursor at end of word, extending selection to new caret position.
  */
void ScintillaWrapper::WordRightEndExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordRightEndExtend\n");
	callScintilla(SCI_WORDRIGHTENDEXTEND);
}

/** Set the set of characters making up whitespace for when moving or selecting by word.
  * Should be called after SetWordChars.
  */
void ScintillaWrapper::SetWhitespaceChars(boost::python::object characters)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWhitespaceChars\n");
	std::string stringcharacters = getStringFromObject(characters);
	callScintilla(SCI_SETWHITESPACECHARS, 0, reinterpret_cast<LPARAM>(stringcharacters.c_str()));
}

/** Get the set of characters making up whitespace for when moving or selecting by word.
  */
boost::python::str ScintillaWrapper::GetWhitespaceChars()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetWhitespaceChars\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETWHITESPACECHARS));
	callScintilla(SCI_GETWHITESPACECHARS, 0, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Set the set of characters making up punctuation characters
  * Should be called after SetWordChars.
  */
void ScintillaWrapper::SetPunctuationChars(boost::python::object characters)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetPunctuationChars\n");
	std::string stringcharacters = getStringFromObject(characters);
	callScintilla(SCI_SETPUNCTUATIONCHARS, 0, reinterpret_cast<LPARAM>(stringcharacters.c_str()));
}

/** Get the set of characters making up punctuation characters
  */
boost::python::str ScintillaWrapper::GetPunctuationChars()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetPunctuationChars\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETPUNCTUATIONCHARS));
	callScintilla(SCI_GETPUNCTUATIONCHARS, 0, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Reset the set of characters for whitespace and word characters to the defaults.
  */
void ScintillaWrapper::SetCharsDefault()
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCharsDefault\n");
	callScintilla(SCI_SETCHARSDEFAULT);
}

/** Get currently selected item position in the auto-completion list
  */
intptr_t ScintillaWrapper::AutoCGetCurrent()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetCurrent\n");
	return callScintilla(SCI_AUTOCGETCURRENT);
}

/** Get currently selected item text in the auto-completion list
  * Returns the length of the item text
  * Result is NUL-terminated.
  */
boost::python::str ScintillaWrapper::AutoCGetCurrentText()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetCurrentText\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_AUTOCGETCURRENTTEXT));
	callScintilla(SCI_AUTOCGETCURRENTTEXT, 0, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Set auto-completion case insensitive behaviour to either prefer case-sensitive matches or have no preference.
  */
void ScintillaWrapper::AutoCSetCaseInsensitiveBehaviour(int behaviour)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetCaseInsensitiveBehaviour\n");
	callScintilla(SCI_AUTOCSETCASEINSENSITIVEBEHAVIOUR, behaviour);
}

/** Get auto-completion case insensitive behaviour.
  */
int ScintillaWrapper::AutoCGetCaseInsensitiveBehaviour()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetCaseInsensitiveBehaviour\n");
	return callScintilla(SCI_AUTOCGETCASEINSENSITIVEBEHAVIOUR);
}

/** Change the effect of autocompleting when there are multiple selections.
  */
void ScintillaWrapper::AutoCSetMulti(int multi)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetMulti\n");
	callScintilla(SCI_AUTOCSETMULTI, multi);
}

/** Retrieve the effect of autocompleting when there are multiple selections.
  */
int ScintillaWrapper::AutoCGetMulti()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetMulti\n");
	return callScintilla(SCI_AUTOCGETMULTI);
}

/** Set the way autocompletion lists are ordered.
  */
void ScintillaWrapper::AutoCSetOrder(int order)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetOrder\n");
	callScintilla(SCI_AUTOCSETORDER, order);
}

/** Get the way autocompletion lists are ordered.
  */
int ScintillaWrapper::AutoCGetOrder()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetOrder\n");
	return callScintilla(SCI_AUTOCGETORDER);
}

/** Enlarge the document to a particular size of text bytes.
  */
void ScintillaWrapper::Allocate(intptr_t bytes)
{
	DEBUG_TRACE(L"ScintillaWrapper::Allocate\n");
	callScintilla(SCI_ALLOCATE, bytes);
}

/** Returns the target converted to UTF8.
  * Return the length in bytes.
  */
boost::python::str ScintillaWrapper::TargetAsUTF8()
{
	DEBUG_TRACE(L"ScintillaWrapper::TargetAsUTF8\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_TARGETASUTF8));
	callScintilla(SCI_TARGETASUTF8, 0, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Set the length of the utf8 argument for calling EncodedFromUTF8.
  * Set to -1 and the string will be measured to the first nul.
  */
void ScintillaWrapper::SetLengthForEncode(intptr_t bytes)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetLengthForEncode\n");
	callScintilla(SCI_SETLENGTHFORENCODE, bytes);
}

/** Translates a UTF8 string into the document encoding.
  * Return the length of the result in bytes.
  * On error return 0.
  */
boost::python::str ScintillaWrapper::EncodedFromUTF8(boost::python::object utf8)
{
	DEBUG_TRACE(L"ScintillaWrapper::EncodedFromUTF8\n");
	std::string utf8String = getStringFromObject(utf8);
	PythonCompatibleStrBuffer result(callScintilla(SCI_ENCODEDFROMUTF8, reinterpret_cast<WPARAM>(utf8String.c_str()), 0));
	callScintilla(SCI_ENCODEDFROMUTF8, reinterpret_cast<WPARAM>(utf8String.c_str()), reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Find the position of a column on a line taking into account tabs and
  * multi-byte characters. If beyond end of line, return line end position.
  */
intptr_t ScintillaWrapper::FindColumn(intptr_t line, intptr_t column)
{
	DEBUG_TRACE(L"ScintillaWrapper::FindColumn\n");
	return callScintilla(SCI_FINDCOLUMN, line, column);
}

/** Can the caret preferred x position only be changed by explicit movement commands?
  */
int ScintillaWrapper::GetCaretSticky()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCaretSticky\n");
	return callScintilla(SCI_GETCARETSTICKY);
}

/** Stop the caret preferred x position changing when the user types.
  */
void ScintillaWrapper::SetCaretSticky(int useCaretStickyBehaviour)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCaretSticky\n");
	callScintilla(SCI_SETCARETSTICKY, useCaretStickyBehaviour);
}

/** Switch between sticky and non-sticky: meant to be bound to a key.
  */
void ScintillaWrapper::ToggleCaretSticky()
{
	DEBUG_TRACE(L"ScintillaWrapper::ToggleCaretSticky\n");
	callScintilla(SCI_TOGGLECARETSTICKY);
}

/** Enable/Disable convert-on-paste for line endings
  */
void ScintillaWrapper::SetPasteConvertEndings(bool convert)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetPasteConvertEndings\n");
	callScintilla(SCI_SETPASTECONVERTENDINGS, convert);
}

/** Get convert-on-paste setting
  */
bool ScintillaWrapper::GetPasteConvertEndings()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetPasteConvertEndings\n");
	return 0 != (callScintilla(SCI_GETPASTECONVERTENDINGS));
}

/** Duplicate the selection. If selection empty duplicate the line containing the caret.
  */
void ScintillaWrapper::SelectionDuplicate()
{
	DEBUG_TRACE(L"ScintillaWrapper::SelectionDuplicate\n");
	callScintilla(SCI_SELECTIONDUPLICATE);
}

/** Set background alpha of the caret line.
  */
void ScintillaWrapper::SetCaretLineBackAlpha(int alpha)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCaretLineBackAlpha\n");
	callScintilla(SCI_SETCARETLINEBACKALPHA, alpha);
}

/** Get the background alpha of the caret line.
  */
int ScintillaWrapper::GetCaretLineBackAlpha()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCaretLineBackAlpha\n");
	return callScintilla(SCI_GETCARETLINEBACKALPHA);
}

/** Set the style of the caret to be drawn.
  */
void ScintillaWrapper::SetCaretStyle(int caretStyle)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCaretStyle\n");
	callScintilla(SCI_SETCARETSTYLE, caretStyle);
}

/** Returns the current style of the caret.
  */
int ScintillaWrapper::GetCaretStyle()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCaretStyle\n");
	return callScintilla(SCI_GETCARETSTYLE);
}

/** Set the indicator used for IndicatorFillRange and IndicatorClearRange
  */
void ScintillaWrapper::SetIndicatorCurrent(int indicator)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetIndicatorCurrent\n");
	callScintilla(SCI_SETINDICATORCURRENT, indicator);
}

/** Get the current indicator
  */
intptr_t ScintillaWrapper::GetIndicatorCurrent()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetIndicatorCurrent\n");
	return callScintilla(SCI_GETINDICATORCURRENT);
}

/** Set the value used for IndicatorFillRange
  */
void ScintillaWrapper::SetIndicatorValue(int value)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetIndicatorValue\n");
	callScintilla(SCI_SETINDICATORVALUE, value);
}

/** Get the current indicator value
  */
intptr_t ScintillaWrapper::GetIndicatorValue()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetIndicatorValue\n");
	return callScintilla(SCI_GETINDICATORVALUE);
}

/** Turn a indicator on over a range.
  */
void ScintillaWrapper::IndicatorFillRange(intptr_t start, intptr_t lengthFill)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicatorFillRange\n");
	callScintilla(SCI_INDICATORFILLRANGE, start, lengthFill);
}

/** Turn a indicator off over a range.
  */
void ScintillaWrapper::IndicatorClearRange(intptr_t start, intptr_t lengthClear)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicatorClearRange\n");
	callScintilla(SCI_INDICATORCLEARRANGE, start, lengthClear);
}

/** Are any indicators present at pos?
  */
intptr_t ScintillaWrapper::IndicatorAllOnFor(intptr_t pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicatorAllOnFor\n");
	return callScintilla(SCI_INDICATORALLONFOR, pos);
}

/** What value does a particular indicator have at a position?
  */
intptr_t ScintillaWrapper::IndicatorValueAt(int indicator, intptr_t pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicatorValueAt\n");
	return callScintilla(SCI_INDICATORVALUEAT, indicator, pos);
}

/** Where does a particular indicator start?
  */
intptr_t ScintillaWrapper::IndicatorStart(int indicator, intptr_t pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicatorStart\n");
	return callScintilla(SCI_INDICATORSTART, indicator, pos);
}

/** Where does a particular indicator end?
  */
intptr_t ScintillaWrapper::IndicatorEnd(int indicator, intptr_t pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicatorEnd\n");
	return callScintilla(SCI_INDICATOREND, indicator, pos);
}

/** Set number of entries in position cache
  */
void ScintillaWrapper::SetPositionCache(int size)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetPositionCache\n");
	callScintilla(SCI_SETPOSITIONCACHE, size);
}

/** How many entries are allocated to the position cache?
  */
intptr_t ScintillaWrapper::GetPositionCache()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetPositionCache\n");
	return callScintilla(SCI_GETPOSITIONCACHE);
}

/** Copy the selection, if selection empty copy the line with the caret
  */
void ScintillaWrapper::CopyAllowLine()
{
	DEBUG_TRACE(L"ScintillaWrapper::CopyAllowLine\n");
	callScintilla(SCI_COPYALLOWLINE);
}

/** Compact the document buffer and return a read-only pointer to the
  * characters in the document.
  */
boost::python::str ScintillaWrapper::GetCharacterPointer()
{
	GILRelease release;
	const char *charPtr = reinterpret_cast<const char*>(callScintilla(SCI_GETCHARACTERPOINTER));
	release.reacquire();
	return boost::python::str(charPtr);
}

/** Return a read-only pointer to a range of characters in the document.
  * May move the gap so that the range is contiguous, but will only move up
  * to lengthRange bytes.
  */
boost::python::str ScintillaWrapper::GetRangePointer(int position, int rangeLength)
{
	GILRelease release;
	const char *charPtr = reinterpret_cast<const char*>(callScintilla(SCI_GETRANGEPOINTER, position, rangeLength));
	release.reacquire();
	return boost::python::str(charPtr, charPtr + rangeLength);
}

/** Return a position which, to avoid performance costs, should not be within
  * the range of a call to GetRangePointer.
  */
intptr_t ScintillaWrapper::GetGapPosition()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetGapPosition\n");
	return callScintilla(SCI_GETGAPPOSITION);
}

/** Set the alpha fill colour of the given indicator.
  */
void ScintillaWrapper::IndicSetAlpha(int indicator, int alpha)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicSetAlpha\n");
	callScintilla(SCI_INDICSETALPHA, indicator, alpha);
}

/** Get the alpha fill colour of the given indicator.
  */
int ScintillaWrapper::IndicGetAlpha(int indicator)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicGetAlpha\n");
	return callScintilla(SCI_INDICGETALPHA, indicator);
}

/** Set the alpha outline colour of the given indicator.
  */
void ScintillaWrapper::IndicSetOutlineAlpha(int indicator, int alpha)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicSetOutlineAlpha\n");
	callScintilla(SCI_INDICSETOUTLINEALPHA, indicator, alpha);
}

/** Get the alpha outline colour of the given indicator.
  */
int ScintillaWrapper::IndicGetOutlineAlpha(int indicator)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicGetOutlineAlpha\n");
	return callScintilla(SCI_INDICGETOUTLINEALPHA, indicator);
}

/** Set extra ascent for each line
  */
void ScintillaWrapper::SetExtraAscent(int extraAscent)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetExtraAscent\n");
	callScintilla(SCI_SETEXTRAASCENT, extraAscent);
}

/** Get extra ascent for each line
  */
intptr_t ScintillaWrapper::GetExtraAscent()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetExtraAscent\n");
	return callScintilla(SCI_GETEXTRAASCENT);
}

/** Set extra descent for each line
  */
void ScintillaWrapper::SetExtraDescent(int extraDescent)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetExtraDescent\n");
	callScintilla(SCI_SETEXTRADESCENT, extraDescent);
}

/** Get extra descent for each line
  */
intptr_t ScintillaWrapper::GetExtraDescent()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetExtraDescent\n");
	return callScintilla(SCI_GETEXTRADESCENT);
}

/** Which symbol was defined for markerNumber with MarkerDefine
  */
intptr_t ScintillaWrapper::MarkerSymbolDefined(int markerNumber)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerSymbolDefined\n");
	return callScintilla(SCI_MARKERSYMBOLDEFINED, markerNumber);
}

/** Set the text in the text margin for a line
  */
void ScintillaWrapper::MarginSetText(intptr_t line, boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarginSetText\n");
	std::string stringtext = getStringFromObject(text);
	callScintilla(SCI_MARGINSETTEXT, line, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Get the text in the text margin for a line
  */
boost::python::str ScintillaWrapper::MarginGetText(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarginGetText\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_MARGINGETTEXT, line));
	callScintilla(SCI_MARGINGETTEXT, line, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Set the style number for the text margin for a line
  */
void ScintillaWrapper::MarginSetStyle(intptr_t line, int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarginSetStyle\n");
	callScintilla(SCI_MARGINSETSTYLE, line, style);
}

/** Get the style number for the text margin for a line
  */
intptr_t ScintillaWrapper::MarginGetStyle(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarginGetStyle\n");
	return callScintilla(SCI_MARGINGETSTYLE, line);
}

/** Set the style in the text margin for a line
  */
void ScintillaWrapper::MarginSetStyles(intptr_t line, boost::python::object styles)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarginSetStyles\n");
	std::string stringstyles = getStringFromObject(styles);
	callScintilla(SCI_MARGINSETSTYLES, line, reinterpret_cast<LPARAM>(stringstyles.c_str()));
}

/** Get the styles in the text margin for a line
  */
boost::python::str ScintillaWrapper::MarginGetStyles(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarginGetStyles\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_MARGINGETSTYLES, line));
	callScintilla(SCI_MARGINGETSTYLES, line, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Clear the margin text on all lines
  */
void ScintillaWrapper::MarginTextClearAll()
{
	DEBUG_TRACE(L"ScintillaWrapper::MarginTextClearAll\n");
	callScintilla(SCI_MARGINTEXTCLEARALL);
}

/** Get the start of the range of style numbers used for margin text
  */
void ScintillaWrapper::MarginSetStyleOffset(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarginSetStyleOffset\n");
	callScintilla(SCI_MARGINSETSTYLEOFFSET, style);
}

/** Get the start of the range of style numbers used for margin text
  */
intptr_t ScintillaWrapper::MarginGetStyleOffset()
{
	DEBUG_TRACE(L"ScintillaWrapper::MarginGetStyleOffset\n");
	return callScintilla(SCI_MARGINGETSTYLEOFFSET);
}

/** Set the margin options.
  */
void ScintillaWrapper::SetMarginOptions(int marginOptions)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMarginOptions\n");
	callScintilla(SCI_SETMARGINOPTIONS, marginOptions);
}

/** Get the margin options.
  */
int ScintillaWrapper::GetMarginOptions()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMarginOptions\n");
	return callScintilla(SCI_GETMARGINOPTIONS);
}

/** Set the annotation text for a line
  */
void ScintillaWrapper::AnnotationSetText(int line, boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationSetText\n");
	const char *newText;
	std::string s;
	if (text.is_none())
	{
		newText = NULL;
	}
	else
	{
		s = getStringFromObject(text);
		newText = s.c_str();
	}
	callScintilla(SCI_ANNOTATIONSETTEXT, static_cast<WPARAM>(line), reinterpret_cast<LPARAM>(newText));
}

/** Get the annotation text for a line
  */
boost::python::str ScintillaWrapper::AnnotationGetText(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationGetText\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_ANNOTATIONGETTEXT, line));
	callScintilla(SCI_ANNOTATIONGETTEXT, line, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Set the style number for the annotations for a line
  */
void ScintillaWrapper::AnnotationSetStyle(intptr_t line, int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationSetStyle\n");
	callScintilla(SCI_ANNOTATIONSETSTYLE, line, style);
}

/** Get the style number for the annotations for a line
  */
intptr_t ScintillaWrapper::AnnotationGetStyle(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationGetStyle\n");
	return callScintilla(SCI_ANNOTATIONGETSTYLE, line);
}

/** Set the annotation styles for a line
  */
void ScintillaWrapper::AnnotationSetStyles(intptr_t line, boost::python::object styles)
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationSetStyles\n");
	std::string stringstyles = getStringFromObject(styles);
	callScintilla(SCI_ANNOTATIONSETSTYLES, line, reinterpret_cast<LPARAM>(stringstyles.c_str()));
}

/** Get the annotation styles for a line
  */
boost::python::str ScintillaWrapper::AnnotationGetStyles(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationGetStyles\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_ANNOTATIONGETSTYLES, line));
	callScintilla(SCI_ANNOTATIONGETSTYLES, line, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Get the number of annotation lines for a line
  */
intptr_t ScintillaWrapper::AnnotationGetLines(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationGetLines\n");
	return callScintilla(SCI_ANNOTATIONGETLINES, line);
}

/** Clear the annotations from all lines
  */
void ScintillaWrapper::AnnotationClearAll()
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationClearAll\n");
	callScintilla(SCI_ANNOTATIONCLEARALL);
}

/** Set the visibility for the annotations for a view
  */
void ScintillaWrapper::AnnotationSetVisible(int visible)
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationSetVisible\n");
	callScintilla(SCI_ANNOTATIONSETVISIBLE, visible);
}

/** Get the visibility for the annotations for a view
  */
int ScintillaWrapper::AnnotationGetVisible()
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationGetVisible\n");
	return callScintilla(SCI_ANNOTATIONGETVISIBLE);
}

/** Get the start of the range of style numbers used for annotations
  */
void ScintillaWrapper::AnnotationSetStyleOffset(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationSetStyleOffset\n");
	callScintilla(SCI_ANNOTATIONSETSTYLEOFFSET, style);
}

/** Get the start of the range of style numbers used for annotations
  */
intptr_t ScintillaWrapper::AnnotationGetStyleOffset()
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationGetStyleOffset\n");
	return callScintilla(SCI_ANNOTATIONGETSTYLEOFFSET);
}

/** Release all extended (>255) style numbers
  */
void ScintillaWrapper::ReleaseAllExtendedStyles()
{
	DEBUG_TRACE(L"ScintillaWrapper::ReleaseAllExtendedStyles\n");
	callScintilla(SCI_RELEASEALLEXTENDEDSTYLES);
}

/** Allocate some extended (>255) style numbers and return the start of the range
  */
intptr_t ScintillaWrapper::AllocateExtendedStyles(int numberStyles)
{
	DEBUG_TRACE(L"ScintillaWrapper::AllocateExtendedStyles\n");
	return callScintilla(SCI_ALLOCATEEXTENDEDSTYLES, numberStyles);
}

/** Add a container action to the undo stack
  */
void ScintillaWrapper::AddUndoAction(int token, int flags)
{
	DEBUG_TRACE(L"ScintillaWrapper::AddUndoAction\n");
	callScintilla(SCI_ADDUNDOACTION, token, flags);
}

/** Find the position of a character from a point within the window.
  */
intptr_t ScintillaWrapper::CharPositionFromPoint(int x, int y)
{
	DEBUG_TRACE(L"ScintillaWrapper::CharPositionFromPoint\n");
	return callScintilla(SCI_CHARPOSITIONFROMPOINT, x, y);
}

/** Find the position of a character from a point within the window.
  * Return INVALID_POSITION if not close to text.
  */
intptr_t ScintillaWrapper::CharPositionFromPointClose(int x, int y)
{
	DEBUG_TRACE(L"ScintillaWrapper::CharPositionFromPointClose\n");
	return callScintilla(SCI_CHARPOSITIONFROMPOINTCLOSE, x, y);
}

/** Set whether switching to rectangular mode while selecting with the mouse is allowed.
  */
void ScintillaWrapper::SetMouseSelectionRectangularSwitch(bool mouseSelectionRectangularSwitch)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMouseSelectionRectangularSwitch\n");
	callScintilla(SCI_SETMOUSESELECTIONRECTANGULARSWITCH, mouseSelectionRectangularSwitch);
}

/** Whether switching to rectangular mode while selecting with the mouse is allowed.
  */
bool ScintillaWrapper::GetMouseSelectionRectangularSwitch()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMouseSelectionRectangularSwitch\n");
	return 0 != (callScintilla(SCI_GETMOUSESELECTIONRECTANGULARSWITCH));
}

/** Set whether multiple selections can be made
  */
void ScintillaWrapper::SetMultipleSelection(bool multipleSelection)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMultipleSelection\n");
	callScintilla(SCI_SETMULTIPLESELECTION, multipleSelection);
}

/** Whether multiple selections can be made
  */
bool ScintillaWrapper::GetMultipleSelection()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMultipleSelection\n");
	return 0 != (callScintilla(SCI_GETMULTIPLESELECTION));
}

/** Set whether typing can be performed into multiple selections
  */
void ScintillaWrapper::SetAdditionalSelectionTyping(bool additionalSelectionTyping)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetAdditionalSelectionTyping\n");
	callScintilla(SCI_SETADDITIONALSELECTIONTYPING, additionalSelectionTyping);
}

/** Whether typing can be performed into multiple selections
  */
bool ScintillaWrapper::GetAdditionalSelectionTyping()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetAdditionalSelectionTyping\n");
	return 0 != (callScintilla(SCI_GETADDITIONALSELECTIONTYPING));
}

/** Set whether additional carets will blink
  */
void ScintillaWrapper::SetAdditionalCaretsBlink(bool additionalCaretsBlink)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetAdditionalCaretsBlink\n");
	callScintilla(SCI_SETADDITIONALCARETSBLINK, additionalCaretsBlink);
}

/** Whether additional carets will blink
  */
bool ScintillaWrapper::GetAdditionalCaretsBlink()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetAdditionalCaretsBlink\n");
	return 0 != (callScintilla(SCI_GETADDITIONALCARETSBLINK));
}

/** Set whether additional carets are visible
  */
void ScintillaWrapper::SetAdditionalCaretsVisible(bool additionalCaretsVisible)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetAdditionalCaretsVisible\n");
	callScintilla(SCI_SETADDITIONALCARETSVISIBLE, additionalCaretsVisible);
}

/** Whether additional carets are visible
  */
bool ScintillaWrapper::GetAdditionalCaretsVisible()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetAdditionalCaretsVisible\n");
	return 0 != (callScintilla(SCI_GETADDITIONALCARETSVISIBLE));
}

/** How many selections are there?
  */
intptr_t ScintillaWrapper::GetSelections()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelections\n");
	return callScintilla(SCI_GETSELECTIONS);
}

/** Is every selected range empty?
  */
bool ScintillaWrapper::GetSelectionEmpty()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionEmpty\n");
	return 0 != (callScintilla(SCI_GETSELECTIONEMPTY));
}

/** Clear selections to a single empty stream selection
  */
void ScintillaWrapper::ClearSelections()
{
	DEBUG_TRACE(L"ScintillaWrapper::ClearSelections\n");
	callScintilla(SCI_CLEARSELECTIONS);
}

/** Set a simple selection
  */
void ScintillaWrapper::SetSelection(intptr_t caret, intptr_t anchor)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelection\n");
	callScintilla(SCI_SETSELECTION, caret, anchor);
}

/** Add a selection
  */
void ScintillaWrapper::AddSelection(intptr_t caret, intptr_t anchor)
{
	DEBUG_TRACE(L"ScintillaWrapper::AddSelection\n");
	callScintilla(SCI_ADDSELECTION, caret, anchor);
}

/** Drop one selection
  */
void ScintillaWrapper::DropSelectionN(int selection)
{
	DEBUG_TRACE(L"ScintillaWrapper::DropSelectionN\n");
	callScintilla(SCI_DROPSELECTIONN, selection);
}

/** Set the main selection
  */
void ScintillaWrapper::SetMainSelection(int selection)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMainSelection\n");
	callScintilla(SCI_SETMAINSELECTION, selection);
}

/** Which selection is the main selection
  */
intptr_t ScintillaWrapper::GetMainSelection()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMainSelection\n");
	return callScintilla(SCI_GETMAINSELECTION);
}

/** Set the caret position of the nth selection.
  */
void ScintillaWrapper::SetSelectionNCaret(int selection, intptr_t caret)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelectionNCaret\n");
	callScintilla(SCI_SETSELECTIONNCARET, selection, caret);
}

/** Return the caret position of the nth selection.
  */
intptr_t ScintillaWrapper::GetSelectionNCaret(int selection)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionNCaret\n");
	return callScintilla(SCI_GETSELECTIONNCARET, selection);
}

/** Set the anchor position of the nth selection.
  */
void ScintillaWrapper::SetSelectionNAnchor(int selection, intptr_t anchor)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelectionNAnchor\n");
	callScintilla(SCI_SETSELECTIONNANCHOR, selection, anchor);
}

/** Return the anchor position of the nth selection.
  */
intptr_t ScintillaWrapper::GetSelectionNAnchor(int selection)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionNAnchor\n");
	return callScintilla(SCI_GETSELECTIONNANCHOR, selection);
}

/** Set the virtual space of the caret of the nth selection.
  */
void ScintillaWrapper::SetSelectionNCaretVirtualSpace(int selection, intptr_t space)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelectionNCaretVirtualSpace\n");
	callScintilla(SCI_SETSELECTIONNCARETVIRTUALSPACE, selection, space);
}

/** Return the virtual space of the caret of the nth selection.
  */
intptr_t ScintillaWrapper::GetSelectionNCaretVirtualSpace(int selection)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionNCaretVirtualSpace\n");
	return callScintilla(SCI_GETSELECTIONNCARETVIRTUALSPACE, selection);
}

/** Set the virtual space of the anchor of the nth selection.
  */
void ScintillaWrapper::SetSelectionNAnchorVirtualSpace(int selection, intptr_t space)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelectionNAnchorVirtualSpace\n");
	callScintilla(SCI_SETSELECTIONNANCHORVIRTUALSPACE, selection, space);
}

/** Return the virtual space of the anchor of the nth selection.
  */
intptr_t ScintillaWrapper::GetSelectionNAnchorVirtualSpace(int selection)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionNAnchorVirtualSpace\n");
	return callScintilla(SCI_GETSELECTIONNANCHORVIRTUALSPACE, selection);
}

/** Sets the position that starts the selection - this becomes the anchor.
  */
void ScintillaWrapper::SetSelectionNStart(int selection, intptr_t anchor)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelectionNStart\n");
	callScintilla(SCI_SETSELECTIONNSTART, selection, anchor);
}

/** Returns the position at the start of the selection.
  */
intptr_t ScintillaWrapper::GetSelectionNStart(int selection)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionNStart\n");
	return callScintilla(SCI_GETSELECTIONNSTART, selection);
}

/** Returns the virtual space at the start of the selection.
  */
intptr_t ScintillaWrapper::GetSelectionNStartVirtualSpace(int selection)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionNStartVirtualSpace\n");
	return callScintilla(SCI_GETSELECTIONNSTARTVIRTUALSPACE, selection);
}

/** Sets the position that ends the selection - this becomes the currentPosition.
  */
void ScintillaWrapper::SetSelectionNEnd(int selection, intptr_t caret)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelectionNEnd\n");
	callScintilla(SCI_SETSELECTIONNEND, selection, caret);
}

/** Returns the virtual space at the end of the selection.
  */
intptr_t ScintillaWrapper::GetSelectionNEndVirtualSpace(int selection)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionNEndVirtualSpace\n");
	return callScintilla(SCI_GETSELECTIONNENDVIRTUALSPACE, selection);
}

/** Returns the position at the end of the selection.
  */
intptr_t ScintillaWrapper::GetSelectionNEnd(int selection)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionNEnd\n");
	return callScintilla(SCI_GETSELECTIONNEND, selection);
}

/** Set the caret position of the rectangular selection.
  */
void ScintillaWrapper::SetRectangularSelectionCaret(intptr_t caret)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetRectangularSelectionCaret\n");
	callScintilla(SCI_SETRECTANGULARSELECTIONCARET, caret);
}

/** Return the caret position of the rectangular selection.
  */
intptr_t ScintillaWrapper::GetRectangularSelectionCaret()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetRectangularSelectionCaret\n");
	return callScintilla(SCI_GETRECTANGULARSELECTIONCARET);
}

/** Set the anchor position of the rectangular selection.
  */
void ScintillaWrapper::SetRectangularSelectionAnchor(intptr_t anchor)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetRectangularSelectionAnchor\n");
	callScintilla(SCI_SETRECTANGULARSELECTIONANCHOR, anchor);
}

/** Return the anchor position of the rectangular selection.
  */
intptr_t ScintillaWrapper::GetRectangularSelectionAnchor()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetRectangularSelectionAnchor\n");
	return callScintilla(SCI_GETRECTANGULARSELECTIONANCHOR);
}

/** Set the virtual space of the caret of the rectangular selection.
  */
void ScintillaWrapper::SetRectangularSelectionCaretVirtualSpace(intptr_t space)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetRectangularSelectionCaretVirtualSpace\n");
	callScintilla(SCI_SETRECTANGULARSELECTIONCARETVIRTUALSPACE, space);
}

/** Return the virtual space of the caret of the rectangular selection.
  */
intptr_t ScintillaWrapper::GetRectangularSelectionCaretVirtualSpace()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetRectangularSelectionCaretVirtualSpace\n");
	return callScintilla(SCI_GETRECTANGULARSELECTIONCARETVIRTUALSPACE);
}

/** Set the virtual space of the anchor of the rectangular selection.
  */
void ScintillaWrapper::SetRectangularSelectionAnchorVirtualSpace(intptr_t space)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetRectangularSelectionAnchorVirtualSpace\n");
	callScintilla(SCI_SETRECTANGULARSELECTIONANCHORVIRTUALSPACE, space);
}

/** Return the virtual space of the anchor of the rectangular selection.
  */
intptr_t ScintillaWrapper::GetRectangularSelectionAnchorVirtualSpace()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetRectangularSelectionAnchorVirtualSpace\n");
	return callScintilla(SCI_GETRECTANGULARSELECTIONANCHORVIRTUALSPACE);
}

/** Set options for virtual space behaviour.
  */
void ScintillaWrapper::SetVirtualSpaceOptions(int virtualSpaceOptions)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetVirtualSpaceOptions\n");
	callScintilla(SCI_SETVIRTUALSPACEOPTIONS, virtualSpaceOptions);
}

/** Return options for virtual space behaviour.
  */
int ScintillaWrapper::GetVirtualSpaceOptions()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetVirtualSpaceOptions\n");
	return callScintilla(SCI_GETVIRTUALSPACEOPTIONS);
}

/** On GTK, allow selecting the modifier key to use for mouse-based
  * rectangular selection. Often the window manager requires Alt+Mouse Drag
  * for moving windows.
  * Valid values are SCMOD_CTRL(default), SCMOD_ALT, or SCMOD_SUPER.
  */
void ScintillaWrapper::SetRectangularSelectionModifier(int modifier)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetRectangularSelectionModifier\n");
	callScintilla(SCI_SETRECTANGULARSELECTIONMODIFIER, modifier);
}

/** Get the modifier key used for rectangular selection.
  */
intptr_t ScintillaWrapper::GetRectangularSelectionModifier()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetRectangularSelectionModifier\n");
	return callScintilla(SCI_GETRECTANGULARSELECTIONMODIFIER);
}

/** Set the foreground colour of additional selections.
  * Must have previously called SetSelFore with non-zero first argument for this to have an effect.
  */
void ScintillaWrapper::SetAdditionalSelFore(boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetAdditionalSelFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	callScintilla(SCI_SETADDITIONALSELFORE, static_cast<WPARAM>(rgbfore));
}

/** Set the background colour of additional selections.
  * Must have previously called SetSelBack with non-zero first argument for this to have an effect.
  */
void ScintillaWrapper::SetAdditionalSelBack(boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetAdditionalSelBack\n");
	COLORREF rgbback = MAKECOLOUR(back);
	callScintilla(SCI_SETADDITIONALSELBACK, static_cast<WPARAM>(rgbback));
}

/** Set the alpha of the selection.
  */
void ScintillaWrapper::SetAdditionalSelAlpha(int alpha)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetAdditionalSelAlpha\n");
	callScintilla(SCI_SETADDITIONALSELALPHA, alpha);
}

/** Get the alpha of the selection.
  */
int ScintillaWrapper::GetAdditionalSelAlpha()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetAdditionalSelAlpha\n");
	return callScintilla(SCI_GETADDITIONALSELALPHA);
}

/** Set the foreground colour of additional carets.
  */
void ScintillaWrapper::SetAdditionalCaretFore(boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetAdditionalCaretFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	callScintilla(SCI_SETADDITIONALCARETFORE, static_cast<WPARAM>(rgbfore));
}

/** Get the foreground colour of additional carets.
  */
boost::python::tuple ScintillaWrapper::GetAdditionalCaretFore()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetAdditionalCaretFore\n");
	int retVal = (int)callScintilla(SCI_GETADDITIONALCARETFORE);
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** Set the main selection to the next selection.
  */
void ScintillaWrapper::RotateSelection()
{
	DEBUG_TRACE(L"ScintillaWrapper::RotateSelection\n");
	callScintilla(SCI_ROTATESELECTION);
}

/** Swap that caret and anchor of the main selection.
  */
void ScintillaWrapper::SwapMainAnchorCaret()
{
	DEBUG_TRACE(L"ScintillaWrapper::SwapMainAnchorCaret\n");
	callScintilla(SCI_SWAPMAINANCHORCARET);
}

/** Add the next occurrence of the main selection to the set of selections as main.
  * If the current selection is empty then select word around caret.
  */
void ScintillaWrapper::MultipleSelectAddNext()
{
	DEBUG_TRACE(L"ScintillaWrapper::MultipleSelectAddNext\n");
	callScintilla(SCI_MULTIPLESELECTADDNEXT);
}

/** Add each occurrence of the main selection in the target to the set of selections.
  * If the current selection is empty then select word around caret.
  */
void ScintillaWrapper::MultipleSelectAddEach()
{
	DEBUG_TRACE(L"ScintillaWrapper::MultipleSelectAddEach\n");
	callScintilla(SCI_MULTIPLESELECTADDEACH);
}

/** Indicate that the internal state of a lexer has changed over a range and therefore
  * there may be a need to redraw.
  */
intptr_t ScintillaWrapper::ChangeLexerState(intptr_t start, intptr_t end)
{
	DEBUG_TRACE(L"ScintillaWrapper::ChangeLexerState\n");
	return callScintilla(SCI_CHANGELEXERSTATE, start, end);
}

/** Find the next line at or after lineStart that is a contracted fold header line.
  * Return -1 when no more lines.
  */
intptr_t ScintillaWrapper::ContractedFoldNext(intptr_t lineStart)
{
	DEBUG_TRACE(L"ScintillaWrapper::ContractedFoldNext\n");
	return callScintilla(SCI_CONTRACTEDFOLDNEXT, lineStart);
}

/** Centre current line in window.
  */
void ScintillaWrapper::VerticalCentreCaret()
{
	DEBUG_TRACE(L"ScintillaWrapper::VerticalCentreCaret\n");
	callScintilla(SCI_VERTICALCENTRECARET);
}

/** Move the selected lines up one line, shifting the line above after the selection
  */
void ScintillaWrapper::MoveSelectedLinesUp()
{
	DEBUG_TRACE(L"ScintillaWrapper::MoveSelectedLinesUp\n");
	callScintilla(SCI_MOVESELECTEDLINESUP);
}

/** Move the selected lines down one line, shifting the line below before the selection
  */
void ScintillaWrapper::MoveSelectedLinesDown()
{
	DEBUG_TRACE(L"ScintillaWrapper::MoveSelectedLinesDown\n");
	callScintilla(SCI_MOVESELECTEDLINESDOWN);
}

/** Set the identifier reported as idFrom in notification messages.
  */
void ScintillaWrapper::SetIdentifier(int identifier)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetIdentifier\n");
	callScintilla(SCI_SETIDENTIFIER, identifier);
}

/** Get the identifier.
  */
intptr_t ScintillaWrapper::GetIdentifier()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetIdentifier\n");
	return callScintilla(SCI_GETIDENTIFIER);
}

/** Set the width for future RGBA image data.
  */
void ScintillaWrapper::RGBAImageSetWidth(int width)
{
	DEBUG_TRACE(L"ScintillaWrapper::RGBAImageSetWidth\n");
	callScintilla(SCI_RGBAIMAGESETWIDTH, width);
}

/** Set the height for future RGBA image data.
  */
void ScintillaWrapper::RGBAImageSetHeight(int height)
{
	DEBUG_TRACE(L"ScintillaWrapper::RGBAImageSetHeight\n");
	callScintilla(SCI_RGBAIMAGESETHEIGHT, height);
}

/** Set the scale factor in percent for future RGBA image data.
  */
void ScintillaWrapper::RGBAImageSetScale(int scalePercent)
{
	DEBUG_TRACE(L"ScintillaWrapper::RGBAImageSetScale\n");
	callScintilla(SCI_RGBAIMAGESETSCALE, scalePercent);
}

/** Define a marker from RGBA data.
  * It has the width and height from RGBAImageSetWidth/Height
  */
void ScintillaWrapper::MarkerDefineRGBAImage(int markerNumber, boost::python::object pixels)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerDefineRGBAImage\n");
	std::string stringpixels = getStringFromObject(pixels);
	callScintilla(SCI_MARKERDEFINERGBAIMAGE, markerNumber, reinterpret_cast<LPARAM>(stringpixels.c_str()));
}

/** Register an RGBA image for use in autocompletion lists.
  * It has the width and height from RGBAImageSetWidth/Height
  */
void ScintillaWrapper::RegisterRGBAImage(int type, boost::python::object pixels)
{
	DEBUG_TRACE(L"ScintillaWrapper::RegisterRGBAImage\n");
	std::string stringpixels = getStringFromObject(pixels);
	callScintilla(SCI_REGISTERRGBAIMAGE, type, reinterpret_cast<LPARAM>(stringpixels.c_str()));
}

/** Scroll to start of document.
  */
void ScintillaWrapper::ScrollToStart()
{
	DEBUG_TRACE(L"ScintillaWrapper::ScrollToStart\n");
	callScintilla(SCI_SCROLLTOSTART);
}

/** Scroll to end of document.
  */
void ScintillaWrapper::ScrollToEnd()
{
	DEBUG_TRACE(L"ScintillaWrapper::ScrollToEnd\n");
	callScintilla(SCI_SCROLLTOEND);
}

/** Set the technology used.
  */
void ScintillaWrapper::SetTechnology(int technology)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetTechnology\n");
	callScintilla(SCI_SETTECHNOLOGY, technology);
}

/** Get the tech.
  */
int ScintillaWrapper::GetTechnology()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTechnology\n");
	return callScintilla(SCI_GETTECHNOLOGY);
}

/** Create an ILoader*.
  */
intptr_t ScintillaWrapper::CreateLoader(intptr_t bytes, int documentOptions)
{
	DEBUG_TRACE(L"ScintillaWrapper::CreateLoader\n");
	return callScintilla(SCI_CREATELOADER, bytes, documentOptions);
}

/** On OS X, show a find indicator.
  */
void ScintillaWrapper::FindIndicatorShow(intptr_t start, intptr_t end)
{
	DEBUG_TRACE(L"ScintillaWrapper::FindIndicatorShow\n");
	callScintilla(SCI_FINDINDICATORSHOW, start, end);
}

/** On OS X, flash a find indicator, then fade out.
  */
void ScintillaWrapper::FindIndicatorFlash(intptr_t start, intptr_t end)
{
	DEBUG_TRACE(L"ScintillaWrapper::FindIndicatorFlash\n");
	callScintilla(SCI_FINDINDICATORFLASH, start, end);
}

/** On OS X, hide the find indicator.
  */
void ScintillaWrapper::FindIndicatorHide()
{
	DEBUG_TRACE(L"ScintillaWrapper::FindIndicatorHide\n");
	callScintilla(SCI_FINDINDICATORHIDE);
}

/** Move caret to before first visible character on display line.
  * If already there move to first character on display line.
  */
void ScintillaWrapper::VCHomeDisplay()
{
	DEBUG_TRACE(L"ScintillaWrapper::VCHomeDisplay\n");
	callScintilla(SCI_VCHOMEDISPLAY);
}

/** Like VCHomeDisplay but extending selection to new caret position.
  */
void ScintillaWrapper::VCHomeDisplayExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::VCHomeDisplayExtend\n");
	callScintilla(SCI_VCHOMEDISPLAYEXTEND);
}

/** Is the caret line always visible?
  */
bool ScintillaWrapper::GetCaretLineVisibleAlways()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCaretLineVisibleAlways\n");
	return 0 != (callScintilla(SCI_GETCARETLINEVISIBLEALWAYS));
}

/** Sets the caret line to always visible.
  */
void ScintillaWrapper::SetCaretLineVisibleAlways(bool alwaysVisible)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCaretLineVisibleAlways\n");
	callScintilla(SCI_SETCARETLINEVISIBLEALWAYS, alwaysVisible);
}

/** Set the line end types that the application wants to use. May not be used if incompatible with lexer or encoding.
  */
void ScintillaWrapper::SetLineEndTypesAllowed(int lineEndBitSet)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetLineEndTypesAllowed\n");
	callScintilla(SCI_SETLINEENDTYPESALLOWED, lineEndBitSet);
}

/** Get the line end types currently allowed.
  */
int ScintillaWrapper::GetLineEndTypesAllowed()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineEndTypesAllowed\n");
	return callScintilla(SCI_GETLINEENDTYPESALLOWED);
}

/** Get the line end types currently recognised. May be a subset of the allowed types due to lexer limitation.
  */
int ScintillaWrapper::GetLineEndTypesActive()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineEndTypesActive\n");
	return callScintilla(SCI_GETLINEENDTYPESACTIVE);
}

/** Set the way a character is drawn.
  */
void ScintillaWrapper::SetRepresentation(boost::python::object encodedCharacter, boost::python::object representation)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetRepresentation\n");
	std::string stringencodedCharacter = getStringFromObject(encodedCharacter);
	std::string stringrepresentation = getStringFromObject(representation);
	callScintilla(SCI_SETREPRESENTATION, reinterpret_cast<WPARAM>(stringencodedCharacter.c_str()), reinterpret_cast<LPARAM>(stringrepresentation.c_str()));
}

/** Set the way a character is drawn.
  * Result is NUL-terminated.
  */
boost::python::str ScintillaWrapper::GetRepresentation(boost::python::object encodedCharacter)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetRepresentation\n");
	std::string encString = getStringFromObject(encodedCharacter);
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETREPRESENTATION, reinterpret_cast<WPARAM>(encString.c_str()), 0));
	callScintilla(SCI_GETREPRESENTATION, reinterpret_cast<WPARAM>(encString.c_str()), reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Remove a character representation.
  */
void ScintillaWrapper::ClearRepresentation(boost::python::object encodedCharacter)
{
	DEBUG_TRACE(L"ScintillaWrapper::ClearRepresentation\n");
	std::string stringencodedCharacter = getStringFromObject(encodedCharacter);
	callScintilla(SCI_CLEARREPRESENTATION, reinterpret_cast<WPARAM>(stringencodedCharacter.c_str()));
}

/** Set the end of line annotation text for a line
  */
void ScintillaWrapper::EOLAnnotationSetText(intptr_t line, boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::EOLAnnotationSetText\n");
	std::string stringtext = getStringFromObject(text);
	callScintilla(SCI_EOLANNOTATIONSETTEXT, line, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Get the end of line annotation text for a line
  */
boost::python::str ScintillaWrapper::EOLAnnotationGetText(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::EOLAnnotationGetText\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_EOLANNOTATIONGETTEXT, line));
	callScintilla(SCI_EOLANNOTATIONGETTEXT, line, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Set the style number for the end of line annotations for a line
  */
void ScintillaWrapper::EOLAnnotationSetStyle(intptr_t line, int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::EOLAnnotationSetStyle\n");
	callScintilla(SCI_EOLANNOTATIONSETSTYLE, line, style);
}

/** Get the style number for the end of line annotations for a line
  */
intptr_t ScintillaWrapper::EOLAnnotationGetStyle(intptr_t line)
{
	DEBUG_TRACE(L"ScintillaWrapper::EOLAnnotationGetStyle\n");
	return callScintilla(SCI_EOLANNOTATIONGETSTYLE, line);
}

/** Clear the end of annotations from all lines
  */
void ScintillaWrapper::EOLAnnotationClearAll()
{
	DEBUG_TRACE(L"ScintillaWrapper::EOLAnnotationClearAll\n");
	callScintilla(SCI_EOLANNOTATIONCLEARALL);
}

/** Set the visibility for the end of line annotations for a view
  */
void ScintillaWrapper::EOLAnnotationSetVisible(int visible)
{
	DEBUG_TRACE(L"ScintillaWrapper::EOLAnnotationSetVisible\n");
	callScintilla(SCI_EOLANNOTATIONSETVISIBLE, visible);
}

/** Get the visibility for the end of line annotations for a view
  */
int ScintillaWrapper::EOLAnnotationGetVisible()
{
	DEBUG_TRACE(L"ScintillaWrapper::EOLAnnotationGetVisible\n");
	return callScintilla(SCI_EOLANNOTATIONGETVISIBLE);
}

/** Get the start of the range of style numbers used for end of line annotations
  */
void ScintillaWrapper::EOLAnnotationSetStyleOffset(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::EOLAnnotationSetStyleOffset\n");
	callScintilla(SCI_EOLANNOTATIONSETSTYLEOFFSET, style);
}

/** Get the start of the range of style numbers used for end of line annotations
  */
intptr_t ScintillaWrapper::EOLAnnotationGetStyleOffset()
{
	DEBUG_TRACE(L"ScintillaWrapper::EOLAnnotationGetStyleOffset\n");
	return callScintilla(SCI_EOLANNOTATIONGETSTYLEOFFSET);
}

/** Start notifying the container of all key presses and commands.
  */
void ScintillaWrapper::StartRecord()
{
	DEBUG_TRACE(L"ScintillaWrapper::StartRecord\n");
	callScintilla(SCI_STARTRECORD);
}

/** Stop notifying the container of all key presses and commands.
  */
void ScintillaWrapper::StopRecord()
{
	DEBUG_TRACE(L"ScintillaWrapper::StopRecord\n");
	callScintilla(SCI_STOPRECORD);
}

/** Set the lexing language of the document.
  */
void ScintillaWrapper::SetLexer(int lexer)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetLexer\n");
	callScintilla(SCI_SETLEXER, lexer);
}

/** Retrieve the lexing language of the document.
  */
intptr_t ScintillaWrapper::GetLexer()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLexer\n");
	return callScintilla(SCI_GETLEXER);
}

/** Colourise a segment of the document using the current lexing language.
  */
void ScintillaWrapper::Colourise(intptr_t start, intptr_t end)
{
	DEBUG_TRACE(L"ScintillaWrapper::Colourise\n");
	callScintilla(SCI_COLOURISE, start, end);
}

/** Set up a value that may be used by a lexer for some optional feature.
  */
void ScintillaWrapper::SetProperty(boost::python::object key, boost::python::object value)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetProperty\n");
	std::string stringkey = getStringFromObject(key);
	std::string stringvalue = getStringFromObject(value);
	callScintilla(SCI_SETPROPERTY, reinterpret_cast<WPARAM>(stringkey.c_str()), reinterpret_cast<LPARAM>(stringvalue.c_str()));
}

/** Set up the key words used by the lexer.
  */
void ScintillaWrapper::SetKeyWords(int keyWordSet, boost::python::object keyWords)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetKeyWords\n");
	std::string stringkeyWords = getStringFromObject(keyWords);
	callScintilla(SCI_SETKEYWORDS, keyWordSet, reinterpret_cast<LPARAM>(stringkeyWords.c_str()));
}

/** Set the lexing language of the document based on string name.
  */
void ScintillaWrapper::SetLexerLanguage(boost::python::object language)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetLexerLanguage\n");
	std::string stringlanguage = getStringFromObject(language);
	callScintilla(SCI_SETLEXERLANGUAGE, 0, reinterpret_cast<LPARAM>(stringlanguage.c_str()));
}

/** Load a lexer library (dll / so).
  */
void ScintillaWrapper::LoadLexerLibrary(boost::python::object path)
{
	DEBUG_TRACE(L"ScintillaWrapper::LoadLexerLibrary\n");
	std::string stringpath = getStringFromObject(path);
	callScintilla(SCI_LOADLEXERLIBRARY, 0, reinterpret_cast<LPARAM>(stringpath.c_str()));
}

/** Retrieve a "property" value previously set with SetProperty.
  * Result is NUL-terminated.
  */
boost::python::str ScintillaWrapper::GetProperty(boost::python::object key)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetProperty\n");
	std::string keyString = getStringFromObject(key);
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETPROPERTY, reinterpret_cast<WPARAM>(keyString.c_str()), 0));
	callScintilla(SCI_GETPROPERTY, reinterpret_cast<WPARAM>(keyString.c_str()), reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Retrieve a "property" value previously set with SetProperty,
  * with "$()" variable replacement on returned buffer.
  * Result is NUL-terminated.
  */
boost::python::str ScintillaWrapper::GetPropertyExpanded(boost::python::object key)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetPropertyExpanded\n");
	std::string keyString = getStringFromObject(key);
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETPROPERTYEXPANDED, reinterpret_cast<WPARAM>(keyString.c_str()), 0));
	callScintilla(SCI_GETPROPERTYEXPANDED, reinterpret_cast<WPARAM>(keyString.c_str()), reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Retrieve a "property" value previously set with SetProperty,
  * interpreted as an int AFTER any "$()" variable replacement.
  */
intptr_t ScintillaWrapper::GetPropertyInt(boost::python::object key, int defaultValue)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetPropertyInt\n");
	std::string stringkey = getStringFromObject(key);
	return callScintilla(SCI_GETPROPERTYINT, reinterpret_cast<WPARAM>(stringkey.c_str()), defaultValue);
}

/** Retrieve the name of the lexer.
  * Return the length of the text.
  * Result is NUL-terminated.
  */
boost::python::str ScintillaWrapper::GetLexerLanguage()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLexerLanguage\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETLEXERLANGUAGE));
	callScintilla(SCI_GETLEXERLANGUAGE, 0, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** For private communication between an application and a known lexer.
  */
intptr_t ScintillaWrapper::PrivateLexerCall(intptr_t operation, intptr_t pointer)
{
	DEBUG_TRACE(L"ScintillaWrapper::PrivateLexerCall\n");
	return callScintilla(SCI_PRIVATELEXERCALL, operation, pointer);
}

/** Retrieve a '\n' separated list of properties understood by the current lexer.
  * Result is NUL-terminated.
  */
boost::python::str ScintillaWrapper::PropertyNames()
{
	DEBUG_TRACE(L"ScintillaWrapper::PropertyNames\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_PROPERTYNAMES));
	callScintilla(SCI_PROPERTYNAMES, 0, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Retrieve the type of a property.
  */
int ScintillaWrapper::PropertyType(boost::python::object name)
{
	DEBUG_TRACE(L"ScintillaWrapper::PropertyType\n");
	std::string stringname = getStringFromObject(name);
	return callScintilla(SCI_PROPERTYTYPE, reinterpret_cast<WPARAM>(stringname.c_str()));
}

/** Describe a property.
  * Result is NUL-terminated.
  */
boost::python::str ScintillaWrapper::DescribeProperty(boost::python::object name)
{
	DEBUG_TRACE(L"ScintillaWrapper::DescribeProperty\n");
	std::string nameString = getStringFromObject(name);
	PythonCompatibleStrBuffer result(callScintilla(SCI_DESCRIBEPROPERTY, reinterpret_cast<WPARAM>(nameString.c_str()), 0));
	callScintilla(SCI_DESCRIBEPROPERTY, reinterpret_cast<WPARAM>(nameString.c_str()), reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Retrieve a '\n' separated list of descriptions of the keyword sets understood by the current lexer.
  * Result is NUL-terminated.
  */
boost::python::str ScintillaWrapper::DescribeKeyWordSets()
{
	DEBUG_TRACE(L"ScintillaWrapper::DescribeKeyWordSets\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_DESCRIBEKEYWORDSETS));
	callScintilla(SCI_DESCRIBEKEYWORDSETS, 0, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Bit set of LineEndType enumeration for which line ends beyond the standard
  * LF, CR, and CRLF are supported by the lexer.
  */
intptr_t ScintillaWrapper::GetLineEndTypesSupported()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineEndTypesSupported\n");
	return callScintilla(SCI_GETLINEENDTYPESSUPPORTED);
}

/** Allocate a set of sub styles for a particular base style, returning start of range
  */
intptr_t ScintillaWrapper::AllocateSubStyles(int styleBase, int numberStyles)
{
	DEBUG_TRACE(L"ScintillaWrapper::AllocateSubStyles\n");
	return callScintilla(SCI_ALLOCATESUBSTYLES, styleBase, numberStyles);
}

/** The starting style number for the sub styles associated with a base style
  */
intptr_t ScintillaWrapper::GetSubStylesStart(int styleBase)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSubStylesStart\n");
	return callScintilla(SCI_GETSUBSTYLESSTART, styleBase);
}

/** The number of sub styles associated with a base style
  */
intptr_t ScintillaWrapper::GetSubStylesLength(int styleBase)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSubStylesLength\n");
	return callScintilla(SCI_GETSUBSTYLESLENGTH, styleBase);
}

/** For a sub style, return the base style, else return the argument.
  */
intptr_t ScintillaWrapper::GetStyleFromSubStyle(int subStyle)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetStyleFromSubStyle\n");
	return callScintilla(SCI_GETSTYLEFROMSUBSTYLE, subStyle);
}

/** For a secondary style, return the primary style, else return the argument.
  */
intptr_t ScintillaWrapper::GetPrimaryStyleFromStyle(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetPrimaryStyleFromStyle\n");
	return callScintilla(SCI_GETPRIMARYSTYLEFROMSTYLE, style);
}

/** Free allocated sub styles
  */
void ScintillaWrapper::FreeSubStyles()
{
	DEBUG_TRACE(L"ScintillaWrapper::FreeSubStyles\n");
	callScintilla(SCI_FREESUBSTYLES);
}

/** Set the identifiers that are shown in a particular style
  */
void ScintillaWrapper::SetIdentifiers(int style, boost::python::object identifiers)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetIdentifiers\n");
	std::string stringidentifiers = getStringFromObject(identifiers);
	callScintilla(SCI_SETIDENTIFIERS, style, reinterpret_cast<LPARAM>(stringidentifiers.c_str()));
}

/** Where styles are duplicated by a feature such as active/inactive code
  * return the distance between the two types.
  */
intptr_t ScintillaWrapper::DistanceToSecondaryStyles()
{
	DEBUG_TRACE(L"ScintillaWrapper::DistanceToSecondaryStyles\n");
	return callScintilla(SCI_DISTANCETOSECONDARYSTYLES);
}

/** Get the set of base styles that can be extended with sub styles
  * Result is NUL-terminated.
  */
boost::python::str ScintillaWrapper::GetSubStyleBases()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSubStyleBases\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETSUBSTYLEBASES));
	callScintilla(SCI_GETSUBSTYLEBASES, 0, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Retrieve the number of named styles for the lexer.
  */
intptr_t ScintillaWrapper::GetNamedStyles()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetNamedStyles\n");
	return callScintilla(SCI_GETNAMEDSTYLES);
}

/** Retrieve the name of a style.
  * Result is NUL-terminated.
  */
boost::python::str ScintillaWrapper::NameOfStyle(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::NameOfStyle\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_NAMEOFSTYLE, style));
	callScintilla(SCI_NAMEOFSTYLE, style, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Retrieve a ' ' separated list of style tags like "literal quoted string".
  * Result is NUL-terminated.
  */
boost::python::str ScintillaWrapper::TagsOfStyle(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::TagsOfStyle\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_TAGSOFSTYLE, style));
	callScintilla(SCI_TAGSOFSTYLE, style, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Retrieve a description of a style.
  * Result is NUL-terminated.
  */
boost::python::str ScintillaWrapper::DescriptionOfStyle(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::DescriptionOfStyle\n");
	PythonCompatibleStrBuffer result(callScintilla(SCI_DESCRIPTIONOFSTYLE, style));
	callScintilla(SCI_DESCRIPTIONOFSTYLE, style, reinterpret_cast<LPARAM>(*result));
	return boost::python::str(result.c_str());
}

/** Set the lexer from an ILexer*.
  */
void ScintillaWrapper::SetILexer(intptr_t ilexer)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetILexer\n");
	callScintilla(SCI_SETILEXER, 0, ilexer);
}

/** Retrieve bidirectional text display state.
  */
int ScintillaWrapper::GetBidirectional()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetBidirectional\n");
	return callScintilla(SCI_GETBIDIRECTIONAL);
}

/** Set bidirectional text display state.
  */
void ScintillaWrapper::SetBidirectional(int bidirectional)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetBidirectional\n");
	callScintilla(SCI_SETBIDIRECTIONAL, bidirectional);
}

/** Retrieve line character index state.
  */
int ScintillaWrapper::GetLineCharacterIndex()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineCharacterIndex\n");
	return callScintilla(SCI_GETLINECHARACTERINDEX);
}

/** Request line character index be created or its use count increased.
  */
void ScintillaWrapper::AllocateLineCharacterIndex(int lineCharacterIndex)
{
	DEBUG_TRACE(L"ScintillaWrapper::AllocateLineCharacterIndex\n");
	callScintilla(SCI_ALLOCATELINECHARACTERINDEX, lineCharacterIndex);
}

/** Decrease use count of line character index and remove if 0.
  */
void ScintillaWrapper::ReleaseLineCharacterIndex(int lineCharacterIndex)
{
	DEBUG_TRACE(L"ScintillaWrapper::ReleaseLineCharacterIndex\n");
	callScintilla(SCI_RELEASELINECHARACTERINDEX, lineCharacterIndex);
}

/** Retrieve the document line containing a position measured in index units.
  */
intptr_t ScintillaWrapper::LineFromIndexPosition(intptr_t pos, int lineCharacterIndex)
{
	DEBUG_TRACE(L"ScintillaWrapper::LineFromIndexPosition\n");
	return callScintilla(SCI_LINEFROMINDEXPOSITION, pos, lineCharacterIndex);
}

/** Retrieve the position measured in index units at the start of a document line.
  */
intptr_t ScintillaWrapper::IndexPositionFromLine(intptr_t line, int lineCharacterIndex)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndexPositionFromLine\n");
	return callScintilla(SCI_INDEXPOSITIONFROMLINE, line, lineCharacterIndex);
}

}
