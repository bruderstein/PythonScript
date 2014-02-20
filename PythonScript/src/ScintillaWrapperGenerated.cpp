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
	inline ~PythonCompatibleStrBuffer() { delete [] m_bufferPtr; }
	inline char* operator*() { return m_bufferPtr; }
	inline const char* c_str() const { return m_bufferPtr; }
	inline size_t size() const { return m_bufferLen; }
private:
	PythonCompatibleStrBuffer();  // default constructor disabled
	PythonCompatibleStrBuffer(const PythonCompatibleStrBuffer&); // copy constructor disabled
	PythonCompatibleStrBuffer& operator = (const PythonCompatibleStrBuffer&); // Disable assignment operator disabled
	size_t m_bufferLen;
	char* m_bufferPtr;
};

/** Add text to the document at current position.
  */
int ScintillaWrapper::AddText(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::AddText\n");
	std::string s = getStringFromObject(text);
	GILRelease gilRelease;
	return callScintilla(SCI_ADDTEXT, s.size(), reinterpret_cast<LPARAM>(s.c_str()));
}

/** Add array of cells to document.
  */
int ScintillaWrapper::AddStyledText(ScintillaCells c)
{
	DEBUG_TRACE(L"ScintillaWrapper::AddStyledText\n");
	GILRelease gilRelease;
	return callScintilla(SCI_ADDSTYLEDTEXT, c.length(), reinterpret_cast<LPARAM>(c.cells()));
}

/** Insert string at a position.
  */
void ScintillaWrapper::InsertText(int pos, boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::InsertText\n");
	std::string stringtext = getStringFromObject(text);
	GILRelease gilRelease;
	callScintilla(SCI_INSERTTEXT, pos, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Delete all text in the document.
  */
void ScintillaWrapper::ClearAll()
{
	DEBUG_TRACE(L"ScintillaWrapper::ClearAll\n");
	GILRelease gilRelease;
	callScintilla(SCI_CLEARALL);
}

/** Set all style bytes to 0, remove all folding information.
  */
void ScintillaWrapper::ClearDocumentStyle()
{
	DEBUG_TRACE(L"ScintillaWrapper::ClearDocumentStyle\n");
	GILRelease gilRelease;
	callScintilla(SCI_CLEARDOCUMENTSTYLE);
}

/** Returns the number of bytes in the document.
  */
int ScintillaWrapper::GetLength()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLength\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETLENGTH);
}

/** Returns the character byte at the position.
  */
int ScintillaWrapper::GetCharAt(int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCharAt\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETCHARAT, pos);
}

/** Returns the position of the caret.
  */
int ScintillaWrapper::GetCurrentPos()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCurrentPos\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETCURRENTPOS);
}

/** Returns the position of the opposite end of the selection to the caret.
  */
int ScintillaWrapper::GetAnchor()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetAnchor\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETANCHOR);
}

/** Returns the style byte at the position.
  */
int ScintillaWrapper::GetStyleAt(int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetStyleAt\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETSTYLEAT, pos);
}

/** Redoes the next action on the undo history.
  */
void ScintillaWrapper::Redo()
{
	DEBUG_TRACE(L"ScintillaWrapper::Redo\n");
	GILRelease gilRelease;
	callScintilla(SCI_REDO);
}

/** Choose between collecting actions into the undo
  * history and discarding them.
  */
void ScintillaWrapper::SetUndoCollection(bool collectUndo)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetUndoCollection\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETUNDOCOLLECTION, collectUndo);
}

/** Select all the text in the document.
  */
void ScintillaWrapper::SelectAll()
{
	DEBUG_TRACE(L"ScintillaWrapper::SelectAll\n");
	GILRelease gilRelease;
	callScintilla(SCI_SELECTALL);
}

/** Remember the current position in the undo history as the position
  * at which the document was saved.
  */
void ScintillaWrapper::SetSavePoint()
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSavePoint\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSAVEPOINT);
}

/** Retrieve a buffer of cells.
  * Returns the number of bytes in the buffer not including terminating NULs.
  */
boost::python::tuple ScintillaWrapper::GetStyledText(int start, int end)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetStyledText\n");
	GILRelease gilRelease;
	Sci_TextRange src;
	if (end < start)
	{
		int temp = start;
		start = end;
		end = temp;
	}
	src.chrg.cpMin = start;
	src.chrg.cpMax = end;
	src.lpstrText = new char[size_t(((end-start) * 2) + 2)];
	callScintilla(SCI_GETSTYLEDTEXT, 0, reinterpret_cast<LPARAM>(&src));
	gilRelease.reacquire();
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
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_CANREDO));
}

/** Retrieve the line number at which a particular marker is located.
  */
int ScintillaWrapper::MarkerLineFromHandle(int handle)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerLineFromHandle\n");
	GILRelease gilRelease;
	return callScintilla(SCI_MARKERLINEFROMHANDLE, handle);
}

/** Delete a marker.
  */
void ScintillaWrapper::MarkerDeleteHandle(int handle)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerDeleteHandle\n");
	GILRelease gilRelease;
	callScintilla(SCI_MARKERDELETEHANDLE, handle);
}

/** Is undo history being collected?
  */
bool ScintillaWrapper::GetUndoCollection()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetUndoCollection\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETUNDOCOLLECTION));
}

/** Are white space characters currently visible?
  * Returns one of SCWS_* constants.
  */
int ScintillaWrapper::GetViewWS()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetViewWS\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETVIEWWS);
}

/** Make white space characters invisible, always visible or visible outside indentation.
  */
void ScintillaWrapper::SetViewWS(int viewWS)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetViewWS\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETVIEWWS, viewWS);
}

/** Find the position from a point within the window.
  */
int ScintillaWrapper::PositionFromPoint(int x, int y)
{
	DEBUG_TRACE(L"ScintillaWrapper::PositionFromPoint\n");
	GILRelease gilRelease;
	return callScintilla(SCI_POSITIONFROMPOINT, x, y);
}

/** Find the position from a point within the window but return
  * INVALID_POSITION if not close to text.
  */
int ScintillaWrapper::PositionFromPointClose(int x, int y)
{
	DEBUG_TRACE(L"ScintillaWrapper::PositionFromPointClose\n");
	GILRelease gilRelease;
	return callScintilla(SCI_POSITIONFROMPOINTCLOSE, x, y);
}

/** Set caret to start of a line and ensure it is visible.
  */
void ScintillaWrapper::GotoLine(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GotoLine\n");
	GILRelease gilRelease;
	callScintilla(SCI_GOTOLINE, line);
}

/** Set caret to a position and ensure it is visible.
  */
void ScintillaWrapper::GotoPos(int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::GotoPos\n");
	GILRelease gilRelease;
	callScintilla(SCI_GOTOPOS, pos);
}

/** Set the selection anchor to a position. The anchor is the opposite
  * end of the selection from the caret.
  */
void ScintillaWrapper::SetAnchor(int posAnchor)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetAnchor\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETANCHOR, posAnchor);
}

/** Retrieve the text of the line containing the caret.
  * Returns the index of the caret on the line.
  */
boost::python::str ScintillaWrapper::GetCurLine()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCurLine\n");
	GILRelease gilRelease;
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETCURLINE) + 1);
	// result.size() does not depend on the order of evaluation here
	//lint -e{864}
	callScintilla(SCI_GETCURLINE, result.size(), reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

/** Retrieve the position of the last correctly styled character.
  */
int ScintillaWrapper::GetEndStyled()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetEndStyled\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETENDSTYLED);
}

/** Convert all line endings in the document to one mode.
  */
void ScintillaWrapper::ConvertEOLs(int eolMode)
{
	DEBUG_TRACE(L"ScintillaWrapper::ConvertEOLs\n");
	GILRelease gilRelease;
	callScintilla(SCI_CONVERTEOLS, eolMode);
}

/** Retrieve the current end of line mode - one of CRLF, CR, or LF.
  */
int ScintillaWrapper::GetEOLMode()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetEOLMode\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETEOLMODE);
}

/** Set the current end of line mode.
  */
void ScintillaWrapper::SetEOLMode(int eolMode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetEOLMode\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETEOLMODE, eolMode);
}

/** Set the current styling position to pos and the styling mask to mask.
  * The styling mask can be used to protect some bits in each styling byte from modification.
  */
void ScintillaWrapper::StartStyling(int pos, int mask)
{
	DEBUG_TRACE(L"ScintillaWrapper::StartStyling\n");
	GILRelease gilRelease;
	callScintilla(SCI_STARTSTYLING, pos, mask);
}

/** Change style from current styling position for length characters to a style
  * and move the current styling position to after this newly styled segment.
  */
void ScintillaWrapper::SetStyling(int length, int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetStyling\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSTYLING, length, style);
}

/** Is drawing done first into a buffer or direct to the screen?
  */
bool ScintillaWrapper::GetBufferedDraw()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetBufferedDraw\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETBUFFEREDDRAW));
}

/** If drawing is buffered then each line of text is drawn into a bitmap buffer
  * before drawing it to the screen to avoid flicker.
  */
void ScintillaWrapper::SetBufferedDraw(bool buffered)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetBufferedDraw\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETBUFFEREDDRAW, buffered);
}

/** Change the visible size of a tab to be a multiple of the width of a space character.
  */
void ScintillaWrapper::SetTabWidth(int tabWidth)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetTabWidth\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETTABWIDTH, tabWidth);
}

/** Retrieve the visible size of a tab.
  */
int ScintillaWrapper::GetTabWidth()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTabWidth\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETTABWIDTH);
}

/** Set the code page used to interpret the bytes of the document as characters.
  * The SC_CP_UTF8 value can be used to enter Unicode mode.
  */
void ScintillaWrapper::SetCodePage(int codePage)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCodePage\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETCODEPAGE, codePage);
}

/** In palette mode, Scintilla uses the environment's palette calls to display
  * more colours. This may lead to ugly displays.
  */
void ScintillaWrapper::SetUsePalette(bool usePalette)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetUsePalette\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETUSEPALETTE, usePalette);
}

/** Set the symbol used for a particular marker number.
  */
void ScintillaWrapper::MarkerDefine(int markerNumber, int markerSymbol)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerDefine\n");
	GILRelease gilRelease;
	callScintilla(SCI_MARKERDEFINE, markerNumber, markerSymbol);
}

/** Set the foreground colour used for a particular marker number.
  */
void ScintillaWrapper::MarkerSetFore(int markerNumber, boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerSetFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	GILRelease gilRelease;
	callScintilla(SCI_MARKERSETFORE, markerNumber, static_cast<LPARAM>(rgbfore));
}

/** Set the background colour used for a particular marker number.
  */
void ScintillaWrapper::MarkerSetBack(int markerNumber, boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerSetBack\n");
	COLORREF rgbback = MAKECOLOUR(back);
	GILRelease gilRelease;
	callScintilla(SCI_MARKERSETBACK, markerNumber, static_cast<LPARAM>(rgbback));
}

/** Add a marker to a line, returning an ID which can be used to find or delete the marker.
  */
int ScintillaWrapper::MarkerAdd(int line, int markerNumber)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerAdd\n");
	GILRelease gilRelease;
	return callScintilla(SCI_MARKERADD, line, markerNumber);
}

/** Delete a marker from a line.
  */
void ScintillaWrapper::MarkerDelete(int line, int markerNumber)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerDelete\n");
	GILRelease gilRelease;
	callScintilla(SCI_MARKERDELETE, line, markerNumber);
}

/** Delete all markers with a particular number from all lines.
  */
void ScintillaWrapper::MarkerDeleteAll(int markerNumber)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerDeleteAll\n");
	GILRelease gilRelease;
	callScintilla(SCI_MARKERDELETEALL, markerNumber);
}

/** Get a bit mask of all the markers set on a line.
  */
int ScintillaWrapper::MarkerGet(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerGet\n");
	GILRelease gilRelease;
	return callScintilla(SCI_MARKERGET, line);
}

/** Find the next line at or after lineStart that includes a marker in mask.
  * Return -1 when no more lines.
  */
int ScintillaWrapper::MarkerNext(int lineStart, int markerMask)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerNext\n");
	GILRelease gilRelease;
	return callScintilla(SCI_MARKERNEXT, lineStart, markerMask);
}

/** Find the previous line before lineStart that includes a marker in mask.
  */
int ScintillaWrapper::MarkerPrevious(int lineStart, int markerMask)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerPrevious\n");
	GILRelease gilRelease;
	return callScintilla(SCI_MARKERPREVIOUS, lineStart, markerMask);
}

/** Define a marker from a pixmap.
  */
void ScintillaWrapper::MarkerDefinePixmap(int markerNumber, boost::python::object pixmap)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerDefinePixmap\n");
	std::string stringpixmap = getStringFromObject(pixmap);
	GILRelease gilRelease;
	callScintilla(SCI_MARKERDEFINEPIXMAP, markerNumber, reinterpret_cast<LPARAM>(stringpixmap.c_str()));
}

/** Add a set of markers to a line.
  */
void ScintillaWrapper::MarkerAddSet(int line, int set)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerAddSet\n");
	GILRelease gilRelease;
	callScintilla(SCI_MARKERADDSET, line, set);
}

/** Set the alpha used for a marker that is drawn in the text area, not the margin.
  */
void ScintillaWrapper::MarkerSetAlpha(int markerNumber, int alpha)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerSetAlpha\n");
	GILRelease gilRelease;
	callScintilla(SCI_MARKERSETALPHA, markerNumber, alpha);
}

/** Set a margin to be either numeric or symbolic.
  */
void ScintillaWrapper::SetMarginTypeN(int margin, int marginType)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMarginTypeN\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETMARGINTYPEN, margin, marginType);
}

/** Retrieve the type of a margin.
  */
int ScintillaWrapper::GetMarginTypeN(int margin)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMarginTypeN\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETMARGINTYPEN, margin);
}

/** Set the width of a margin to a width expressed in pixels.
  */
void ScintillaWrapper::SetMarginWidthN(int margin, int pixelWidth)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMarginWidthN\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETMARGINWIDTHN, margin, pixelWidth);
}

/** Retrieve the width of a margin in pixels.
  */
int ScintillaWrapper::GetMarginWidthN(int margin)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMarginWidthN\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETMARGINWIDTHN, margin);
}

/** Set a mask that determines which markers are displayed in a margin.
  */
void ScintillaWrapper::SetMarginMaskN(int margin, int mask)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMarginMaskN\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETMARGINMASKN, margin, mask);
}

/** Retrieve the marker mask of a margin.
  */
int ScintillaWrapper::GetMarginMaskN(int margin)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMarginMaskN\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETMARGINMASKN, margin);
}

/** Make a margin sensitive or insensitive to mouse clicks.
  */
void ScintillaWrapper::SetMarginSensitiveN(int margin, bool sensitive)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMarginSensitiveN\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETMARGINSENSITIVEN, margin, sensitive);
}

/** Retrieve the mouse click sensitivity of a margin.
  */
bool ScintillaWrapper::GetMarginSensitiveN(int margin)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMarginSensitiveN\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETMARGINSENSITIVEN, margin));
}

/** Set the cursor shown when the mouse is inside a margin.
  */
void ScintillaWrapper::SetMarginCursorN(int margin, int cursor)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMarginCursorN\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETMARGINCURSORN, margin, cursor);
}

/** Retrieve the cursor shown in a margin.
  */
int ScintillaWrapper::GetMarginCursorN(int margin)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMarginCursorN\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETMARGINCURSORN, margin);
}

/** Clear all the styles and make equivalent to the global default style.
  */
void ScintillaWrapper::StyleClearAll()
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleClearAll\n");
	GILRelease gilRelease;
	callScintilla(SCI_STYLECLEARALL);
}

/** Set the foreground colour of a style.
  */
void ScintillaWrapper::StyleSetFore(int style, boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	GILRelease gilRelease;
	callScintilla(SCI_STYLESETFORE, style, static_cast<LPARAM>(rgbfore));
}

/** Set the background colour of a style.
  */
void ScintillaWrapper::StyleSetBack(int style, boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetBack\n");
	COLORREF rgbback = MAKECOLOUR(back);
	GILRelease gilRelease;
	callScintilla(SCI_STYLESETBACK, style, static_cast<LPARAM>(rgbback));
}

/** Set a style to be bold or not.
  */
void ScintillaWrapper::StyleSetBold(int style, bool bold)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetBold\n");
	GILRelease gilRelease;
	callScintilla(SCI_STYLESETBOLD, style, bold);
}

/** Set a style to be italic or not.
  */
void ScintillaWrapper::StyleSetItalic(int style, bool italic)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetItalic\n");
	GILRelease gilRelease;
	callScintilla(SCI_STYLESETITALIC, style, italic);
}

/** Set the size of characters of a style.
  */
void ScintillaWrapper::StyleSetSize(int style, int sizePoints)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetSize\n");
	GILRelease gilRelease;
	callScintilla(SCI_STYLESETSIZE, style, sizePoints);
}

/** Set the font of a style.
  */
void ScintillaWrapper::StyleSetFont(int style, boost::python::object fontName)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetFont\n");
	std::string stringfontName = getStringFromObject(fontName);
	GILRelease gilRelease;
	callScintilla(SCI_STYLESETFONT, style, reinterpret_cast<LPARAM>(stringfontName.c_str()));
}

/** Set a style to have its end of line filled or not.
  */
void ScintillaWrapper::StyleSetEOLFilled(int style, bool filled)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetEOLFilled\n");
	GILRelease gilRelease;
	callScintilla(SCI_STYLESETEOLFILLED, style, filled);
}

/** Reset the default style to its state at startup
  */
void ScintillaWrapper::StyleResetDefault()
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleResetDefault\n");
	GILRelease gilRelease;
	callScintilla(SCI_STYLERESETDEFAULT);
}

/** Set a style to be underlined or not.
  */
void ScintillaWrapper::StyleSetUnderline(int style, bool underline)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetUnderline\n");
	GILRelease gilRelease;
	callScintilla(SCI_STYLESETUNDERLINE, style, underline);
}

/** Get the foreground colour of a style.
  */
boost::python::tuple ScintillaWrapper::StyleGetFore(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetFore\n");
	GILRelease gilRelease;
	int retVal = (int)callScintilla(SCI_STYLEGETFORE, style);
	gilRelease.reacquire();
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** Get the background colour of a style.
  */
boost::python::tuple ScintillaWrapper::StyleGetBack(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetBack\n");
	GILRelease gilRelease;
	int retVal = (int)callScintilla(SCI_STYLEGETBACK, style);
	gilRelease.reacquire();
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** Get is a style bold or not.
  */
bool ScintillaWrapper::StyleGetBold(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetBold\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_STYLEGETBOLD, style));
}

/** Get is a style italic or not.
  */
bool ScintillaWrapper::StyleGetItalic(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetItalic\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_STYLEGETITALIC, style));
}

/** Get the size of characters of a style.
  */
int ScintillaWrapper::StyleGetSize(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetSize\n");
	GILRelease gilRelease;
	return callScintilla(SCI_STYLEGETSIZE, style);
}

/** Get the font of a style.
  * Returns the length of the fontName
  */
boost::python::str ScintillaWrapper::StyleGetFont()
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetFont\n");
	GILRelease gilRelease;
	PythonCompatibleStrBuffer result(callScintilla(SCI_STYLEGETFONT));
	callScintilla(SCI_STYLEGETFONT, 0, reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

/** Get is a style to have its end of line filled or not.
  */
bool ScintillaWrapper::StyleGetEOLFilled(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetEOLFilled\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_STYLEGETEOLFILLED, style));
}

/** Get is a style underlined or not.
  */
bool ScintillaWrapper::StyleGetUnderline(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetUnderline\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_STYLEGETUNDERLINE, style));
}

/** Get is a style mixed case, or to force upper or lower case.
  */
int ScintillaWrapper::StyleGetCase(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetCase\n");
	GILRelease gilRelease;
	return callScintilla(SCI_STYLEGETCASE, style);
}

/** Get the character get of the font in a style.
  */
int ScintillaWrapper::StyleGetCharacterSet(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetCharacterSet\n");
	GILRelease gilRelease;
	return callScintilla(SCI_STYLEGETCHARACTERSET, style);
}

/** Get is a style visible or not.
  */
bool ScintillaWrapper::StyleGetVisible(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetVisible\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_STYLEGETVISIBLE, style));
}

/** Get is a style changeable or not (read only).
  * Experimental feature, currently buggy.
  */
bool ScintillaWrapper::StyleGetChangeable(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetChangeable\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_STYLEGETCHANGEABLE, style));
}

/** Get is a style a hotspot or not.
  */
bool ScintillaWrapper::StyleGetHotSpot(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleGetHotSpot\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_STYLEGETHOTSPOT, style));
}

/** Set a style to be mixed case, or to force upper or lower case.
  */
void ScintillaWrapper::StyleSetCase(int style, int caseForce)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetCase\n");
	GILRelease gilRelease;
	callScintilla(SCI_STYLESETCASE, style, caseForce);
}

/** Set the character set of the font in a style.
  */
void ScintillaWrapper::StyleSetCharacterSet(int style, int characterSet)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetCharacterSet\n");
	GILRelease gilRelease;
	callScintilla(SCI_STYLESETCHARACTERSET, style, characterSet);
}

/** Set a style to be a hotspot or not.
  */
void ScintillaWrapper::StyleSetHotSpot(int style, bool hotspot)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetHotSpot\n");
	GILRelease gilRelease;
	callScintilla(SCI_STYLESETHOTSPOT, style, hotspot);
}

/** Set the foreground colour of the main and additional selections and whether to use this setting.
  */
void ScintillaWrapper::SetSelFore(bool useSetting, boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	GILRelease gilRelease;
	callScintilla(SCI_SETSELFORE, useSetting, static_cast<LPARAM>(rgbfore));
}

/** Set the background colour of the main and additional selections and whether to use this setting.
  */
void ScintillaWrapper::SetSelBack(bool useSetting, boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelBack\n");
	COLORREF rgbback = MAKECOLOUR(back);
	GILRelease gilRelease;
	callScintilla(SCI_SETSELBACK, useSetting, static_cast<LPARAM>(rgbback));
}

/** Get the alpha of the selection.
  */
int ScintillaWrapper::GetSelAlpha()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelAlpha\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETSELALPHA);
}

/** Set the alpha of the selection.
  */
void ScintillaWrapper::SetSelAlpha(int alpha)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelAlpha\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSELALPHA, alpha);
}

/** Is the selection end of line filled?
  */
bool ScintillaWrapper::GetSelEOLFilled()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelEOLFilled\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETSELEOLFILLED));
}

/** Set the selection to have its end of line filled or not.
  */
void ScintillaWrapper::SetSelEOLFilled(bool filled)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelEOLFilled\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSELEOLFILLED, filled);
}

/** Set the foreground colour of the caret.
  */
void ScintillaWrapper::SetCaretFore(boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCaretFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	GILRelease gilRelease;
	callScintilla(SCI_SETCARETFORE, static_cast<WPARAM>(rgbfore));
}

/** When key+modifier combination km is pressed perform msg.
  */
void ScintillaWrapper::AssignCmdKey(int km, int msg)
{
	DEBUG_TRACE(L"ScintillaWrapper::AssignCmdKey\n");
	GILRelease gilRelease;
	callScintilla(SCI_ASSIGNCMDKEY, km, msg);
}

/** When key+modifier combination km is pressed do nothing.
  */
void ScintillaWrapper::ClearCmdKey(int km)
{
	DEBUG_TRACE(L"ScintillaWrapper::ClearCmdKey\n");
	GILRelease gilRelease;
	callScintilla(SCI_CLEARCMDKEY, km);
}

/** Drop all key mappings.
  */
void ScintillaWrapper::ClearAllCmdKeys()
{
	DEBUG_TRACE(L"ScintillaWrapper::ClearAllCmdKeys\n");
	GILRelease gilRelease;
	callScintilla(SCI_CLEARALLCMDKEYS);
}

/** Set the styles for a segment of the document.
  */
int ScintillaWrapper::SetStylingEx(boost::python::object styles)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetStylingEx\n");
	std::string s = getStringFromObject(styles);
	GILRelease gilRelease;
	return callScintilla(SCI_SETSTYLINGEX, s.size(), reinterpret_cast<LPARAM>(s.c_str()));
}

/** Set a style to be visible or not.
  */
void ScintillaWrapper::StyleSetVisible(int style, bool visible)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetVisible\n");
	GILRelease gilRelease;
	callScintilla(SCI_STYLESETVISIBLE, style, visible);
}

/** Get the time in milliseconds that the caret is on and off.
  */
int ScintillaWrapper::GetCaretPeriod()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCaretPeriod\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETCARETPERIOD);
}

/** Get the time in milliseconds that the caret is on and off. 0 = steady on.
  */
void ScintillaWrapper::SetCaretPeriod(int periodMilliseconds)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCaretPeriod\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETCARETPERIOD, periodMilliseconds);
}

/** Set the set of characters making up words for when moving or selecting by word.
  * First sets defaults like SetCharsDefault.
  */
void ScintillaWrapper::SetWordChars(boost::python::object characters)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWordChars\n");
	std::string stringcharacters = getStringFromObject(characters);
	GILRelease gilRelease;
	callScintilla(SCI_SETWORDCHARS, 0, reinterpret_cast<LPARAM>(stringcharacters.c_str()));
}

/** Start a sequence of actions that is undone and redone as a unit.
  * May be nested.
  */
void ScintillaWrapper::BeginUndoAction()
{
	DEBUG_TRACE(L"ScintillaWrapper::BeginUndoAction\n");
	GILRelease gilRelease;
	callScintilla(SCI_BEGINUNDOACTION);
}

/** End a sequence of actions that is undone and redone as a unit.
  */
void ScintillaWrapper::EndUndoAction()
{
	DEBUG_TRACE(L"ScintillaWrapper::EndUndoAction\n");
	GILRelease gilRelease;
	callScintilla(SCI_ENDUNDOACTION);
}

/** Set an indicator to plain, squiggle or TT.
  */
void ScintillaWrapper::IndicSetStyle(int indic, int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicSetStyle\n");
	GILRelease gilRelease;
	callScintilla(SCI_INDICSETSTYLE, indic, style);
}

/** Retrieve the style of an indicator.
  */
int ScintillaWrapper::IndicGetStyle(int indic)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicGetStyle\n");
	GILRelease gilRelease;
	return callScintilla(SCI_INDICGETSTYLE, indic);
}

/** Set the foreground colour of an indicator.
  */
void ScintillaWrapper::IndicSetFore(int indic, boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicSetFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	GILRelease gilRelease;
	callScintilla(SCI_INDICSETFORE, indic, static_cast<LPARAM>(rgbfore));
}

/** Retrieve the foreground colour of an indicator.
  */
boost::python::tuple ScintillaWrapper::IndicGetFore(int indic)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicGetFore\n");
	GILRelease gilRelease;
	int retVal = (int)callScintilla(SCI_INDICGETFORE, indic);
	gilRelease.reacquire();
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** Set an indicator to draw under text or over(default).
  */
void ScintillaWrapper::IndicSetUnder(int indic, bool under)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicSetUnder\n");
	GILRelease gilRelease;
	callScintilla(SCI_INDICSETUNDER, indic, under);
}

/** Retrieve whether indicator drawn under or over text.
  */
bool ScintillaWrapper::IndicGetUnder(int indic)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicGetUnder\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_INDICGETUNDER, indic));
}

/** Set the foreground colour of all whitespace and whether to use this setting.
  */
void ScintillaWrapper::SetWhitespaceFore(bool useSetting, boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWhitespaceFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	GILRelease gilRelease;
	callScintilla(SCI_SETWHITESPACEFORE, useSetting, static_cast<LPARAM>(rgbfore));
}

/** Set the background colour of all whitespace and whether to use this setting.
  */
void ScintillaWrapper::SetWhitespaceBack(bool useSetting, boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWhitespaceBack\n");
	COLORREF rgbback = MAKECOLOUR(back);
	GILRelease gilRelease;
	callScintilla(SCI_SETWHITESPACEBACK, useSetting, static_cast<LPARAM>(rgbback));
}

/** Set the size of the dots used to mark space characters.
  */
void ScintillaWrapper::SetWhitespaceSize(int size)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWhitespaceSize\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETWHITESPACESIZE, size);
}

/** Get the size of the dots used to mark space characters.
  */
int ScintillaWrapper::GetWhitespaceSize()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetWhitespaceSize\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETWHITESPACESIZE);
}

/** Divide each styling byte into lexical class bits (default: 5) and indicator
  * bits (default: 3). If a lexer requires more than 32 lexical states, then this
  * is used to expand the possible states.
  */
void ScintillaWrapper::SetStyleBits(int bits)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetStyleBits\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSTYLEBITS, bits);
}

/** Retrieve number of bits in style bytes used to hold the lexical state.
  */
int ScintillaWrapper::GetStyleBits()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetStyleBits\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETSTYLEBITS);
}

/** Used to hold extra styling information for each line.
  */
void ScintillaWrapper::SetLineState(int line, int state)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetLineState\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETLINESTATE, line, state);
}

/** Retrieve the extra styling information for a line.
  */
int ScintillaWrapper::GetLineState(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineState\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETLINESTATE, line);
}

/** Retrieve the last line number that has line state.
  */
int ScintillaWrapper::GetMaxLineState()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMaxLineState\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETMAXLINESTATE);
}

/** Is the background of the line containing the caret in a different colour?
  */
bool ScintillaWrapper::GetCaretLineVisible()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCaretLineVisible\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETCARETLINEVISIBLE));
}

/** Display the background of the line containing the caret in a different colour.
  */
void ScintillaWrapper::SetCaretLineVisible(bool show)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCaretLineVisible\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETCARETLINEVISIBLE, show);
}

/** Get the colour of the background of the line containing the caret.
  */
boost::python::tuple ScintillaWrapper::GetCaretLineBack()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCaretLineBack\n");
	GILRelease gilRelease;
	int retVal = (int)callScintilla(SCI_GETCARETLINEBACK);
	gilRelease.reacquire();
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** Set the colour of the background of the line containing the caret.
  */
void ScintillaWrapper::SetCaretLineBack(boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCaretLineBack\n");
	COLORREF rgbback = MAKECOLOUR(back);
	GILRelease gilRelease;
	callScintilla(SCI_SETCARETLINEBACK, static_cast<WPARAM>(rgbback));
}

/** Set a style to be changeable or not (read only).
  * Experimental feature, currently buggy.
  */
void ScintillaWrapper::StyleSetChangeable(int style, bool changeable)
{
	DEBUG_TRACE(L"ScintillaWrapper::StyleSetChangeable\n");
	GILRelease gilRelease;
	callScintilla(SCI_STYLESETCHANGEABLE, style, changeable);
}

/** Display a auto-completion list.
  * The lenEntered parameter indicates how many characters before
  * the caret should be used to provide context.
  */
void ScintillaWrapper::AutoCShow(int lenEntered, boost::python::object itemList)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCShow\n");
	std::string stringitemList = getStringFromObject(itemList);
	GILRelease gilRelease;
	callScintilla(SCI_AUTOCSHOW, lenEntered, reinterpret_cast<LPARAM>(stringitemList.c_str()));
}

/** Remove the auto-completion list from the screen.
  */
void ScintillaWrapper::AutoCCancel()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCCancel\n");
	GILRelease gilRelease;
	callScintilla(SCI_AUTOCCANCEL);
}

/** Is there an auto-completion list visible?
  */
bool ScintillaWrapper::AutoCActive()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCActive\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_AUTOCACTIVE));
}

/** Retrieve the position of the caret when the auto-completion list was displayed.
  */
int ScintillaWrapper::AutoCPosStart()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCPosStart\n");
	GILRelease gilRelease;
	return callScintilla(SCI_AUTOCPOSSTART);
}

/** User has selected an item so remove the list and insert the selection.
  */
void ScintillaWrapper::AutoCComplete()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCComplete\n");
	GILRelease gilRelease;
	callScintilla(SCI_AUTOCCOMPLETE);
}

/** Define a set of character that when typed cancel the auto-completion list.
  */
void ScintillaWrapper::AutoCStops(boost::python::object characterSet)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCStops\n");
	std::string stringcharacterSet = getStringFromObject(characterSet);
	GILRelease gilRelease;
	callScintilla(SCI_AUTOCSTOPS, 0, reinterpret_cast<LPARAM>(stringcharacterSet.c_str()));
}

/** Change the separator character in the string setting up an auto-completion list.
  * Default is space but can be changed if items contain space.
  */
void ScintillaWrapper::AutoCSetSeparator(int separatorCharacter)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetSeparator\n");
	GILRelease gilRelease;
	callScintilla(SCI_AUTOCSETSEPARATOR, separatorCharacter);
}

/** Retrieve the auto-completion list separator character.
  */
int ScintillaWrapper::AutoCGetSeparator()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetSeparator\n");
	GILRelease gilRelease;
	return callScintilla(SCI_AUTOCGETSEPARATOR);
}

/** Select the item in the auto-completion list that starts with a string.
  */
void ScintillaWrapper::AutoCSelect(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSelect\n");
	std::string stringtext = getStringFromObject(text);
	GILRelease gilRelease;
	callScintilla(SCI_AUTOCSELECT, 0, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Should the auto-completion list be cancelled if the user backspaces to a
  * position before where the box was created.
  */
void ScintillaWrapper::AutoCSetCancelAtStart(bool cancel)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetCancelAtStart\n");
	GILRelease gilRelease;
	callScintilla(SCI_AUTOCSETCANCELATSTART, cancel);
}

/** Retrieve whether auto-completion cancelled by backspacing before start.
  */
bool ScintillaWrapper::AutoCGetCancelAtStart()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetCancelAtStart\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_AUTOCGETCANCELATSTART));
}

/** Define a set of characters that when typed will cause the autocompletion to
  * choose the selected item.
  */
void ScintillaWrapper::AutoCSetFillUps(boost::python::object characterSet)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetFillUps\n");
	std::string stringcharacterSet = getStringFromObject(characterSet);
	GILRelease gilRelease;
	callScintilla(SCI_AUTOCSETFILLUPS, 0, reinterpret_cast<LPARAM>(stringcharacterSet.c_str()));
}

/** Should a single item auto-completion list automatically choose the item.
  */
void ScintillaWrapper::AutoCSetChooseSingle(bool chooseSingle)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetChooseSingle\n");
	GILRelease gilRelease;
	callScintilla(SCI_AUTOCSETCHOOSESINGLE, chooseSingle);
}

/** Retrieve whether a single item auto-completion list automatically choose the item.
  */
bool ScintillaWrapper::AutoCGetChooseSingle()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetChooseSingle\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_AUTOCGETCHOOSESINGLE));
}

/** Set whether case is significant when performing auto-completion searches.
  */
void ScintillaWrapper::AutoCSetIgnoreCase(bool ignoreCase)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetIgnoreCase\n");
	GILRelease gilRelease;
	callScintilla(SCI_AUTOCSETIGNORECASE, ignoreCase);
}

/** Retrieve state of ignore case flag.
  */
bool ScintillaWrapper::AutoCGetIgnoreCase()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetIgnoreCase\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_AUTOCGETIGNORECASE));
}

/** Display a list of strings and send notification when user chooses one.
  */
void ScintillaWrapper::UserListShow(int listType, boost::python::object itemList)
{
	DEBUG_TRACE(L"ScintillaWrapper::UserListShow\n");
	std::string stringitemList = getStringFromObject(itemList);
	GILRelease gilRelease;
	callScintilla(SCI_USERLISTSHOW, listType, reinterpret_cast<LPARAM>(stringitemList.c_str()));
}

/** Set whether or not autocompletion is hidden automatically when nothing matches.
  */
void ScintillaWrapper::AutoCSetAutoHide(bool autoHide)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetAutoHide\n");
	GILRelease gilRelease;
	callScintilla(SCI_AUTOCSETAUTOHIDE, autoHide);
}

/** Retrieve whether or not autocompletion is hidden automatically when nothing matches.
  */
bool ScintillaWrapper::AutoCGetAutoHide()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetAutoHide\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_AUTOCGETAUTOHIDE));
}

/** Set whether or not autocompletion deletes any word characters
  * after the inserted text upon completion.
  */
void ScintillaWrapper::AutoCSetDropRestOfWord(bool dropRestOfWord)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetDropRestOfWord\n");
	GILRelease gilRelease;
	callScintilla(SCI_AUTOCSETDROPRESTOFWORD, dropRestOfWord);
}

/** Retrieve whether or not autocompletion deletes any word characters
  * after the inserted text upon completion.
  */
bool ScintillaWrapper::AutoCGetDropRestOfWord()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetDropRestOfWord\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_AUTOCGETDROPRESTOFWORD));
}

/** Register an XPM image for use in autocompletion lists.
  */
void ScintillaWrapper::RegisterImage(int type, boost::python::object xpmData)
{
	DEBUG_TRACE(L"ScintillaWrapper::RegisterImage\n");
	std::string stringxpmData = getStringFromObject(xpmData);
	GILRelease gilRelease;
	callScintilla(SCI_REGISTERIMAGE, type, reinterpret_cast<LPARAM>(stringxpmData.c_str()));
}

/** Clear all the registered XPM images.
  */
void ScintillaWrapper::ClearRegisteredImages()
{
	DEBUG_TRACE(L"ScintillaWrapper::ClearRegisteredImages\n");
	GILRelease gilRelease;
	callScintilla(SCI_CLEARREGISTEREDIMAGES);
}

/** Retrieve the auto-completion list type-separator character.
  */
int ScintillaWrapper::AutoCGetTypeSeparator()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetTypeSeparator\n");
	GILRelease gilRelease;
	return callScintilla(SCI_AUTOCGETTYPESEPARATOR);
}

/** Change the type-separator character in the string setting up an auto-completion list.
  * Default is '?' but can be changed if items contain '?'.
  */
void ScintillaWrapper::AutoCSetTypeSeparator(int separatorCharacter)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetTypeSeparator\n");
	GILRelease gilRelease;
	callScintilla(SCI_AUTOCSETTYPESEPARATOR, separatorCharacter);
}

/** Set the maximum width, in characters, of auto-completion and user lists.
  * Set to 0 to autosize to fit longest item, which is the default.
  */
void ScintillaWrapper::AutoCSetMaxWidth(int characterCount)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetMaxWidth\n");
	GILRelease gilRelease;
	callScintilla(SCI_AUTOCSETMAXWIDTH, characterCount);
}

/** Get the maximum width, in characters, of auto-completion and user lists.
  */
int ScintillaWrapper::AutoCGetMaxWidth()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetMaxWidth\n");
	GILRelease gilRelease;
	return callScintilla(SCI_AUTOCGETMAXWIDTH);
}

/** Set the maximum height, in rows, of auto-completion and user lists.
  * The default is 5 rows.
  */
void ScintillaWrapper::AutoCSetMaxHeight(int rowCount)
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCSetMaxHeight\n");
	GILRelease gilRelease;
	callScintilla(SCI_AUTOCSETMAXHEIGHT, rowCount);
}

/** Set the maximum height, in rows, of auto-completion and user lists.
  */
int ScintillaWrapper::AutoCGetMaxHeight()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetMaxHeight\n");
	GILRelease gilRelease;
	return callScintilla(SCI_AUTOCGETMAXHEIGHT);
}

/** Set the number of spaces used for one level of indentation.
  */
void ScintillaWrapper::SetIndent(int indentSize)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetIndent\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETINDENT, indentSize);
}

/** Retrieve indentation size.
  */
int ScintillaWrapper::GetIndent()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetIndent\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETINDENT);
}

/** Indentation will only use space characters if useTabs is false, otherwise
  * it will use a combination of tabs and spaces.
  */
void ScintillaWrapper::SetUseTabs(bool useTabs)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetUseTabs\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETUSETABS, useTabs);
}

/** Retrieve whether tabs will be used in indentation.
  */
bool ScintillaWrapper::GetUseTabs()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetUseTabs\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETUSETABS));
}

/** Change the indentation of a line to a number of columns.
  */
void ScintillaWrapper::SetLineIndentation(int line, int indentSize)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetLineIndentation\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETLINEINDENTATION, line, indentSize);
}

/** Retrieve the number of columns that a line is indented.
  */
int ScintillaWrapper::GetLineIndentation(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineIndentation\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETLINEINDENTATION, line);
}

/** Retrieve the position before the first non indentation character on a line.
  */
int ScintillaWrapper::GetLineIndentPosition(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineIndentPosition\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETLINEINDENTPOSITION, line);
}

/** Retrieve the column number of a position, taking tab width into account.
  */
int ScintillaWrapper::GetColumn(int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetColumn\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETCOLUMN, pos);
}

/** Show or hide the horizontal scroll bar.
  */
void ScintillaWrapper::SetHScrollBar(bool show)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetHScrollBar\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETHSCROLLBAR, show);
}

/** Is the horizontal scroll bar visible?
  */
bool ScintillaWrapper::GetHScrollBar()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetHScrollBar\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETHSCROLLBAR));
}

/** Show or hide indentation guides.
  */
void ScintillaWrapper::SetIndentationGuides(int indentView)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetIndentationGuides\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETINDENTATIONGUIDES, indentView);
}

/** Are the indentation guides visible?
  */
int ScintillaWrapper::GetIndentationGuides()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetIndentationGuides\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETINDENTATIONGUIDES);
}

/** Set the highlighted indentation guide column.
  * 0 = no highlighted guide.
  */
void ScintillaWrapper::SetHighlightGuide(int column)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetHighlightGuide\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETHIGHLIGHTGUIDE, column);
}

/** Get the highlighted indentation guide column.
  */
int ScintillaWrapper::GetHighlightGuide()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetHighlightGuide\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETHIGHLIGHTGUIDE);
}

/** Get the position after the last visible characters on a line.
  */
int ScintillaWrapper::GetLineEndPosition(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineEndPosition\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETLINEENDPOSITION, line);
}

/** Get the code page used to interpret the bytes of the document as characters.
  */
int ScintillaWrapper::GetCodePage()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCodePage\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETCODEPAGE);
}

/** Get the foreground colour of the caret.
  */
boost::python::tuple ScintillaWrapper::GetCaretFore()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCaretFore\n");
	GILRelease gilRelease;
	int retVal = (int)callScintilla(SCI_GETCARETFORE);
	gilRelease.reacquire();
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** In palette mode?
  */
bool ScintillaWrapper::GetUsePalette()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetUsePalette\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETUSEPALETTE));
}

/** In read-only mode?
  */
bool ScintillaWrapper::GetReadOnly()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetReadOnly\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETREADONLY));
}

/** Sets the position of the caret.
  */
void ScintillaWrapper::SetCurrentPos(int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCurrentPos\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETCURRENTPOS, pos);
}

/** Sets the position that starts the selection - this becomes the anchor.
  */
void ScintillaWrapper::SetSelectionStart(int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelectionStart\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSELECTIONSTART, pos);
}

/** Returns the position at the start of the selection.
  */
int ScintillaWrapper::GetSelectionStart()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionStart\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETSELECTIONSTART);
}

/** Sets the position that ends the selection - this becomes the currentPosition.
  */
void ScintillaWrapper::SetSelectionEnd(int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelectionEnd\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSELECTIONEND, pos);
}

/** Returns the position at the end of the selection.
  */
int ScintillaWrapper::GetSelectionEnd()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionEnd\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETSELECTIONEND);
}

/** Sets the print magnification added to the point size of each style for printing.
  */
void ScintillaWrapper::SetPrintMagnification(int magnification)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetPrintMagnification\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETPRINTMAGNIFICATION, magnification);
}

/** Returns the print magnification.
  */
int ScintillaWrapper::GetPrintMagnification()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetPrintMagnification\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETPRINTMAGNIFICATION);
}

/** Modify colours when printing for clearer printed text.
  */
void ScintillaWrapper::SetPrintColourMode(int mode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetPrintColourMode\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETPRINTCOLOURMODE, mode);
}

/** Returns the print colour mode.
  */
int ScintillaWrapper::GetPrintColourMode()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetPrintColourMode\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETPRINTCOLOURMODE);
}

/** Find some text in the document.
  */
boost::python::object ScintillaWrapper::FindText(int flags, int start, int end, boost::python::object ft)
{
	DEBUG_TRACE(L"ScintillaWrapper::FindText\n");
	std::string search = getStringFromObject(ft);
	GILRelease gilRelease;
	Sci_TextToFind src;
	src.chrg.cpMin = start;
	src.chrg.cpMax = end;
	// We assume  findText won't write to this buffer - it should be const
	src.lpstrText = const_cast<char*>(search.c_str());
	int result = callScintilla(SCI_FINDTEXT, flags, reinterpret_cast<LPARAM>(&src));
	gilRelease.reacquire();
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
int ScintillaWrapper::GetFirstVisibleLine()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetFirstVisibleLine\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETFIRSTVISIBLELINE);
}

/** Retrieve the contents of a line.
  * Returns the length of the line.
  */
boost::python::str ScintillaWrapper::GetLine(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLine\n");
	GILRelease gilRelease;
	int lineCount = callScintilla(SCI_GETLINECOUNT);
	if (line >= lineCount)
	{
		throw out_of_bounds_exception();
	}
	else
	{
		PythonCompatibleStrBuffer result(callScintilla(SCI_LINELENGTH, line));
		callScintilla(SCI_GETLINE, line, reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
		return boost::python::str(result.c_str());
	}
}

/** Returns the number of lines in the document. There is always at least one.
  */
int ScintillaWrapper::GetLineCount()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineCount\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETLINECOUNT);
}

/** Sets the size in pixels of the left margin.
  */
void ScintillaWrapper::SetMarginLeft(int pixelWidth)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMarginLeft\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETMARGINLEFT, 0, pixelWidth);
}

/** Returns the size in pixels of the left margin.
  */
int ScintillaWrapper::GetMarginLeft()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMarginLeft\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETMARGINLEFT);
}

/** Sets the size in pixels of the right margin.
  */
void ScintillaWrapper::SetMarginRight(int pixelWidth)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMarginRight\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETMARGINRIGHT, 0, pixelWidth);
}

/** Returns the size in pixels of the right margin.
  */
int ScintillaWrapper::GetMarginRight()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMarginRight\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETMARGINRIGHT);
}

/** Is the document different from when it was last saved?
  */
bool ScintillaWrapper::GetModify()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetModify\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETMODIFY));
}

/** Select a range of text.
  */
void ScintillaWrapper::SetSel(int start, int end)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSel\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSEL, start, end);
}

/** Retrieve the selected text.
  * Return the length of the text.
  */
boost::python::str ScintillaWrapper::GetSelText()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelText\n");
	GILRelease gilRelease;
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETSELTEXT));
	callScintilla(SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

/** Retrieve a range of text.
  * Return the length of the text.
  */
boost::python::str ScintillaWrapper::GetTextRange(int start, int end)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTextRange\n");
	GILRelease gilRelease;
	Sci_TextRange src;
	if (end == -1)
	{
		end = GetLength();
	}

	if (end < start)
	{
		int temp = start;
		start = end;
		end = temp;
	}
	PythonCompatibleStrBuffer result((end-start) + 1);
	src.chrg.cpMin = start;
	src.chrg.cpMax = end;
	src.lpstrText = *result;
	callScintilla(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&src));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

/** Draw the selection in normal style or with selection highlighted.
  */
void ScintillaWrapper::HideSelection(bool normal)
{
	DEBUG_TRACE(L"ScintillaWrapper::HideSelection\n");
	GILRelease gilRelease;
	callScintilla(SCI_HIDESELECTION, normal);
}

/** Retrieve the x value of the point in the window where a position is displayed.
  */
int ScintillaWrapper::PointXFromPosition(int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::PointXFromPosition\n");
	GILRelease gilRelease;
	return callScintilla(SCI_POINTXFROMPOSITION, 0, pos);
}

/** Retrieve the y value of the point in the window where a position is displayed.
  */
int ScintillaWrapper::PointYFromPosition(int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::PointYFromPosition\n");
	GILRelease gilRelease;
	return callScintilla(SCI_POINTYFROMPOSITION, 0, pos);
}

/** Retrieve the line containing a position.
  */
int ScintillaWrapper::LineFromPosition(int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::LineFromPosition\n");
	GILRelease gilRelease;
	return callScintilla(SCI_LINEFROMPOSITION, pos);
}

/** Retrieve the position at the start of a line.
  */
int ScintillaWrapper::PositionFromLine(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::PositionFromLine\n");
	GILRelease gilRelease;
	return callScintilla(SCI_POSITIONFROMLINE, line);
}

/** Scroll horizontally and vertically.
  */
void ScintillaWrapper::LineScroll(int columns, int lines)
{
	DEBUG_TRACE(L"ScintillaWrapper::LineScroll\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINESCROLL, columns, lines);
}

/** Ensure the caret is visible.
  */
void ScintillaWrapper::ScrollCaret()
{
	DEBUG_TRACE(L"ScintillaWrapper::ScrollCaret\n");
	GILRelease gilRelease;
	callScintilla(SCI_SCROLLCARET);
}

/** Replace the selected text with the argument text.
  */
void ScintillaWrapper::ReplaceSel(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::ReplaceSel\n");
	std::string stringtext = getStringFromObject(text);
	GILRelease gilRelease;
	callScintilla(SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Set to read only or read write.
  */
void ScintillaWrapper::SetReadOnly(bool readOnly)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetReadOnly\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETREADONLY, readOnly);
}

/** Null operation.
  */
void ScintillaWrapper::Null()
{
	DEBUG_TRACE(L"ScintillaWrapper::Null\n");
	GILRelease gilRelease;
	callScintilla(SCI_NULL);
}

/** Will a paste succeed?
  */
bool ScintillaWrapper::CanPaste()
{
	DEBUG_TRACE(L"ScintillaWrapper::CanPaste\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_CANPASTE));
}

/** Are there any undoable actions in the undo history?
  */
bool ScintillaWrapper::CanUndo()
{
	DEBUG_TRACE(L"ScintillaWrapper::CanUndo\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_CANUNDO));
}

/** Delete the undo history.
  */
void ScintillaWrapper::EmptyUndoBuffer()
{
	DEBUG_TRACE(L"ScintillaWrapper::EmptyUndoBuffer\n");
	GILRelease gilRelease;
	callScintilla(SCI_EMPTYUNDOBUFFER);
}

/** Undo one action in the undo history.
  */
void ScintillaWrapper::Undo()
{
	DEBUG_TRACE(L"ScintillaWrapper::Undo\n");
	GILRelease gilRelease;
	callScintilla(SCI_UNDO);
}

/** Cut the selection to the clipboard.
  */
void ScintillaWrapper::Cut()
{
	DEBUG_TRACE(L"ScintillaWrapper::Cut\n");
	GILRelease gilRelease;
	callScintilla(SCI_CUT);
}

/** Copy the selection to the clipboard.
  */
void ScintillaWrapper::Copy()
{
	DEBUG_TRACE(L"ScintillaWrapper::Copy\n");
	GILRelease gilRelease;
	callScintilla(SCI_COPY);
}

/** Paste the contents of the clipboard into the document replacing the selection.
  */
void ScintillaWrapper::Paste()
{
	DEBUG_TRACE(L"ScintillaWrapper::Paste\n");
	GILRelease gilRelease;
	callScintilla(SCI_PASTE);
}

/** Clear the selection.
  */
void ScintillaWrapper::Clear()
{
	DEBUG_TRACE(L"ScintillaWrapper::Clear\n");
	GILRelease gilRelease;
	callScintilla(SCI_CLEAR);
}

/** Replace the contents of the document with the argument text.
  */
void ScintillaWrapper::SetText(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetText\n");
	std::string stringtext = getStringFromObject(text);
	GILRelease gilRelease;
	callScintilla(SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Retrieve all the text in the document.
  * Returns number of characters retrieved.
  */
boost::python::str ScintillaWrapper::GetText()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetText\n");
	GILRelease gilRelease;
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETTEXT) + 1);
	// result.size() does not depend on the order of evaluation here
	//lint -e{864}
	callScintilla(SCI_GETTEXT, result.size(), reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

/** Retrieve the number of characters in the document.
  */
int ScintillaWrapper::GetTextLength()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTextLength\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETTEXTLENGTH);
}

/** Retrieve a pointer to a function that processes messages for this Scintilla.
  */
int ScintillaWrapper::GetDirectFunction()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetDirectFunction\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETDIRECTFUNCTION);
}

/** Retrieve a pointer value to use as the first argument when calling
  * the function returned by GetDirectFunction.
  */
int ScintillaWrapper::GetDirectPointer()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetDirectPointer\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETDIRECTPOINTER);
}

/** Set to overtype (true) or insert mode.
  */
void ScintillaWrapper::SetOvertype(bool overtype)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetOvertype\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETOVERTYPE, overtype);
}

/** Returns true if overtype mode is active otherwise false is returned.
  */
bool ScintillaWrapper::GetOvertype()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetOvertype\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETOVERTYPE));
}

/** Set the width of the insert mode caret.
  */
void ScintillaWrapper::SetCaretWidth(int pixelWidth)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCaretWidth\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETCARETWIDTH, pixelWidth);
}

/** Returns the width of the insert mode caret.
  */
int ScintillaWrapper::GetCaretWidth()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCaretWidth\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETCARETWIDTH);
}

/** Sets the position that starts the target which is used for updating the
  * document without affecting the scroll position.
  */
void ScintillaWrapper::SetTargetStart(int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetTargetStart\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETTARGETSTART, pos);
}

/** Get the position that starts the target.
  */
int ScintillaWrapper::GetTargetStart()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTargetStart\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETTARGETSTART);
}

/** Sets the position that ends the target which is used for updating the
  * document without affecting the scroll position.
  */
void ScintillaWrapper::SetTargetEnd(int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetTargetEnd\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETTARGETEND, pos);
}

/** Get the position that ends the target.
  */
int ScintillaWrapper::GetTargetEnd()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTargetEnd\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETTARGETEND);
}

/** Replace the target text with the argument text.
  * Text is counted so it can contain NULs.
  * Returns the length of the replacement text.
  */
int ScintillaWrapper::ReplaceTarget(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::ReplaceTarget\n");
	std::string s = getStringFromObject(text);
	GILRelease gilRelease;
	return callScintilla(SCI_REPLACETARGET, s.size(), reinterpret_cast<LPARAM>(s.c_str()));
}

/** Replace the target text with the argument text after \d processing.
  * Text is counted so it can contain NULs.
  * Looks for \d where d is between 1 and 9 and replaces these with the strings
  * matched in the last search operation which were surrounded by \( and \).
  * Returns the length of the replacement text including any change
  * caused by processing the \d patterns.
  */
int ScintillaWrapper::ReplaceTargetRE(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::ReplaceTargetRE\n");
	std::string s = getStringFromObject(text);
	GILRelease gilRelease;
	return callScintilla(SCI_REPLACETARGETRE, s.size(), reinterpret_cast<LPARAM>(s.c_str()));
}

/** Search for a counted string in the target and set the target to the found
  * range. Text is counted so it can contain NULs.
  * Returns length of range or -1 for failure in which case target is not moved.
  */
int ScintillaWrapper::SearchInTarget(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::SearchInTarget\n");
	std::string s = getStringFromObject(text);
	GILRelease gilRelease;
	return callScintilla(SCI_SEARCHINTARGET, s.size(), reinterpret_cast<LPARAM>(s.c_str()));
}

/** Set the search flags used by SearchInTarget.
  */
void ScintillaWrapper::SetSearchFlags(int flags)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSearchFlags\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSEARCHFLAGS, flags);
}

/** Get the search flags used by SearchInTarget.
  */
int ScintillaWrapper::GetSearchFlags()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSearchFlags\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETSEARCHFLAGS);
}

/** Show a call tip containing a definition near position pos.
  */
void ScintillaWrapper::CallTipShow(int pos, boost::python::object definition)
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipShow\n");
	std::string stringdefinition = getStringFromObject(definition);
	GILRelease gilRelease;
	callScintilla(SCI_CALLTIPSHOW, pos, reinterpret_cast<LPARAM>(stringdefinition.c_str()));
}

/** Remove the call tip from the screen.
  */
void ScintillaWrapper::CallTipCancel()
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipCancel\n");
	GILRelease gilRelease;
	callScintilla(SCI_CALLTIPCANCEL);
}

/** Is there an active call tip?
  */
bool ScintillaWrapper::CallTipActive()
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipActive\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_CALLTIPACTIVE));
}

/** Retrieve the position where the caret was before displaying the call tip.
  */
int ScintillaWrapper::CallTipPosStart()
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipPosStart\n");
	GILRelease gilRelease;
	return callScintilla(SCI_CALLTIPPOSSTART);
}

/** Highlight a segment of the definition.
  */
void ScintillaWrapper::CallTipSetHlt(int start, int end)
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipSetHlt\n");
	GILRelease gilRelease;
	callScintilla(SCI_CALLTIPSETHLT, start, end);
}

/** Set the background colour for the call tip.
  */
void ScintillaWrapper::CallTipSetBack(boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipSetBack\n");
	COLORREF rgbback = MAKECOLOUR(back);
	GILRelease gilRelease;
	callScintilla(SCI_CALLTIPSETBACK, static_cast<WPARAM>(rgbback));
}

/** Set the foreground colour for the call tip.
  */
void ScintillaWrapper::CallTipSetFore(boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipSetFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	GILRelease gilRelease;
	callScintilla(SCI_CALLTIPSETFORE, static_cast<WPARAM>(rgbfore));
}

/** Set the foreground colour for the highlighted part of the call tip.
  */
void ScintillaWrapper::CallTipSetForeHlt(boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipSetForeHlt\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	GILRelease gilRelease;
	callScintilla(SCI_CALLTIPSETFOREHLT, static_cast<WPARAM>(rgbfore));
}

/** Enable use of STYLE_CALLTIP and set call tip tab size in pixels.
  */
void ScintillaWrapper::CallTipUseStyle(int tabSize)
{
	DEBUG_TRACE(L"ScintillaWrapper::CallTipUseStyle\n");
	GILRelease gilRelease;
	callScintilla(SCI_CALLTIPUSESTYLE, tabSize);
}

/** Find the display line of a document line taking hidden lines into account.
  */
int ScintillaWrapper::VisibleFromDocLine(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::VisibleFromDocLine\n");
	GILRelease gilRelease;
	return callScintilla(SCI_VISIBLEFROMDOCLINE, line);
}

/** Find the document line of a display line taking hidden lines into account.
  */
int ScintillaWrapper::DocLineFromVisible(int lineDisplay)
{
	DEBUG_TRACE(L"ScintillaWrapper::DocLineFromVisible\n");
	GILRelease gilRelease;
	return callScintilla(SCI_DOCLINEFROMVISIBLE, lineDisplay);
}

/** The number of display lines needed to wrap a document line
  */
int ScintillaWrapper::WrapCount(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::WrapCount\n");
	GILRelease gilRelease;
	return callScintilla(SCI_WRAPCOUNT, line);
}

/** Set the fold level of a line.
  * This encodes an integer level along with flags indicating whether the
  * line is a header and whether it is effectively white space.
  */
void ScintillaWrapper::SetFoldLevel(int line, int level)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetFoldLevel\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETFOLDLEVEL, line, level);
}

/** Retrieve the fold level of a line.
  */
int ScintillaWrapper::GetFoldLevel(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetFoldLevel\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETFOLDLEVEL, line);
}

/** Find the last child line of a header line.
  */
int ScintillaWrapper::GetLastChild(int line, int level)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLastChild\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETLASTCHILD, line, level);
}

/** Find the parent line of a child line.
  */
int ScintillaWrapper::GetFoldParent(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetFoldParent\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETFOLDPARENT, line);
}

/** Make a range of lines visible.
  */
void ScintillaWrapper::ShowLines(int lineStart, int lineEnd)
{
	DEBUG_TRACE(L"ScintillaWrapper::ShowLines\n");
	GILRelease gilRelease;
	callScintilla(SCI_SHOWLINES, lineStart, lineEnd);
}

/** Make a range of lines invisible.
  */
void ScintillaWrapper::HideLines(int lineStart, int lineEnd)
{
	DEBUG_TRACE(L"ScintillaWrapper::HideLines\n");
	GILRelease gilRelease;
	callScintilla(SCI_HIDELINES, lineStart, lineEnd);
}

/** Is a line visible?
  */
bool ScintillaWrapper::GetLineVisible(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineVisible\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETLINEVISIBLE, line));
}

/** Show the children of a header line.
  */
void ScintillaWrapper::SetFoldExpanded(int line, bool expanded)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetFoldExpanded\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETFOLDEXPANDED, line, expanded);
}

/** Is a header line expanded?
  */
bool ScintillaWrapper::GetFoldExpanded(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetFoldExpanded\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETFOLDEXPANDED, line));
}

/** Switch a header line between expanded and contracted.
  */
void ScintillaWrapper::ToggleFold(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::ToggleFold\n");
	GILRelease gilRelease;
	callScintilla(SCI_TOGGLEFOLD, line);
}

/** Ensure a particular line is visible by expanding any header line hiding it.
  */
void ScintillaWrapper::EnsureVisible(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::EnsureVisible\n");
	GILRelease gilRelease;
	callScintilla(SCI_ENSUREVISIBLE, line);
}

/** Set some style options for folding.
  */
void ScintillaWrapper::SetFoldFlags(int flags)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetFoldFlags\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETFOLDFLAGS, flags);
}

/** Ensure a particular line is visible by expanding any header line hiding it.
  * Use the currently set visibility policy to determine which range to display.
  */
void ScintillaWrapper::EnsureVisibleEnforcePolicy(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::EnsureVisibleEnforcePolicy\n");
	GILRelease gilRelease;
	callScintilla(SCI_ENSUREVISIBLEENFORCEPOLICY, line);
}

/** Sets whether a tab pressed when caret is within indentation indents.
  */
void ScintillaWrapper::SetTabIndents(bool tabIndents)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetTabIndents\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETTABINDENTS, tabIndents);
}

/** Does a tab pressed when caret is within indentation indent?
  */
bool ScintillaWrapper::GetTabIndents()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTabIndents\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETTABINDENTS));
}

/** Sets whether a backspace pressed when caret is within indentation unindents.
  */
void ScintillaWrapper::SetBackSpaceUnIndents(bool bsUnIndents)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetBackSpaceUnIndents\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETBACKSPACEUNINDENTS, bsUnIndents);
}

/** Does a backspace pressed when caret is within indentation unindent?
  */
bool ScintillaWrapper::GetBackSpaceUnIndents()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetBackSpaceUnIndents\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETBACKSPACEUNINDENTS));
}

/** Sets the time the mouse must sit still to generate a mouse dwell event.
  */
void ScintillaWrapper::SetMouseDwellTime(int periodMilliseconds)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMouseDwellTime\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETMOUSEDWELLTIME, periodMilliseconds);
}

/** Retrieve the time the mouse must sit still to generate a mouse dwell event.
  */
int ScintillaWrapper::GetMouseDwellTime()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMouseDwellTime\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETMOUSEDWELLTIME);
}

/** Get position of start of word.
  */
int ScintillaWrapper::WordStartPosition(int pos, bool onlyWordCharacters)
{
	DEBUG_TRACE(L"ScintillaWrapper::WordStartPosition\n");
	GILRelease gilRelease;
	return callScintilla(SCI_WORDSTARTPOSITION, pos, onlyWordCharacters);
}

/** Get position of end of word.
  */
int ScintillaWrapper::WordEndPosition(int pos, bool onlyWordCharacters)
{
	DEBUG_TRACE(L"ScintillaWrapper::WordEndPosition\n");
	GILRelease gilRelease;
	return callScintilla(SCI_WORDENDPOSITION, pos, onlyWordCharacters);
}

/** Sets whether text is word wrapped.
  */
void ScintillaWrapper::SetWrapMode(int mode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWrapMode\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETWRAPMODE, mode);
}

/** Retrieve whether text is word wrapped.
  */
int ScintillaWrapper::GetWrapMode()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetWrapMode\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETWRAPMODE);
}

/** Set the display mode of visual flags for wrapped lines.
  */
void ScintillaWrapper::SetWrapVisualFlags(int wrapVisualFlags)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWrapVisualFlags\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETWRAPVISUALFLAGS, wrapVisualFlags);
}

/** Retrive the display mode of visual flags for wrapped lines.
  */
int ScintillaWrapper::GetWrapVisualFlags()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetWrapVisualFlags\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETWRAPVISUALFLAGS);
}

/** Set the location of visual flags for wrapped lines.
  */
void ScintillaWrapper::SetWrapVisualFlagsLocation(int wrapVisualFlagsLocation)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWrapVisualFlagsLocation\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETWRAPVISUALFLAGSLOCATION, wrapVisualFlagsLocation);
}

/** Retrive the location of visual flags for wrapped lines.
  */
int ScintillaWrapper::GetWrapVisualFlagsLocation()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetWrapVisualFlagsLocation\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETWRAPVISUALFLAGSLOCATION);
}

/** Set the start indent for wrapped lines.
  */
void ScintillaWrapper::SetWrapStartIndent(int indent)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWrapStartIndent\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETWRAPSTARTINDENT, indent);
}

/** Retrive the start indent for wrapped lines.
  */
int ScintillaWrapper::GetWrapStartIndent()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetWrapStartIndent\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETWRAPSTARTINDENT);
}

/** Sets how wrapped sublines are placed. Default is fixed.
  */
void ScintillaWrapper::SetWrapIndentMode(int mode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWrapIndentMode\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETWRAPINDENTMODE, mode);
}

/** Retrieve how wrapped sublines are placed. Default is fixed.
  */
int ScintillaWrapper::GetWrapIndentMode()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetWrapIndentMode\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETWRAPINDENTMODE);
}

/** Sets the degree of caching of layout information.
  */
void ScintillaWrapper::SetLayoutCache(int mode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetLayoutCache\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETLAYOUTCACHE, mode);
}

/** Retrieve the degree of caching of layout information.
  */
int ScintillaWrapper::GetLayoutCache()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLayoutCache\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETLAYOUTCACHE);
}

/** Sets the document width assumed for scrolling.
  */
void ScintillaWrapper::SetScrollWidth(int pixelWidth)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetScrollWidth\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSCROLLWIDTH, pixelWidth);
}

/** Retrieve the document width assumed for scrolling.
  */
int ScintillaWrapper::GetScrollWidth()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetScrollWidth\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETSCROLLWIDTH);
}

/** Sets whether the maximum width line displayed is used to set scroll width.
  */
void ScintillaWrapper::SetScrollWidthTracking(bool tracking)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetScrollWidthTracking\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSCROLLWIDTHTRACKING, tracking);
}

/** Retrieve whether the scroll width tracks wide lines.
  */
bool ScintillaWrapper::GetScrollWidthTracking()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetScrollWidthTracking\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETSCROLLWIDTHTRACKING));
}

/** Measure the pixel width of some text in a particular style.
  * NUL terminated text argument.
  * Does not handle tab or control characters.
  */
int ScintillaWrapper::TextWidth(int style, boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::TextWidth\n");
	std::string stringtext = getStringFromObject(text);
	GILRelease gilRelease;
	return callScintilla(SCI_TEXTWIDTH, style, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Sets the scroll range so that maximum scroll position has
  * the last line at the bottom of the view (default).
  * Setting this to false allows scrolling one page below the last line.
  */
void ScintillaWrapper::SetEndAtLastLine(bool endAtLastLine)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetEndAtLastLine\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETENDATLASTLINE, endAtLastLine);
}

/** Retrieve whether the maximum scroll position has the last
  * line at the bottom of the view.
  */
bool ScintillaWrapper::GetEndAtLastLine()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetEndAtLastLine\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETENDATLASTLINE));
}

/** Retrieve the height of a particular line of text in pixels.
  */
int ScintillaWrapper::TextHeight(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::TextHeight\n");
	GILRelease gilRelease;
	return callScintilla(SCI_TEXTHEIGHT, line);
}

/** Show or hide the vertical scroll bar.
  */
void ScintillaWrapper::SetVScrollBar(bool show)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetVScrollBar\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETVSCROLLBAR, show);
}

/** Is the vertical scroll bar visible?
  */
bool ScintillaWrapper::GetVScrollBar()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetVScrollBar\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETVSCROLLBAR));
}

/** Append a string to the end of the document without changing the selection.
  */
int ScintillaWrapper::AppendText(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::AppendText\n");
	std::string s = getStringFromObject(text);
	GILRelease gilRelease;
	return callScintilla(SCI_APPENDTEXT, s.size(), reinterpret_cast<LPARAM>(s.c_str()));
}

/** Is drawing done in two phases with backgrounds drawn before faoregrounds?
  */
bool ScintillaWrapper::GetTwoPhaseDraw()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTwoPhaseDraw\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETTWOPHASEDRAW));
}

/** In twoPhaseDraw mode, drawing is performed in two phases, first the background
  * and then the foreground. This avoids chopping off characters that overlap the next run.
  */
void ScintillaWrapper::SetTwoPhaseDraw(bool twoPhase)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetTwoPhaseDraw\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETTWOPHASEDRAW, twoPhase);
}

/** Choose the quality level for text from the FontQuality enumeration.
  */
void ScintillaWrapper::SetFontQuality(int fontQuality)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetFontQuality\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETFONTQUALITY, fontQuality);
}

/** Retrieve the quality level for text.
  */
int ScintillaWrapper::GetFontQuality()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetFontQuality\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETFONTQUALITY);
}

/** Scroll so that a display line is at the top of the display.
  */
void ScintillaWrapper::SetFirstVisibleLine(int lineDisplay)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetFirstVisibleLine\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETFIRSTVISIBLELINE, lineDisplay);
}

/** Change the effect of pasting when there are multiple selections.
  */
void ScintillaWrapper::SetMultiPaste(int multiPaste)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMultiPaste\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETMULTIPASTE, multiPaste);
}

/** Retrieve the effect of pasting when there are multiple selections..
  */
int ScintillaWrapper::GetMultiPaste()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMultiPaste\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETMULTIPASTE);
}

/** Retrieve the value of a tag from a regular expression search.
  */
boost::python::str ScintillaWrapper::GetTag()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetTag\n");
	GILRelease gilRelease;
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETTAG));
	callScintilla(SCI_GETTAG, 0, reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

/** Make the target range start and end be the same as the selection range start and end.
  */
void ScintillaWrapper::TargetFromSelection()
{
	DEBUG_TRACE(L"ScintillaWrapper::TargetFromSelection\n");
	GILRelease gilRelease;
	callScintilla(SCI_TARGETFROMSELECTION);
}

/** Join the lines in the target.
  */
void ScintillaWrapper::LinesJoin()
{
	DEBUG_TRACE(L"ScintillaWrapper::LinesJoin\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINESJOIN);
}

/** Split the lines in the target into lines that are less wide than pixelWidth
  * where possible.
  */
void ScintillaWrapper::LinesSplit(int pixelWidth)
{
	DEBUG_TRACE(L"ScintillaWrapper::LinesSplit\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINESSPLIT, pixelWidth);
}

/** Set the colours used as a chequerboard pattern in the fold margin
  */
void ScintillaWrapper::SetFoldMarginColour(bool useSetting, boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetFoldMarginColour\n");
	COLORREF rgbback = MAKECOLOUR(back);
	GILRelease gilRelease;
	callScintilla(SCI_SETFOLDMARGINCOLOUR, useSetting, static_cast<LPARAM>(rgbback));
}

/** Set the colours used as a chequerboard pattern in the fold margin
  */
void ScintillaWrapper::SetFoldMarginHiColour(bool useSetting, boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetFoldMarginHiColour\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	GILRelease gilRelease;
	callScintilla(SCI_SETFOLDMARGINHICOLOUR, useSetting, static_cast<LPARAM>(rgbfore));
}

/** Move caret down one line.
  */
void ScintillaWrapper::LineDown()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineDown\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINEDOWN);
}

/** Move caret down one line extending selection to new caret position.
  */
void ScintillaWrapper::LineDownExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineDownExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINEDOWNEXTEND);
}

/** Move caret up one line.
  */
void ScintillaWrapper::LineUp()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineUp\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINEUP);
}

/** Move caret up one line extending selection to new caret position.
  */
void ScintillaWrapper::LineUpExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineUpExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINEUPEXTEND);
}

/** Move caret left one character.
  */
void ScintillaWrapper::CharLeft()
{
	DEBUG_TRACE(L"ScintillaWrapper::CharLeft\n");
	GILRelease gilRelease;
	callScintilla(SCI_CHARLEFT);
}

/** Move caret left one character extending selection to new caret position.
  */
void ScintillaWrapper::CharLeftExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::CharLeftExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_CHARLEFTEXTEND);
}

/** Move caret right one character.
  */
void ScintillaWrapper::CharRight()
{
	DEBUG_TRACE(L"ScintillaWrapper::CharRight\n");
	GILRelease gilRelease;
	callScintilla(SCI_CHARRIGHT);
}

/** Move caret right one character extending selection to new caret position.
  */
void ScintillaWrapper::CharRightExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::CharRightExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_CHARRIGHTEXTEND);
}

/** Move caret left one word.
  */
void ScintillaWrapper::WordLeft()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordLeft\n");
	GILRelease gilRelease;
	callScintilla(SCI_WORDLEFT);
}

/** Move caret left one word extending selection to new caret position.
  */
void ScintillaWrapper::WordLeftExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordLeftExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_WORDLEFTEXTEND);
}

/** Move caret right one word.
  */
void ScintillaWrapper::WordRight()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordRight\n");
	GILRelease gilRelease;
	callScintilla(SCI_WORDRIGHT);
}

/** Move caret right one word extending selection to new caret position.
  */
void ScintillaWrapper::WordRightExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordRightExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_WORDRIGHTEXTEND);
}

/** Move caret to first position on line.
  */
void ScintillaWrapper::Home()
{
	DEBUG_TRACE(L"ScintillaWrapper::Home\n");
	GILRelease gilRelease;
	callScintilla(SCI_HOME);
}

/** Move caret to first position on line extending selection to new caret position.
  */
void ScintillaWrapper::HomeExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::HomeExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_HOMEEXTEND);
}

/** Move caret to last position on line.
  */
void ScintillaWrapper::LineEnd()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineEnd\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINEEND);
}

/** Move caret to last position on line extending selection to new caret position.
  */
void ScintillaWrapper::LineEndExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineEndExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINEENDEXTEND);
}

/** Move caret to first position in document.
  */
void ScintillaWrapper::DocumentStart()
{
	DEBUG_TRACE(L"ScintillaWrapper::DocumentStart\n");
	GILRelease gilRelease;
	callScintilla(SCI_DOCUMENTSTART);
}

/** Move caret to first position in document extending selection to new caret position.
  */
void ScintillaWrapper::DocumentStartExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::DocumentStartExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_DOCUMENTSTARTEXTEND);
}

/** Move caret to last position in document.
  */
void ScintillaWrapper::DocumentEnd()
{
	DEBUG_TRACE(L"ScintillaWrapper::DocumentEnd\n");
	GILRelease gilRelease;
	callScintilla(SCI_DOCUMENTEND);
}

/** Move caret to last position in document extending selection to new caret position.
  */
void ScintillaWrapper::DocumentEndExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::DocumentEndExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_DOCUMENTENDEXTEND);
}

/** Move caret one page up.
  */
void ScintillaWrapper::PageUp()
{
	DEBUG_TRACE(L"ScintillaWrapper::PageUp\n");
	GILRelease gilRelease;
	callScintilla(SCI_PAGEUP);
}

/** Move caret one page up extending selection to new caret position.
  */
void ScintillaWrapper::PageUpExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::PageUpExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_PAGEUPEXTEND);
}

/** Move caret one page down.
  */
void ScintillaWrapper::PageDown()
{
	DEBUG_TRACE(L"ScintillaWrapper::PageDown\n");
	GILRelease gilRelease;
	callScintilla(SCI_PAGEDOWN);
}

/** Move caret one page down extending selection to new caret position.
  */
void ScintillaWrapper::PageDownExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::PageDownExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_PAGEDOWNEXTEND);
}

/** Switch from insert to overtype mode or the reverse.
  */
void ScintillaWrapper::EditToggleOvertype()
{
	DEBUG_TRACE(L"ScintillaWrapper::EditToggleOvertype\n");
	GILRelease gilRelease;
	callScintilla(SCI_EDITTOGGLEOVERTYPE);
}

/** Cancel any modes such as call tip or auto-completion list display.
  */
void ScintillaWrapper::Cancel()
{
	DEBUG_TRACE(L"ScintillaWrapper::Cancel\n");
	GILRelease gilRelease;
	callScintilla(SCI_CANCEL);
}

/** Delete the selection or if no selection, the character before the caret.
  */
void ScintillaWrapper::DeleteBack()
{
	DEBUG_TRACE(L"ScintillaWrapper::DeleteBack\n");
	GILRelease gilRelease;
	callScintilla(SCI_DELETEBACK);
}

/** If selection is empty or all on one line replace the selection with a tab character.
  * If more than one line selected, indent the lines.
  */
void ScintillaWrapper::Tab()
{
	DEBUG_TRACE(L"ScintillaWrapper::Tab\n");
	GILRelease gilRelease;
	callScintilla(SCI_TAB);
}

/** Dedent the selected lines.
  */
void ScintillaWrapper::BackTab()
{
	DEBUG_TRACE(L"ScintillaWrapper::BackTab\n");
	GILRelease gilRelease;
	callScintilla(SCI_BACKTAB);
}

/** Insert a new line, may use a CRLF, CR or LF depending on EOL mode.
  */
void ScintillaWrapper::NewLine()
{
	DEBUG_TRACE(L"ScintillaWrapper::NewLine\n");
	GILRelease gilRelease;
	callScintilla(SCI_NEWLINE);
}

/** Insert a Form Feed character.
  */
void ScintillaWrapper::FormFeed()
{
	DEBUG_TRACE(L"ScintillaWrapper::FormFeed\n");
	GILRelease gilRelease;
	callScintilla(SCI_FORMFEED);
}

/** Move caret to before first visible character on line.
  * If already there move to first character on line.
  */
void ScintillaWrapper::VCHome()
{
	DEBUG_TRACE(L"ScintillaWrapper::VCHome\n");
	GILRelease gilRelease;
	callScintilla(SCI_VCHOME);
}

/** Like VCHome but extending selection to new caret position.
  */
void ScintillaWrapper::VCHomeExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::VCHomeExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_VCHOMEEXTEND);
}

/** Magnify the displayed text by increasing the sizes by 1 point.
  */
void ScintillaWrapper::ZoomIn()
{
	DEBUG_TRACE(L"ScintillaWrapper::ZoomIn\n");
	GILRelease gilRelease;
	callScintilla(SCI_ZOOMIN);
}

/** Make the displayed text smaller by decreasing the sizes by 1 point.
  */
void ScintillaWrapper::ZoomOut()
{
	DEBUG_TRACE(L"ScintillaWrapper::ZoomOut\n");
	GILRelease gilRelease;
	callScintilla(SCI_ZOOMOUT);
}

/** Delete the word to the left of the caret.
  */
void ScintillaWrapper::DelWordLeft()
{
	DEBUG_TRACE(L"ScintillaWrapper::DelWordLeft\n");
	GILRelease gilRelease;
	callScintilla(SCI_DELWORDLEFT);
}

/** Delete the word to the right of the caret.
  */
void ScintillaWrapper::DelWordRight()
{
	DEBUG_TRACE(L"ScintillaWrapper::DelWordRight\n");
	GILRelease gilRelease;
	callScintilla(SCI_DELWORDRIGHT);
}

/** Delete the word to the right of the caret, but not the trailing non-word characters.
  */
void ScintillaWrapper::DelWordRightEnd()
{
	DEBUG_TRACE(L"ScintillaWrapper::DelWordRightEnd\n");
	GILRelease gilRelease;
	callScintilla(SCI_DELWORDRIGHTEND);
}

/** Cut the line containing the caret.
  */
void ScintillaWrapper::LineCut()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineCut\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINECUT);
}

/** Delete the line containing the caret.
  */
void ScintillaWrapper::LineDelete()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineDelete\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINEDELETE);
}

/** Switch the current line with the previous.
  */
void ScintillaWrapper::LineTranspose()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineTranspose\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINETRANSPOSE);
}

/** Duplicate the current line.
  */
void ScintillaWrapper::LineDuplicate()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineDuplicate\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINEDUPLICATE);
}

/** Transform the selection to lower case.
  */
void ScintillaWrapper::LowerCase()
{
	DEBUG_TRACE(L"ScintillaWrapper::LowerCase\n");
	GILRelease gilRelease;
	callScintilla(SCI_LOWERCASE);
}

/** Transform the selection to upper case.
  */
void ScintillaWrapper::UpperCase()
{
	DEBUG_TRACE(L"ScintillaWrapper::UpperCase\n");
	GILRelease gilRelease;
	callScintilla(SCI_UPPERCASE);
}

/** Scroll the document down, keeping the caret visible.
  */
void ScintillaWrapper::LineScrollDown()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineScrollDown\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINESCROLLDOWN);
}

/** Scroll the document up, keeping the caret visible.
  */
void ScintillaWrapper::LineScrollUp()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineScrollUp\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINESCROLLUP);
}

/** Delete the selection or if no selection, the character before the caret.
  * Will not delete the character before at the start of a line.
  */
void ScintillaWrapper::DeleteBackNotLine()
{
	DEBUG_TRACE(L"ScintillaWrapper::DeleteBackNotLine\n");
	GILRelease gilRelease;
	callScintilla(SCI_DELETEBACKNOTLINE);
}

/** Move caret to first position on display line.
  */
void ScintillaWrapper::HomeDisplay()
{
	DEBUG_TRACE(L"ScintillaWrapper::HomeDisplay\n");
	GILRelease gilRelease;
	callScintilla(SCI_HOMEDISPLAY);
}

/** Move caret to first position on display line extending selection to
  * new caret position.
  */
void ScintillaWrapper::HomeDisplayExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::HomeDisplayExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_HOMEDISPLAYEXTEND);
}

/** Move caret to last position on display line.
  */
void ScintillaWrapper::LineEndDisplay()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineEndDisplay\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINEENDDISPLAY);
}

/** Move caret to last position on display line extending selection to new
  * caret position.
  */
void ScintillaWrapper::LineEndDisplayExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineEndDisplayExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINEENDDISPLAYEXTEND);
}

/** These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
  * except they behave differently when word-wrap is enabled:
  * They go first to the start / end of the display line, like (Home|LineEnd)Display
  * The difference is that, the cursor is already at the point, it goes on to the start
  * or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
  */
void ScintillaWrapper::HomeWrap()
{
	DEBUG_TRACE(L"ScintillaWrapper::HomeWrap\n");
	GILRelease gilRelease;
	callScintilla(SCI_HOMEWRAP);
}

/** These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
  * except they behave differently when word-wrap is enabled:
  * They go first to the start / end of the display line, like (Home|LineEnd)Display
  * The difference is that, the cursor is already at the point, it goes on to the start
  * or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
  */
void ScintillaWrapper::HomeWrapExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::HomeWrapExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_HOMEWRAPEXTEND);
}

/** These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
  * except they behave differently when word-wrap is enabled:
  * They go first to the start / end of the display line, like (Home|LineEnd)Display
  * The difference is that, the cursor is already at the point, it goes on to the start
  * or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
  */
void ScintillaWrapper::LineEndWrap()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineEndWrap\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINEENDWRAP);
}

/** These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
  * except they behave differently when word-wrap is enabled:
  * They go first to the start / end of the display line, like (Home|LineEnd)Display
  * The difference is that, the cursor is already at the point, it goes on to the start
  * or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
  */
void ScintillaWrapper::LineEndWrapExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineEndWrapExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINEENDWRAPEXTEND);
}

/** These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
  * except they behave differently when word-wrap is enabled:
  * They go first to the start / end of the display line, like (Home|LineEnd)Display
  * The difference is that, the cursor is already at the point, it goes on to the start
  * or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
  */
void ScintillaWrapper::VCHomeWrap()
{
	DEBUG_TRACE(L"ScintillaWrapper::VCHomeWrap\n");
	GILRelease gilRelease;
	callScintilla(SCI_VCHOMEWRAP);
}

/** These are like their namesakes Home(Extend)?, LineEnd(Extend)?, VCHome(Extend)?
  * except they behave differently when word-wrap is enabled:
  * They go first to the start / end of the display line, like (Home|LineEnd)Display
  * The difference is that, the cursor is already at the point, it goes on to the start
  * or end of the document line, as appropriate for (Home|LineEnd|VCHome)(Extend)?.
  */
void ScintillaWrapper::VCHomeWrapExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::VCHomeWrapExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_VCHOMEWRAPEXTEND);
}

/** Copy the line containing the caret.
  */
void ScintillaWrapper::LineCopy()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineCopy\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINECOPY);
}

/** Move the caret inside current view if it's not there already.
  */
void ScintillaWrapper::MoveCaretInsideView()
{
	DEBUG_TRACE(L"ScintillaWrapper::MoveCaretInsideView\n");
	GILRelease gilRelease;
	callScintilla(SCI_MOVECARETINSIDEVIEW);
}

/** How many characters are on a line, including end of line characters?
  */
int ScintillaWrapper::LineLength(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::LineLength\n");
	GILRelease gilRelease;
	return callScintilla(SCI_LINELENGTH, line);
}

/** Highlight the characters at two positions.
  */
void ScintillaWrapper::BraceHighlight(int pos1, int pos2)
{
	DEBUG_TRACE(L"ScintillaWrapper::BraceHighlight\n");
	GILRelease gilRelease;
	callScintilla(SCI_BRACEHIGHLIGHT, pos1, pos2);
}

/** Highlight the character at a position indicating there is no matching brace.
  */
void ScintillaWrapper::BraceBadLight(int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::BraceBadLight\n");
	GILRelease gilRelease;
	callScintilla(SCI_BRACEBADLIGHT, pos);
}

/** Find the position of a matching brace or INVALID_POSITION if no match.
  */
int ScintillaWrapper::BraceMatch(int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::BraceMatch\n");
	GILRelease gilRelease;
	return callScintilla(SCI_BRACEMATCH, pos);
}

/** Are the end of line characters visible?
  */
bool ScintillaWrapper::GetViewEOL()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetViewEOL\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETVIEWEOL));
}

/** Make the end of line characters visible or invisible.
  */
void ScintillaWrapper::SetViewEOL(bool visible)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetViewEOL\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETVIEWEOL, visible);
}

/** Retrieve a pointer to the document object.
  */
int ScintillaWrapper::GetDocPointer()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetDocPointer\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETDOCPOINTER);
}

/** Change the document object used.
  */
void ScintillaWrapper::SetDocPointer(int pointer)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetDocPointer\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETDOCPOINTER, 0, pointer);
}

/** Set which document modification events are sent to the container.
  */
void ScintillaWrapper::SetModEventMask(int mask)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetModEventMask\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETMODEVENTMASK, mask);
}

/** Retrieve the column number which text should be kept within.
  */
int ScintillaWrapper::GetEdgeColumn()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetEdgeColumn\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETEDGECOLUMN);
}

/** Set the column number of the edge.
  * If text goes past the edge then it is highlighted.
  */
void ScintillaWrapper::SetEdgeColumn(int column)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetEdgeColumn\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETEDGECOLUMN, column);
}

/** Retrieve the edge highlight mode.
  */
int ScintillaWrapper::GetEdgeMode()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetEdgeMode\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETEDGEMODE);
}

/** The edge may be displayed by a line (EDGE_LINE) or by highlighting text that
  * goes beyond it (EDGE_BACKGROUND) or not displayed at all (EDGE_NONE).
  */
void ScintillaWrapper::SetEdgeMode(int mode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetEdgeMode\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETEDGEMODE, mode);
}

/** Retrieve the colour used in edge indication.
  */
boost::python::tuple ScintillaWrapper::GetEdgeColour()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetEdgeColour\n");
	GILRelease gilRelease;
	int retVal = (int)callScintilla(SCI_GETEDGECOLOUR);
	gilRelease.reacquire();
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** Change the colour used in edge indication.
  */
void ScintillaWrapper::SetEdgeColour(boost::python::tuple edgeColour)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetEdgeColour\n");
	COLORREF rgbedgeColour = MAKECOLOUR(edgeColour);
	GILRelease gilRelease;
	callScintilla(SCI_SETEDGECOLOUR, static_cast<WPARAM>(rgbedgeColour));
}

/** Sets the current caret position to be the search anchor.
  */
void ScintillaWrapper::SearchAnchor()
{
	DEBUG_TRACE(L"ScintillaWrapper::SearchAnchor\n");
	GILRelease gilRelease;
	callScintilla(SCI_SEARCHANCHOR);
}

/** Find some text starting at the search anchor.
  * Does not ensure the selection is visible.
  */
int ScintillaWrapper::SearchNext(int flags, boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::SearchNext\n");
	std::string stringtext = getStringFromObject(text);
	GILRelease gilRelease;
	return callScintilla(SCI_SEARCHNEXT, flags, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Find some text starting at the search anchor and moving backwards.
  * Does not ensure the selection is visible.
  */
int ScintillaWrapper::SearchPrev(int flags, boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::SearchPrev\n");
	std::string stringtext = getStringFromObject(text);
	GILRelease gilRelease;
	return callScintilla(SCI_SEARCHPREV, flags, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Retrieves the number of lines completely visible.
  */
int ScintillaWrapper::LinesOnScreen()
{
	DEBUG_TRACE(L"ScintillaWrapper::LinesOnScreen\n");
	GILRelease gilRelease;
	return callScintilla(SCI_LINESONSCREEN);
}

/** Set whether a pop up menu is displayed automatically when the user presses
  * the wrong mouse button.
  */
void ScintillaWrapper::UsePopUp(bool allowPopUp)
{
	DEBUG_TRACE(L"ScintillaWrapper::UsePopUp\n");
	GILRelease gilRelease;
	callScintilla(SCI_USEPOPUP, allowPopUp);
}

/** Is the selection rectangular? The alternative is the more common stream selection.
  */
bool ScintillaWrapper::SelectionIsRectangle()
{
	DEBUG_TRACE(L"ScintillaWrapper::SelectionIsRectangle\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_SELECTIONISRECTANGLE));
}

/** Set the zoom level. This number of points is added to the size of all fonts.
  * It may be positive to magnify or negative to reduce.
  */
void ScintillaWrapper::SetZoom(int zoom)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetZoom\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETZOOM, zoom);
}

/** Retrieve the zoom level.
  */
int ScintillaWrapper::GetZoom()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetZoom\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETZOOM);
}

/** Create a new document object.
  * Starts with reference count of 1 and not selected into editor.
  */
int ScintillaWrapper::CreateDocument()
{
	DEBUG_TRACE(L"ScintillaWrapper::CreateDocument\n");
	GILRelease gilRelease;
	return callScintilla(SCI_CREATEDOCUMENT);
}

/** Extend life of document.
  */
void ScintillaWrapper::AddRefDocument(int doc)
{
	DEBUG_TRACE(L"ScintillaWrapper::AddRefDocument\n");
	GILRelease gilRelease;
	callScintilla(SCI_ADDREFDOCUMENT, 0, doc);
}

/** Release a reference to the document, deleting document if it fades to black.
  */
void ScintillaWrapper::ReleaseDocument(int doc)
{
	DEBUG_TRACE(L"ScintillaWrapper::ReleaseDocument\n");
	GILRelease gilRelease;
	callScintilla(SCI_RELEASEDOCUMENT, 0, doc);
}

/** Get which document modification events are sent to the container.
  */
int ScintillaWrapper::GetModEventMask()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetModEventMask\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETMODEVENTMASK);
}

/** Change internal focus flag.
  */
void ScintillaWrapper::SetFocus(bool focus)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetFocus\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETFOCUS, focus);
}

/** Get internal focus flag.
  */
bool ScintillaWrapper::GetFocus()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetFocus\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETFOCUS));
}

/** Change error status - 0 = OK.
  */
void ScintillaWrapper::SetStatus(int statusCode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetStatus\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSTATUS, statusCode);
}

/** Get error status.
  */
int ScintillaWrapper::GetStatus()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetStatus\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETSTATUS);
}

/** Set whether the mouse is captured when its button is pressed.
  */
void ScintillaWrapper::SetMouseDownCaptures(bool captures)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMouseDownCaptures\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETMOUSEDOWNCAPTURES, captures);
}

/** Get whether mouse gets captured.
  */
bool ScintillaWrapper::GetMouseDownCaptures()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMouseDownCaptures\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETMOUSEDOWNCAPTURES));
}

/** Sets the cursor to one of the SC_CURSOR* values.
  */
void ScintillaWrapper::SetCursor(int cursorType)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCursor\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETCURSOR, cursorType);
}

/** Get cursor type.
  */
int ScintillaWrapper::GetCursor()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCursor\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETCURSOR);
}

/** Change the way control characters are displayed:
  * If symbol is < 32, keep the drawn way, else, use the given character.
  */
void ScintillaWrapper::SetControlCharSymbol(int symbol)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetControlCharSymbol\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETCONTROLCHARSYMBOL, symbol);
}

/** Get the way control characters are displayed.
  */
int ScintillaWrapper::GetControlCharSymbol()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetControlCharSymbol\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETCONTROLCHARSYMBOL);
}

/** Move to the previous change in capitalisation.
  */
void ScintillaWrapper::WordPartLeft()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordPartLeft\n");
	GILRelease gilRelease;
	callScintilla(SCI_WORDPARTLEFT);
}

/** Move to the previous change in capitalisation extending selection
  * to new caret position.
  */
void ScintillaWrapper::WordPartLeftExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordPartLeftExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_WORDPARTLEFTEXTEND);
}

/** Move to the change next in capitalisation.
  */
void ScintillaWrapper::WordPartRight()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordPartRight\n");
	GILRelease gilRelease;
	callScintilla(SCI_WORDPARTRIGHT);
}

/** Move to the next change in capitalisation extending selection
  * to new caret position.
  */
void ScintillaWrapper::WordPartRightExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordPartRightExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_WORDPARTRIGHTEXTEND);
}

/** Set the way the display area is determined when a particular line
  * is to be moved to by Find, FindNext, GotoLine, etc.
  */
void ScintillaWrapper::SetVisiblePolicy(int visiblePolicy, int visibleSlop)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetVisiblePolicy\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETVISIBLEPOLICY, visiblePolicy, visibleSlop);
}

/** Delete back from the current position to the start of the line.
  */
void ScintillaWrapper::DelLineLeft()
{
	DEBUG_TRACE(L"ScintillaWrapper::DelLineLeft\n");
	GILRelease gilRelease;
	callScintilla(SCI_DELLINELEFT);
}

/** Delete forwards from the current position to the end of the line.
  */
void ScintillaWrapper::DelLineRight()
{
	DEBUG_TRACE(L"ScintillaWrapper::DelLineRight\n");
	GILRelease gilRelease;
	callScintilla(SCI_DELLINERIGHT);
}

/** Get and Set the xOffset (ie, horizonal scroll position).
  */
void ScintillaWrapper::SetXOffset(int newOffset)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetXOffset\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETXOFFSET, newOffset);
}

/** Get and Set the xOffset (ie, horizonal scroll position).
  */
int ScintillaWrapper::GetXOffset()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetXOffset\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETXOFFSET);
}

/** Set the last x chosen value to be the caret x position.
  */
void ScintillaWrapper::ChooseCaretX()
{
	DEBUG_TRACE(L"ScintillaWrapper::ChooseCaretX\n");
	GILRelease gilRelease;
	callScintilla(SCI_CHOOSECARETX);
}

/** Set the focus to this Scintilla widget.
  */
void ScintillaWrapper::GrabFocus()
{
	DEBUG_TRACE(L"ScintillaWrapper::GrabFocus\n");
	GILRelease gilRelease;
	callScintilla(SCI_GRABFOCUS);
}

/** Set the way the caret is kept visible when going sideway.
  * The exclusion zone is given in pixels.
  */
void ScintillaWrapper::SetXCaretPolicy(int caretPolicy, int caretSlop)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetXCaretPolicy\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETXCARETPOLICY, caretPolicy, caretSlop);
}

/** Set the way the line the caret is on is kept visible.
  * The exclusion zone is given in lines.
  */
void ScintillaWrapper::SetYCaretPolicy(int caretPolicy, int caretSlop)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetYCaretPolicy\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETYCARETPOLICY, caretPolicy, caretSlop);
}

/** Set printing to line wrapped (SC_WRAP_WORD) or not line wrapped (SC_WRAP_NONE).
  */
void ScintillaWrapper::SetPrintWrapMode(int mode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetPrintWrapMode\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETPRINTWRAPMODE, mode);
}

/** Is printing line wrapped?
  */
int ScintillaWrapper::GetPrintWrapMode()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetPrintWrapMode\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETPRINTWRAPMODE);
}

/** Set a fore colour for active hotspots.
  */
void ScintillaWrapper::SetHotspotActiveFore(bool useSetting, boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetHotspotActiveFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	GILRelease gilRelease;
	callScintilla(SCI_SETHOTSPOTACTIVEFORE, useSetting, static_cast<LPARAM>(rgbfore));
}

/** Get the fore colour for active hotspots.
  */
boost::python::tuple ScintillaWrapper::GetHotspotActiveFore()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetHotspotActiveFore\n");
	GILRelease gilRelease;
	int retVal = (int)callScintilla(SCI_GETHOTSPOTACTIVEFORE);
	gilRelease.reacquire();
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** Set a back colour for active hotspots.
  */
void ScintillaWrapper::SetHotspotActiveBack(bool useSetting, boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetHotspotActiveBack\n");
	COLORREF rgbback = MAKECOLOUR(back);
	GILRelease gilRelease;
	callScintilla(SCI_SETHOTSPOTACTIVEBACK, useSetting, static_cast<LPARAM>(rgbback));
}

/** Get the back colour for active hotspots.
  */
boost::python::tuple ScintillaWrapper::GetHotspotActiveBack()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetHotspotActiveBack\n");
	GILRelease gilRelease;
	int retVal = (int)callScintilla(SCI_GETHOTSPOTACTIVEBACK);
	gilRelease.reacquire();
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** Enable / Disable underlining active hotspots.
  */
void ScintillaWrapper::SetHotspotActiveUnderline(bool underline)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetHotspotActiveUnderline\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETHOTSPOTACTIVEUNDERLINE, underline);
}

/** Get whether underlining for active hotspots.
  */
bool ScintillaWrapper::GetHotspotActiveUnderline()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetHotspotActiveUnderline\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETHOTSPOTACTIVEUNDERLINE));
}

/** Limit hotspots to single line so hotspots on two lines don't merge.
  */
void ScintillaWrapper::SetHotspotSingleLine(bool singleLine)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetHotspotSingleLine\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETHOTSPOTSINGLELINE, singleLine);
}

/** Get the HotspotSingleLine property
  */
bool ScintillaWrapper::GetHotspotSingleLine()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetHotspotSingleLine\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETHOTSPOTSINGLELINE));
}

/** Move caret between paragraphs (delimited by empty lines).
  */
void ScintillaWrapper::ParaDown()
{
	DEBUG_TRACE(L"ScintillaWrapper::ParaDown\n");
	GILRelease gilRelease;
	callScintilla(SCI_PARADOWN);
}

/** Move caret between paragraphs (delimited by empty lines).
  */
void ScintillaWrapper::ParaDownExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::ParaDownExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_PARADOWNEXTEND);
}

/** Move caret between paragraphs (delimited by empty lines).
  */
void ScintillaWrapper::ParaUp()
{
	DEBUG_TRACE(L"ScintillaWrapper::ParaUp\n");
	GILRelease gilRelease;
	callScintilla(SCI_PARAUP);
}

/** Move caret between paragraphs (delimited by empty lines).
  */
void ScintillaWrapper::ParaUpExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::ParaUpExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_PARAUPEXTEND);
}

/** Given a valid document position, return the previous position taking code
  * page into account. Returns 0 if passed 0.
  */
int ScintillaWrapper::PositionBefore(int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::PositionBefore\n");
	GILRelease gilRelease;
	return callScintilla(SCI_POSITIONBEFORE, pos);
}

/** Given a valid document position, return the next position taking code
  * page into account. Maximum value returned is the last position in the document.
  */
int ScintillaWrapper::PositionAfter(int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::PositionAfter\n");
	GILRelease gilRelease;
	return callScintilla(SCI_POSITIONAFTER, pos);
}

/** Copy a range of text to the clipboard. Positions are clipped into the document.
  */
void ScintillaWrapper::CopyRange(int start, int end)
{
	DEBUG_TRACE(L"ScintillaWrapper::CopyRange\n");
	GILRelease gilRelease;
	callScintilla(SCI_COPYRANGE, start, end);
}

/** Copy argument text to the clipboard.
  */
int ScintillaWrapper::CopyText(boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::CopyText\n");
	std::string s = getStringFromObject(text);
	GILRelease gilRelease;
	return callScintilla(SCI_COPYTEXT, s.size(), reinterpret_cast<LPARAM>(s.c_str()));
}

/** Set the selection mode to stream (SC_SEL_STREAM) or rectangular (SC_SEL_RECTANGLE/SC_SEL_THIN) or
  * by lines (SC_SEL_LINES).
  */
void ScintillaWrapper::SetSelectionMode(int mode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelectionMode\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSELECTIONMODE, mode);
}

/** Get the mode of the current selection.
  */
int ScintillaWrapper::GetSelectionMode()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionMode\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETSELECTIONMODE);
}

/** Retrieve the position of the start of the selection at the given line (INVALID_POSITION if no selection on this line).
  */
int ScintillaWrapper::GetLineSelStartPosition(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineSelStartPosition\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETLINESELSTARTPOSITION, line);
}

/** Retrieve the position of the end of the selection at the given line (INVALID_POSITION if no selection on this line).
  */
int ScintillaWrapper::GetLineSelEndPosition(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLineSelEndPosition\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETLINESELENDPOSITION, line);
}

/** Move caret down one line, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::LineDownRectExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineDownRectExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINEDOWNRECTEXTEND);
}

/** Move caret up one line, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::LineUpRectExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineUpRectExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINEUPRECTEXTEND);
}

/** Move caret left one character, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::CharLeftRectExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::CharLeftRectExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_CHARLEFTRECTEXTEND);
}

/** Move caret right one character, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::CharRightRectExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::CharRightRectExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_CHARRIGHTRECTEXTEND);
}

/** Move caret to first position on line, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::HomeRectExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::HomeRectExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_HOMERECTEXTEND);
}

/** Move caret to before first visible character on line.
  * If already there move to first character on line.
  * In either case, extend rectangular selection to new caret position.
  */
void ScintillaWrapper::VCHomeRectExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::VCHomeRectExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_VCHOMERECTEXTEND);
}

/** Move caret to last position on line, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::LineEndRectExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::LineEndRectExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_LINEENDRECTEXTEND);
}

/** Move caret one page up, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::PageUpRectExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::PageUpRectExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_PAGEUPRECTEXTEND);
}

/** Move caret one page down, extending rectangular selection to new caret position.
  */
void ScintillaWrapper::PageDownRectExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::PageDownRectExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_PAGEDOWNRECTEXTEND);
}

/** Move caret to top of page, or one page up if already at top of page.
  */
void ScintillaWrapper::StutteredPageUp()
{
	DEBUG_TRACE(L"ScintillaWrapper::StutteredPageUp\n");
	GILRelease gilRelease;
	callScintilla(SCI_STUTTEREDPAGEUP);
}

/** Move caret to top of page, or one page up if already at top of page, extending selection to new caret position.
  */
void ScintillaWrapper::StutteredPageUpExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::StutteredPageUpExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_STUTTEREDPAGEUPEXTEND);
}

/** Move caret to bottom of page, or one page down if already at bottom of page.
  */
void ScintillaWrapper::StutteredPageDown()
{
	DEBUG_TRACE(L"ScintillaWrapper::StutteredPageDown\n");
	GILRelease gilRelease;
	callScintilla(SCI_STUTTEREDPAGEDOWN);
}

/** Move caret to bottom of page, or one page down if already at bottom of page, extending selection to new caret position.
  */
void ScintillaWrapper::StutteredPageDownExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::StutteredPageDownExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_STUTTEREDPAGEDOWNEXTEND);
}

/** Move caret left one word, position cursor at end of word.
  */
void ScintillaWrapper::WordLeftEnd()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordLeftEnd\n");
	GILRelease gilRelease;
	callScintilla(SCI_WORDLEFTEND);
}

/** Move caret left one word, position cursor at end of word, extending selection to new caret position.
  */
void ScintillaWrapper::WordLeftEndExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordLeftEndExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_WORDLEFTENDEXTEND);
}

/** Move caret right one word, position cursor at end of word.
  */
void ScintillaWrapper::WordRightEnd()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordRightEnd\n");
	GILRelease gilRelease;
	callScintilla(SCI_WORDRIGHTEND);
}

/** Move caret right one word, position cursor at end of word, extending selection to new caret position.
  */
void ScintillaWrapper::WordRightEndExtend()
{
	DEBUG_TRACE(L"ScintillaWrapper::WordRightEndExtend\n");
	GILRelease gilRelease;
	callScintilla(SCI_WORDRIGHTENDEXTEND);
}

/** Set the set of characters making up whitespace for when moving or selecting by word.
  * Should be called after SetWordChars.
  */
void ScintillaWrapper::SetWhitespaceChars(boost::python::object characters)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetWhitespaceChars\n");
	std::string stringcharacters = getStringFromObject(characters);
	GILRelease gilRelease;
	callScintilla(SCI_SETWHITESPACECHARS, 0, reinterpret_cast<LPARAM>(stringcharacters.c_str()));
}

/** Reset the set of characters for whitespace and word characters to the defaults.
  */
void ScintillaWrapper::SetCharsDefault()
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCharsDefault\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETCHARSDEFAULT);
}

/** Get currently selected item position in the auto-completion list
  */
int ScintillaWrapper::AutoCGetCurrent()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetCurrent\n");
	GILRelease gilRelease;
	return callScintilla(SCI_AUTOCGETCURRENT);
}

/** Get currently selected item text in the auto-completion list
  * Returns the length of the item text
  */
boost::python::str ScintillaWrapper::AutoCGetCurrentText()
{
	DEBUG_TRACE(L"ScintillaWrapper::AutoCGetCurrentText\n");
	GILRelease gilRelease;
	PythonCompatibleStrBuffer result(callScintilla(SCI_AUTOCGETCURRENTTEXT));
	callScintilla(SCI_AUTOCGETCURRENTTEXT, 0, reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

/** Enlarge the document to a particular size of text bytes.
  */
void ScintillaWrapper::Allocate(int bytes)
{
	DEBUG_TRACE(L"ScintillaWrapper::Allocate\n");
	GILRelease gilRelease;
	callScintilla(SCI_ALLOCATE, bytes);
}

/** Returns the target converted to UTF8.
  * Return the length in bytes.
  */
boost::python::str ScintillaWrapper::TargetAsUTF8()
{
	DEBUG_TRACE(L"ScintillaWrapper::TargetAsUTF8\n");
	GILRelease gilRelease;
	PythonCompatibleStrBuffer result(callScintilla(SCI_TARGETASUTF8));
	callScintilla(SCI_TARGETASUTF8, 0, reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

/** Set the length of the utf8 argument for calling EncodedFromUTF8.
  * Set to -1 and the string will be measured to the first nul.
  */
void ScintillaWrapper::SetLengthForEncode(int bytes)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetLengthForEncode\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETLENGTHFORENCODE, bytes);
}

/** Translates a UTF8 string into the document encoding.
  * Return the length of the result in bytes.
  * On error return 0.
  */
boost::python::str ScintillaWrapper::EncodedFromUTF8()
{
	DEBUG_TRACE(L"ScintillaWrapper::EncodedFromUTF8\n");
	GILRelease gilRelease;
	PythonCompatibleStrBuffer result(callScintilla(SCI_ENCODEDFROMUTF8));
	callScintilla(SCI_ENCODEDFROMUTF8, 0, reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

/** Find the position of a column on a line taking into account tabs and
  * multi-byte characters. If beyond end of line, return line end position.
  */
int ScintillaWrapper::FindColumn(int line, int column)
{
	DEBUG_TRACE(L"ScintillaWrapper::FindColumn\n");
	GILRelease gilRelease;
	return callScintilla(SCI_FINDCOLUMN, line, column);
}

/** Can the caret preferred x position only be changed by explicit movement commands?
  */
int ScintillaWrapper::GetCaretSticky()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCaretSticky\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETCARETSTICKY);
}

/** Stop the caret preferred x position changing when the user types.
  */
void ScintillaWrapper::SetCaretSticky(int useCaretStickyBehaviour)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCaretSticky\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETCARETSTICKY, useCaretStickyBehaviour);
}

/** Switch between sticky and non-sticky: meant to be bound to a key.
  */
void ScintillaWrapper::ToggleCaretSticky()
{
	DEBUG_TRACE(L"ScintillaWrapper::ToggleCaretSticky\n");
	GILRelease gilRelease;
	callScintilla(SCI_TOGGLECARETSTICKY);
}

/** Enable/Disable convert-on-paste for line endings
  */
void ScintillaWrapper::SetPasteConvertEndings(bool convert)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetPasteConvertEndings\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETPASTECONVERTENDINGS, convert);
}

/** Get convert-on-paste setting
  */
bool ScintillaWrapper::GetPasteConvertEndings()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetPasteConvertEndings\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETPASTECONVERTENDINGS));
}

/** Duplicate the selection. If selection empty duplicate the line containing the caret.
  */
void ScintillaWrapper::SelectionDuplicate()
{
	DEBUG_TRACE(L"ScintillaWrapper::SelectionDuplicate\n");
	GILRelease gilRelease;
	callScintilla(SCI_SELECTIONDUPLICATE);
}

/** Set background alpha of the caret line.
  */
void ScintillaWrapper::SetCaretLineBackAlpha(int alpha)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCaretLineBackAlpha\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETCARETLINEBACKALPHA, alpha);
}

/** Get the background alpha of the caret line.
  */
int ScintillaWrapper::GetCaretLineBackAlpha()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCaretLineBackAlpha\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETCARETLINEBACKALPHA);
}

/** Set the style of the caret to be drawn.
  */
void ScintillaWrapper::SetCaretStyle(int caretStyle)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetCaretStyle\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETCARETSTYLE, caretStyle);
}

/** Returns the current style of the caret.
  */
int ScintillaWrapper::GetCaretStyle()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetCaretStyle\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETCARETSTYLE);
}

/** Set the indicator used for IndicatorFillRange and IndicatorClearRange
  */
void ScintillaWrapper::SetIndicatorCurrent(int indicator)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetIndicatorCurrent\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETINDICATORCURRENT, indicator);
}

/** Get the current indicator
  */
int ScintillaWrapper::GetIndicatorCurrent()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetIndicatorCurrent\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETINDICATORCURRENT);
}

/** Set the value used for IndicatorFillRange
  */
void ScintillaWrapper::SetIndicatorValue(int value)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetIndicatorValue\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETINDICATORVALUE, value);
}

/** Get the current indicator vaue
  */
int ScintillaWrapper::GetIndicatorValue()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetIndicatorValue\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETINDICATORVALUE);
}

/** Turn a indicator on over a range.
  */
void ScintillaWrapper::IndicatorFillRange(int position, int fillLength)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicatorFillRange\n");
	GILRelease gilRelease;
	callScintilla(SCI_INDICATORFILLRANGE, position, fillLength);
}

/** Turn a indicator off over a range.
  */
void ScintillaWrapper::IndicatorClearRange(int position, int clearLength)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicatorClearRange\n");
	GILRelease gilRelease;
	callScintilla(SCI_INDICATORCLEARRANGE, position, clearLength);
}

/** Are any indicators present at position?
  */
int ScintillaWrapper::IndicatorAllOnFor(int position)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicatorAllOnFor\n");
	GILRelease gilRelease;
	return callScintilla(SCI_INDICATORALLONFOR, position);
}

/** What value does a particular indicator have at at a position?
  */
int ScintillaWrapper::IndicatorValueAt(int indicator, int position)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicatorValueAt\n");
	GILRelease gilRelease;
	return callScintilla(SCI_INDICATORVALUEAT, indicator, position);
}

/** Where does a particular indicator start?
  */
int ScintillaWrapper::IndicatorStart(int indicator, int position)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicatorStart\n");
	GILRelease gilRelease;
	return callScintilla(SCI_INDICATORSTART, indicator, position);
}

/** Where does a particular indicator end?
  */
int ScintillaWrapper::IndicatorEnd(int indicator, int position)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicatorEnd\n");
	GILRelease gilRelease;
	return callScintilla(SCI_INDICATOREND, indicator, position);
}

/** Set number of entries in position cache
  */
void ScintillaWrapper::SetPositionCache(int size)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetPositionCache\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETPOSITIONCACHE, size);
}

/** How many entries are allocated to the position cache?
  */
int ScintillaWrapper::GetPositionCache()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetPositionCache\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETPOSITIONCACHE);
}

/** Copy the selection, if selection empty copy the line with the caret
  */
void ScintillaWrapper::CopyAllowLine()
{
	DEBUG_TRACE(L"ScintillaWrapper::CopyAllowLine\n");
	GILRelease gilRelease;
	callScintilla(SCI_COPYALLOWLINE);
}

/** Always interpret keyboard input as Unicode
  */
void ScintillaWrapper::SetKeysUnicode(bool keysUnicode)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetKeysUnicode\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETKEYSUNICODE, keysUnicode);
}

/** Are keys always interpreted as Unicode?
  */
bool ScintillaWrapper::GetKeysUnicode()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetKeysUnicode\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETKEYSUNICODE));
}

/** Set the alpha fill colour of the given indicator.
  */
void ScintillaWrapper::IndicSetAlpha(int indicator, int alpha)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicSetAlpha\n");
	GILRelease gilRelease;
	callScintilla(SCI_INDICSETALPHA, indicator, alpha);
}

/** Get the alpha fill colour of the given indicator.
  */
int ScintillaWrapper::IndicGetAlpha(int indicator)
{
	DEBUG_TRACE(L"ScintillaWrapper::IndicGetAlpha\n");
	GILRelease gilRelease;
	return callScintilla(SCI_INDICGETALPHA, indicator);
}

/** Set extra ascent for each line
  */
void ScintillaWrapper::SetExtraAscent(int extraAscent)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetExtraAscent\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETEXTRAASCENT, extraAscent);
}

/** Get extra ascent for each line
  */
int ScintillaWrapper::GetExtraAscent()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetExtraAscent\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETEXTRAASCENT);
}

/** Set extra descent for each line
  */
void ScintillaWrapper::SetExtraDescent(int extraDescent)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetExtraDescent\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETEXTRADESCENT, extraDescent);
}

/** Get extra descent for each line
  */
int ScintillaWrapper::GetExtraDescent()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetExtraDescent\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETEXTRADESCENT);
}

/** Which symbol was defined for markerNumber with MarkerDefine
  */
int ScintillaWrapper::MarkerSymbolDefined(int markerNumber)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarkerSymbolDefined\n");
	GILRelease gilRelease;
	return callScintilla(SCI_MARKERSYMBOLDEFINED, markerNumber);
}

/** Set the text in the text margin for a line
  */
void ScintillaWrapper::MarginSetText(int line, boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarginSetText\n");
	std::string stringtext = getStringFromObject(text);
	GILRelease gilRelease;
	callScintilla(SCI_MARGINSETTEXT, line, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Get the text in the text margin for a line
  */
boost::python::str ScintillaWrapper::MarginGetText(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarginGetText\n");
	GILRelease gilRelease;
	PythonCompatibleStrBuffer result(callScintilla(SCI_MARGINGETTEXT, line));
	callScintilla(SCI_MARGINGETTEXT, line, reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

/** Set the style number for the text margin for a line
  */
void ScintillaWrapper::MarginSetStyle(int line, int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarginSetStyle\n");
	GILRelease gilRelease;
	callScintilla(SCI_MARGINSETSTYLE, line, style);
}

/** Get the style number for the text margin for a line
  */
int ScintillaWrapper::MarginGetStyle(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarginGetStyle\n");
	GILRelease gilRelease;
	return callScintilla(SCI_MARGINGETSTYLE, line);
}

/** Set the style in the text margin for a line
  */
void ScintillaWrapper::MarginSetStyles(int line, boost::python::object styles)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarginSetStyles\n");
	std::string stringstyles = getStringFromObject(styles);
	GILRelease gilRelease;
	callScintilla(SCI_MARGINSETSTYLES, line, reinterpret_cast<LPARAM>(stringstyles.c_str()));
}

/** Get the styles in the text margin for a line
  */
boost::python::str ScintillaWrapper::MarginGetStyles(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarginGetStyles\n");
	GILRelease gilRelease;
	PythonCompatibleStrBuffer result(callScintilla(SCI_MARGINGETSTYLES, line));
	callScintilla(SCI_MARGINGETSTYLES, line, reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

/** Clear the margin text on all lines
  */
void ScintillaWrapper::MarginTextClearAll()
{
	DEBUG_TRACE(L"ScintillaWrapper::MarginTextClearAll\n");
	GILRelease gilRelease;
	callScintilla(SCI_MARGINTEXTCLEARALL);
}

/** Get the start of the range of style numbers used for margin text
  */
void ScintillaWrapper::MarginSetStyleOffset(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::MarginSetStyleOffset\n");
	GILRelease gilRelease;
	callScintilla(SCI_MARGINSETSTYLEOFFSET, style);
}

/** Get the start of the range of style numbers used for margin text
  */
int ScintillaWrapper::MarginGetStyleOffset()
{
	DEBUG_TRACE(L"ScintillaWrapper::MarginGetStyleOffset\n");
	GILRelease gilRelease;
	return callScintilla(SCI_MARGINGETSTYLEOFFSET);
}

/** Set the annotation text for a line
  */
void ScintillaWrapper::AnnotationSetText(int line, boost::python::object text)
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationSetText\n");
	std::string stringtext = getStringFromObject(text);
	GILRelease gilRelease;
	callScintilla(SCI_ANNOTATIONSETTEXT, line, reinterpret_cast<LPARAM>(stringtext.c_str()));
}

/** Get the annotation text for a line
  */
boost::python::str ScintillaWrapper::AnnotationGetText(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationGetText\n");
	GILRelease gilRelease;
	PythonCompatibleStrBuffer result(callScintilla(SCI_ANNOTATIONGETTEXT, line));
	callScintilla(SCI_ANNOTATIONGETTEXT, line, reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

/** Set the style number for the annotations for a line
  */
void ScintillaWrapper::AnnotationSetStyle(int line, int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationSetStyle\n");
	GILRelease gilRelease;
	callScintilla(SCI_ANNOTATIONSETSTYLE, line, style);
}

/** Get the style number for the annotations for a line
  */
int ScintillaWrapper::AnnotationGetStyle(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationGetStyle\n");
	GILRelease gilRelease;
	return callScintilla(SCI_ANNOTATIONGETSTYLE, line);
}

/** Set the annotation styles for a line
  */
void ScintillaWrapper::AnnotationSetStyles(int line, boost::python::object styles)
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationSetStyles\n");
	std::string stringstyles = getStringFromObject(styles);
	GILRelease gilRelease;
	callScintilla(SCI_ANNOTATIONSETSTYLES, line, reinterpret_cast<LPARAM>(stringstyles.c_str()));
}

/** Get the annotation styles for a line
  */
boost::python::str ScintillaWrapper::AnnotationGetStyles(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationGetStyles\n");
	GILRelease gilRelease;
	PythonCompatibleStrBuffer result(callScintilla(SCI_ANNOTATIONGETSTYLES, line));
	callScintilla(SCI_ANNOTATIONGETSTYLES, line, reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

/** Get the number of annotation lines for a line
  */
int ScintillaWrapper::AnnotationGetLines(int line)
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationGetLines\n");
	GILRelease gilRelease;
	return callScintilla(SCI_ANNOTATIONGETLINES, line);
}

/** Clear the annotations from all lines
  */
void ScintillaWrapper::AnnotationClearAll()
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationClearAll\n");
	GILRelease gilRelease;
	callScintilla(SCI_ANNOTATIONCLEARALL);
}

/** Set the visibility for the annotations for a view
  */
void ScintillaWrapper::AnnotationSetVisible(int visible)
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationSetVisible\n");
	GILRelease gilRelease;
	callScintilla(SCI_ANNOTATIONSETVISIBLE, visible);
}

/** Get the visibility for the annotations for a view
  */
int ScintillaWrapper::AnnotationGetVisible()
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationGetVisible\n");
	GILRelease gilRelease;
	return callScintilla(SCI_ANNOTATIONGETVISIBLE);
}

/** Get the start of the range of style numbers used for annotations
  */
void ScintillaWrapper::AnnotationSetStyleOffset(int style)
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationSetStyleOffset\n");
	GILRelease gilRelease;
	callScintilla(SCI_ANNOTATIONSETSTYLEOFFSET, style);
}

/** Get the start of the range of style numbers used for annotations
  */
int ScintillaWrapper::AnnotationGetStyleOffset()
{
	DEBUG_TRACE(L"ScintillaWrapper::AnnotationGetStyleOffset\n");
	GILRelease gilRelease;
	return callScintilla(SCI_ANNOTATIONGETSTYLEOFFSET);
}

/** Add a container action to the undo stack
  */
void ScintillaWrapper::AddUndoAction(int token, int flags)
{
	DEBUG_TRACE(L"ScintillaWrapper::AddUndoAction\n");
	GILRelease gilRelease;
	callScintilla(SCI_ADDUNDOACTION, token, flags);
}

/** Find the position of a character from a point within the window.
  */
int ScintillaWrapper::CharPositionFromPoint(int x, int y)
{
	DEBUG_TRACE(L"ScintillaWrapper::CharPositionFromPoint\n");
	GILRelease gilRelease;
	return callScintilla(SCI_CHARPOSITIONFROMPOINT, x, y);
}

/** Find the position of a character from a point within the window.
  * Return INVALID_POSITION if not close to text.
  */
int ScintillaWrapper::CharPositionFromPointClose(int x, int y)
{
	DEBUG_TRACE(L"ScintillaWrapper::CharPositionFromPointClose\n");
	GILRelease gilRelease;
	return callScintilla(SCI_CHARPOSITIONFROMPOINTCLOSE, x, y);
}

/** Set whether multiple selections can be made
  */
void ScintillaWrapper::SetMultipleSelection(bool multipleSelection)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMultipleSelection\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETMULTIPLESELECTION, multipleSelection);
}

/** Whether multiple selections can be made
  */
bool ScintillaWrapper::GetMultipleSelection()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMultipleSelection\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETMULTIPLESELECTION));
}

/** Set whether typing can be performed into multiple selections
  */
void ScintillaWrapper::SetAdditionalSelectionTyping(bool additionalSelectionTyping)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetAdditionalSelectionTyping\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETADDITIONALSELECTIONTYPING, additionalSelectionTyping);
}

/** Whether typing can be performed into multiple selections
  */
bool ScintillaWrapper::GetAdditionalSelectionTyping()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetAdditionalSelectionTyping\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETADDITIONALSELECTIONTYPING));
}

/** Set whether additional carets will blink
  */
void ScintillaWrapper::SetAdditionalCaretsBlink(bool additionalCaretsBlink)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetAdditionalCaretsBlink\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETADDITIONALCARETSBLINK, additionalCaretsBlink);
}

/** Whether additional carets will blink
  */
bool ScintillaWrapper::GetAdditionalCaretsBlink()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetAdditionalCaretsBlink\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETADDITIONALCARETSBLINK));
}

/** Set whether additional carets are visible
  */
void ScintillaWrapper::SetAdditionalCaretsVisible(bool additionalCaretsBlink)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetAdditionalCaretsVisible\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETADDITIONALCARETSVISIBLE, additionalCaretsBlink);
}

/** Whether additional carets are visible
  */
bool ScintillaWrapper::GetAdditionalCaretsVisible()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetAdditionalCaretsVisible\n");
	GILRelease gilRelease;
	return 0 != (callScintilla(SCI_GETADDITIONALCARETSVISIBLE));
}

/** How many selections are there?
  */
int ScintillaWrapper::GetSelections()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelections\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETSELECTIONS);
}

/** Clear selections to a single empty stream selection
  */
void ScintillaWrapper::ClearSelections()
{
	DEBUG_TRACE(L"ScintillaWrapper::ClearSelections\n");
	GILRelease gilRelease;
	callScintilla(SCI_CLEARSELECTIONS);
}

/** Set a simple selection
  */
int ScintillaWrapper::SetSelection(int caret, int anchor)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelection\n");
	GILRelease gilRelease;
	return callScintilla(SCI_SETSELECTION, caret, anchor);
}

/** Add a selection
  */
int ScintillaWrapper::AddSelection(int caret, int anchor)
{
	DEBUG_TRACE(L"ScintillaWrapper::AddSelection\n");
	GILRelease gilRelease;
	return callScintilla(SCI_ADDSELECTION, caret, anchor);
}

/** Set the main selection
  */
void ScintillaWrapper::SetMainSelection(int selection)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetMainSelection\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETMAINSELECTION, selection);
}

/** Which selection is the main selection
  */
int ScintillaWrapper::GetMainSelection()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetMainSelection\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETMAINSELECTION);
}

/** Which selection is the main selection
  */
void ScintillaWrapper::SetSelectionNCaret(int selection, int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelectionNCaret\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSELECTIONNCARET, selection, pos);
}

/** Which selection is the main selection
  */
int ScintillaWrapper::GetSelectionNCaret(int selection)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionNCaret\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETSELECTIONNCARET, selection);
}

/** Which selection is the main selection
  */
void ScintillaWrapper::SetSelectionNAnchor(int selection, int posAnchor)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelectionNAnchor\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSELECTIONNANCHOR, selection, posAnchor);
}

/** Which selection is the main selection
  */
int ScintillaWrapper::GetSelectionNAnchor(int selection)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionNAnchor\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETSELECTIONNANCHOR, selection);
}

/** Which selection is the main selection
  */
void ScintillaWrapper::SetSelectionNCaretVirtualSpace(int selection, int space)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelectionNCaretVirtualSpace\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSELECTIONNCARETVIRTUALSPACE, selection, space);
}

/** Which selection is the main selection
  */
int ScintillaWrapper::GetSelectionNCaretVirtualSpace(int selection)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionNCaretVirtualSpace\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETSELECTIONNCARETVIRTUALSPACE, selection);
}

/** Which selection is the main selection
  */
void ScintillaWrapper::SetSelectionNAnchorVirtualSpace(int selection, int space)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelectionNAnchorVirtualSpace\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSELECTIONNANCHORVIRTUALSPACE, selection, space);
}

/** Which selection is the main selection
  */
int ScintillaWrapper::GetSelectionNAnchorVirtualSpace(int selection)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionNAnchorVirtualSpace\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETSELECTIONNANCHORVIRTUALSPACE, selection);
}

/** Sets the position that starts the selection - this becomes the anchor.
  */
void ScintillaWrapper::SetSelectionNStart(int selection, int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelectionNStart\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSELECTIONNSTART, selection, pos);
}

/** Returns the position at the start of the selection.
  */
int ScintillaWrapper::GetSelectionNStart(int selection)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionNStart\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETSELECTIONNSTART, selection);
}

/** Sets the position that ends the selection - this becomes the currentPosition.
  */
void ScintillaWrapper::SetSelectionNEnd(int selection, int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetSelectionNEnd\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETSELECTIONNEND, selection, pos);
}

/** Returns the position at the end of the selection.
  */
int ScintillaWrapper::GetSelectionNEnd(int selection)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetSelectionNEnd\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETSELECTIONNEND, selection);
}

/** Returns the position at the end of the selection.
  */
void ScintillaWrapper::SetRectangularSelectionCaret(int pos)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetRectangularSelectionCaret\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETRECTANGULARSELECTIONCARET, pos);
}

/** Returns the position at the end of the selection.
  */
int ScintillaWrapper::GetRectangularSelectionCaret()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetRectangularSelectionCaret\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETRECTANGULARSELECTIONCARET);
}

/** Returns the position at the end of the selection.
  */
void ScintillaWrapper::SetRectangularSelectionAnchor(int posAnchor)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetRectangularSelectionAnchor\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETRECTANGULARSELECTIONANCHOR, posAnchor);
}

/** Returns the position at the end of the selection.
  */
int ScintillaWrapper::GetRectangularSelectionAnchor()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetRectangularSelectionAnchor\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETRECTANGULARSELECTIONANCHOR);
}

/** Returns the position at the end of the selection.
  */
void ScintillaWrapper::SetRectangularSelectionCaretVirtualSpace(int space)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetRectangularSelectionCaretVirtualSpace\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETRECTANGULARSELECTIONCARETVIRTUALSPACE, space);
}

/** Returns the position at the end of the selection.
  */
int ScintillaWrapper::GetRectangularSelectionCaretVirtualSpace()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetRectangularSelectionCaretVirtualSpace\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETRECTANGULARSELECTIONCARETVIRTUALSPACE);
}

/** Returns the position at the end of the selection.
  */
void ScintillaWrapper::SetRectangularSelectionAnchorVirtualSpace(int space)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetRectangularSelectionAnchorVirtualSpace\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETRECTANGULARSELECTIONANCHORVIRTUALSPACE, space);
}

/** Returns the position at the end of the selection.
  */
int ScintillaWrapper::GetRectangularSelectionAnchorVirtualSpace()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetRectangularSelectionAnchorVirtualSpace\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETRECTANGULARSELECTIONANCHORVIRTUALSPACE);
}

/** Returns the position at the end of the selection.
  */
void ScintillaWrapper::SetVirtualSpaceOptions(int virtualSpaceOptions)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetVirtualSpaceOptions\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETVIRTUALSPACEOPTIONS, virtualSpaceOptions);
}

/** Returns the position at the end of the selection.
  */
int ScintillaWrapper::GetVirtualSpaceOptions()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetVirtualSpaceOptions\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETVIRTUALSPACEOPTIONS);
}

/** On GTK+, allow selecting the modifier key to use for mouse-based
  * rectangular selection. Often the window manager requires Alt+Mouse Drag
  * for moving windows.
  * Valid values are SCMOD_CTRL(default), SCMOD_ALT, or SCMOD_SUPER.
  */
void ScintillaWrapper::SetRectangularSelectionModifier(int modifier)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetRectangularSelectionModifier\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETRECTANGULARSELECTIONMODIFIER, modifier);
}

/** Get the modifier key used for rectangular selection.
  */
int ScintillaWrapper::GetRectangularSelectionModifier()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetRectangularSelectionModifier\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETRECTANGULARSELECTIONMODIFIER);
}

/** Set the foreground colour of additional selections.
  * Must have previously called SetSelFore with non-zero first argument for this to have an effect.
  */
void ScintillaWrapper::SetAdditionalSelFore(boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetAdditionalSelFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	GILRelease gilRelease;
	callScintilla(SCI_SETADDITIONALSELFORE, static_cast<WPARAM>(rgbfore));
}

/** Set the background colour of additional selections.
  * Must have previously called SetSelBack with non-zero first argument for this to have an effect.
  */
void ScintillaWrapper::SetAdditionalSelBack(boost::python::tuple back)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetAdditionalSelBack\n");
	COLORREF rgbback = MAKECOLOUR(back);
	GILRelease gilRelease;
	callScintilla(SCI_SETADDITIONALSELBACK, static_cast<WPARAM>(rgbback));
}

/** Set the alpha of the selection.
  */
void ScintillaWrapper::SetAdditionalSelAlpha(int alpha)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetAdditionalSelAlpha\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETADDITIONALSELALPHA, alpha);
}

/** Get the alpha of the selection.
  */
int ScintillaWrapper::GetAdditionalSelAlpha()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetAdditionalSelAlpha\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETADDITIONALSELALPHA);
}

/** Set the foreground colour of additional carets.
  */
void ScintillaWrapper::SetAdditionalCaretFore(boost::python::tuple fore)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetAdditionalCaretFore\n");
	COLORREF rgbfore = MAKECOLOUR(fore);
	GILRelease gilRelease;
	callScintilla(SCI_SETADDITIONALCARETFORE, static_cast<WPARAM>(rgbfore));
}

/** Get the foreground colour of additional carets.
  */
boost::python::tuple ScintillaWrapper::GetAdditionalCaretFore()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetAdditionalCaretFore\n");
	GILRelease gilRelease;
	int retVal = (int)callScintilla(SCI_GETADDITIONALCARETFORE);
	gilRelease.reacquire();
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

/** Set the main selection to the next selection.
  */
void ScintillaWrapper::RotateSelection()
{
	DEBUG_TRACE(L"ScintillaWrapper::RotateSelection\n");
	GILRelease gilRelease;
	callScintilla(SCI_ROTATESELECTION);
}

/** Swap that caret and anchor of the main selection.
  */
void ScintillaWrapper::SwapMainAnchorCaret()
{
	DEBUG_TRACE(L"ScintillaWrapper::SwapMainAnchorCaret\n");
	GILRelease gilRelease;
	callScintilla(SCI_SWAPMAINANCHORCARET);
}

/** Indicate that the internal state of a lexer has changed over a range and therefore
  * there may be a need to redraw.
  */
int ScintillaWrapper::ChangeLexerState(int start, int end)
{
	DEBUG_TRACE(L"ScintillaWrapper::ChangeLexerState\n");
	GILRelease gilRelease;
	return callScintilla(SCI_CHANGELEXERSTATE, start, end);
}

/** Find the next line at or after lineStart that is a contracted fold header line.
  * Return -1 when no more lines.
  */
int ScintillaWrapper::ContractedFoldNext(int lineStart)
{
	DEBUG_TRACE(L"ScintillaWrapper::ContractedFoldNext\n");
	GILRelease gilRelease;
	return callScintilla(SCI_CONTRACTEDFOLDNEXT, lineStart);
}

/** Centre current line in window.
  */
void ScintillaWrapper::VerticalCentreCaret()
{
	DEBUG_TRACE(L"ScintillaWrapper::VerticalCentreCaret\n");
	GILRelease gilRelease;
	callScintilla(SCI_VERTICALCENTRECARET);
}

/** Start notifying the container of all key presses and commands.
  */
void ScintillaWrapper::StartRecord()
{
	DEBUG_TRACE(L"ScintillaWrapper::StartRecord\n");
	GILRelease gilRelease;
	callScintilla(SCI_STARTRECORD);
}

/** Stop notifying the container of all key presses and commands.
  */
void ScintillaWrapper::StopRecord()
{
	DEBUG_TRACE(L"ScintillaWrapper::StopRecord\n");
	GILRelease gilRelease;
	callScintilla(SCI_STOPRECORD);
}

/** Set the lexing language of the document.
  */
void ScintillaWrapper::SetLexer(int lexer)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetLexer\n");
	GILRelease gilRelease;
	callScintilla(SCI_SETLEXER, lexer);
}

/** Retrieve the lexing language of the document.
  */
int ScintillaWrapper::GetLexer()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLexer\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETLEXER);
}

/** Colourise a segment of the document using the current lexing language.
  */
void ScintillaWrapper::Colourise(int start, int end)
{
	DEBUG_TRACE(L"ScintillaWrapper::Colourise\n");
	GILRelease gilRelease;
	callScintilla(SCI_COLOURISE, start, end);
}

/** Set up a value that may be used by a lexer for some optional feature.
  */
void ScintillaWrapper::SetProperty(boost::python::object key, boost::python::object value)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetProperty\n");
	std::string stringkey = getStringFromObject(key);
	std::string stringvalue = getStringFromObject(value);
	GILRelease gilRelease;
	callScintilla(SCI_SETPROPERTY, reinterpret_cast<WPARAM>(stringkey.c_str()), reinterpret_cast<LPARAM>(stringvalue.c_str()));
}

/** Set up the key words used by the lexer.
  */
void ScintillaWrapper::SetKeyWords(int keywordSet, boost::python::object keyWords)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetKeyWords\n");
	std::string stringkeyWords = getStringFromObject(keyWords);
	GILRelease gilRelease;
	callScintilla(SCI_SETKEYWORDS, keywordSet, reinterpret_cast<LPARAM>(stringkeyWords.c_str()));
}

/** Set the lexing language of the document based on string name.
  */
void ScintillaWrapper::SetLexerLanguage(boost::python::object language)
{
	DEBUG_TRACE(L"ScintillaWrapper::SetLexerLanguage\n");
	std::string stringlanguage = getStringFromObject(language);
	GILRelease gilRelease;
	callScintilla(SCI_SETLEXERLANGUAGE, 0, reinterpret_cast<LPARAM>(stringlanguage.c_str()));
}

/** Load a lexer library (dll / so).
  */
void ScintillaWrapper::LoadLexerLibrary(boost::python::object path)
{
	DEBUG_TRACE(L"ScintillaWrapper::LoadLexerLibrary\n");
	std::string stringpath = getStringFromObject(path);
	GILRelease gilRelease;
	callScintilla(SCI_LOADLEXERLIBRARY, 0, reinterpret_cast<LPARAM>(stringpath.c_str()));
}

/** Retrieve a "property" value previously set with SetProperty.
  */
boost::python::str ScintillaWrapper::GetProperty(boost::python::object key)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetProperty\n");
	std::string keyString = getStringFromObject(key);
	GILRelease gilRelease;
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETPROPERTY, reinterpret_cast<WPARAM>(keyString.c_str()), 0));
	callScintilla(SCI_GETPROPERTY, reinterpret_cast<WPARAM>(keyString.c_str()), reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

/** Retrieve a "property" value previously set with SetProperty,
  * with "$()" variable replacement on returned buffer.
  */
boost::python::str ScintillaWrapper::GetPropertyExpanded(boost::python::object key)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetPropertyExpanded\n");
	std::string keyString = getStringFromObject(key);
	GILRelease gilRelease;
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETPROPERTYEXPANDED, reinterpret_cast<WPARAM>(keyString.c_str()), 0));
	callScintilla(SCI_GETPROPERTYEXPANDED, reinterpret_cast<WPARAM>(keyString.c_str()), reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

/** Retrieve a "property" value previously set with SetProperty,
  * interpreted as an int AFTER any "$()" variable replacement.
  */
int ScintillaWrapper::GetPropertyInt(boost::python::object key)
{
	DEBUG_TRACE(L"ScintillaWrapper::GetPropertyInt\n");
	std::string stringkey = getStringFromObject(key);
	GILRelease gilRelease;
	return callScintilla(SCI_GETPROPERTYINT, reinterpret_cast<WPARAM>(stringkey.c_str()));
}

/** Retrieve the number of bits the current lexer needs for styling.
  */
int ScintillaWrapper::GetStyleBitsNeeded()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetStyleBitsNeeded\n");
	GILRelease gilRelease;
	return callScintilla(SCI_GETSTYLEBITSNEEDED);
}

/** Retrieve the name of the lexer.
  * Return the length of the text.
  */
boost::python::str ScintillaWrapper::GetLexerLanguage()
{
	DEBUG_TRACE(L"ScintillaWrapper::GetLexerLanguage\n");
	GILRelease gilRelease;
	PythonCompatibleStrBuffer result(callScintilla(SCI_GETLEXERLANGUAGE));
	callScintilla(SCI_GETLEXERLANGUAGE, 0, reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

/** For private communication between an application and a known lexer.
  */
int ScintillaWrapper::PrivateLexerCall(int operation, int pointer)
{
	DEBUG_TRACE(L"ScintillaWrapper::PrivateLexerCall\n");
	GILRelease gilRelease;
	return callScintilla(SCI_PRIVATELEXERCALL, operation, pointer);
}

/** Retrieve a '\n' separated list of properties understood by the current lexer.
  */
boost::python::str ScintillaWrapper::PropertyNames()
{
	DEBUG_TRACE(L"ScintillaWrapper::PropertyNames\n");
	GILRelease gilRelease;
	PythonCompatibleStrBuffer result(callScintilla(SCI_PROPERTYNAMES));
	callScintilla(SCI_PROPERTYNAMES, 0, reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

/** Retrieve the type of a property.
  */
int ScintillaWrapper::PropertyType(boost::python::object name)
{
	DEBUG_TRACE(L"ScintillaWrapper::PropertyType\n");
	std::string stringname = getStringFromObject(name);
	GILRelease gilRelease;
	return callScintilla(SCI_PROPERTYTYPE, reinterpret_cast<WPARAM>(stringname.c_str()));
}

/** Describe a property.
  */
boost::python::str ScintillaWrapper::DescribeProperty()
{
	DEBUG_TRACE(L"ScintillaWrapper::DescribeProperty\n");
	GILRelease gilRelease;
	PythonCompatibleStrBuffer result(callScintilla(SCI_DESCRIBEPROPERTY));
	callScintilla(SCI_DESCRIBEPROPERTY, 0, reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

/** Retrieve a '\n' separated list of descriptions of the keyword sets understood by the current lexer.
  */
boost::python::str ScintillaWrapper::DescribeKeyWordSets()
{
	DEBUG_TRACE(L"ScintillaWrapper::DescribeKeyWordSets\n");
	GILRelease gilRelease;
	PythonCompatibleStrBuffer result(callScintilla(SCI_DESCRIBEKEYWORDSETS));
	callScintilla(SCI_DESCRIBEKEYWORDSETS, 0, reinterpret_cast<LPARAM>(*result));
	gilRelease.reacquire();
	return boost::python::str(result.c_str());
}

}
