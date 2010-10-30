#include "stdafx.h"

#include "ScintillaWrapper.h"
#include "Scintilla.h"

using namespace std;
using namespace boost::python;



ScintillaWrapper::ScintillaWrapper(const HWND handle)
	: PyProducerConsumer<CallbackExecArgs*>(),
	  m_handle(handle),
	  m_tempString(NULL),
	  m_tempStringLength(0)
{
}

namespace PythonScript
{
void translateOutOfBounds(out_of_bounds_exception const& /* e */)
{
	PyErr_SetString(PyExc_IndexError, "Out of Bounds");
}


}


ScintillaWrapper::~ScintillaWrapper()
{
}
/*
void ScintillaWrapper::addText(str s)
{
	const char *raw = extract<const char*>(s);
	call(SCI_ADDTEXT, len(s), reinterpret_cast<LPARAM>(raw));
}


void ScintillaWrapper::AddStyledText(ScintillaCells s)
{
	call(SCI_ADDSTYLEDTEXT, s.length(), reinterpret_cast<LPARAM>(s.cells()));
}

str ScintillaWrapper::getLine(int lineNumber)
{
	int resultLength = call(SCI_GETLINE, lineNumber, NULL);
	char * result = (char *)malloc(resultLength + 1);
	call(SCI_GETLINE, lineNumber, reinterpret_cast<LPARAM>(result));
	result[resultLength] = '\0';
	str o = str((const char *)result);
	free(result);
	return o;
}
*/
void ScintillaWrapper::notify(SCNotification *notifyCode)
{
	if (!m_notificationsEnabled)
		return;

	pair<callbackT::iterator, callbackT::iterator> callbackIter 
		= m_callbacks.equal_range(notifyCode->nmhdr.code);
	
	if (callbackIter.first != callbackIter.second)
	{
		// Create the parameters for the callback
		dict params;
		params["code"] = notifyCode->nmhdr.code;

		
		switch(notifyCode->nmhdr.code)
		{
			
			case SCN_STYLENEEDED:
				params["position"] = notifyCode->position;
				break;

			case SCN_CHARADDED:
				params["ch"] = notifyCode->ch;
				break;

			case SCN_SAVEPOINTREACHED:
				break;

			case SCN_SAVEPOINTLEFT:
				break;

			case SCN_MODIFYATTEMPTRO:
				break;

			case SCN_KEY:
				params["ch"] = notifyCode->ch;
				params["modifiers"] = notifyCode->modifiers;
				break;

			case SCN_DOUBLECLICK:
				params["position"] = notifyCode->position;
				params["modifiers"] = notifyCode->modifiers;
				params["line"] = notifyCode->line;
				break;

			case SCN_UPDATEUI:
				break;

			case SCN_MODIFIED:
				params["position"] = notifyCode->position;
				params["modificationType"] = notifyCode->modificationType;
				params["text"] = notifyCode->text;
				params["length"] = notifyCode->length;
				params["linesAdded"] = notifyCode->linesAdded;
				params["line"] = notifyCode->line;
				params["foldLevelNow"] = notifyCode->foldLevelNow;
				params["foldLevelPrev"] = notifyCode->foldLevelPrev;
				if (notifyCode->modificationType & SC_MOD_CHANGEANNOTATION)
				{
					params["annotationLinesAdded"] = notifyCode->annotationLinesAdded;
				}
				if (notifyCode->modificationType & SC_MOD_CONTAINER)
				{
					params["token"] = notifyCode->token;
				}

				
				break;

			case SCN_MACRORECORD:
				params["message"] = notifyCode->message;
				params["wParam"] = notifyCode->wParam;
				params["lParam"] = notifyCode->lParam;
				break;

			case SCN_MARGINCLICK:
				params["margin"] = notifyCode->margin;
				break;

			case SCN_NEEDSHOWN:
				break;

			case SCN_PAINTED:
				break;

			case SCN_USERLISTSELECTION:
				params["text"] = notifyCode->text;
				params["listType"] = notifyCode->listType;
				break;

			case SCN_URIDROPPED:
				break;

			case SCN_DWELLSTART:
				params["position"] = notifyCode->position;
				params["x"] = notifyCode->x;
				params["y"] = notifyCode->y;
				break;

			case SCN_DWELLEND:
				params["position"] = notifyCode->position;
				params["x"] = notifyCode->x;
				params["y"] = notifyCode->y;
				break;

			case SCN_ZOOM:
				break;

			case SCN_HOTSPOTCLICK:
				params["position"] = notifyCode->position;
				params["modifiers"] = notifyCode->modifiers;
				break;

			case SCN_HOTSPOTDOUBLECLICK:
				params["position"] = notifyCode->position;
				params["modifiers"] = notifyCode->modifiers;
				break;

			case SCN_INDICATORCLICK:
				break;

			case SCN_INDICATORRELEASE:
				break;

			case SCN_CALLTIPCLICK:
				params["position"] = notifyCode->position;
				break;

			case SCN_AUTOCSELECTION:
				params["text"] = notifyCode->text;
				break;

			case SCN_AUTOCCANCELLED:
				break;

			case SCN_AUTOCCHARDELETED:
				break;



		
		default:
			// Unknown notification, so just fill in all the parameters.
			params["idFrom"] = notifyCode->nmhdr.idFrom;
			params["hwndFrom"] = notifyCode->nmhdr.hwndFrom;
			params["position"] = notifyCode->position;
			params["modificationType"] = notifyCode->modificationType;
			params["text"] = notifyCode->text;
			params["length"] = notifyCode->length;
			params["linesAdded"] = notifyCode->linesAdded;
			params["line"] = notifyCode->line;
			params["foldLevelNow"] = notifyCode->foldLevelNow;
			params["foldLevelPrev"] = notifyCode->foldLevelPrev;
			params["annotationLinesAdded"] = notifyCode->annotationLinesAdded;
			params["listType"] = notifyCode->listType;
			params["message"] = notifyCode->message;
			params["wParam"] = notifyCode->wParam;
			params["lParam"] = notifyCode->lParam;
			params["modifiers"] = notifyCode->modifiers;
			params["token"] = notifyCode->token;
			params["x"] = notifyCode->x;
			params["y"] = notifyCode->y;
			break;
		}

		std::list<PyObject*> *callbacks = new std::list<PyObject*>();
		
		while (callbackIter.first != callbackIter.second)
		{
			callbacks->push_back(callbackIter.first->second);		
			++callbackIter.first;
		}

		CallbackExecArgs *args = new CallbackExecArgs();

		args->callbacks = callbacks;
		args->params = params;

		produce(args);
	}
}


void ScintillaWrapper::consume(CallbackExecArgs *args)
{
	for (std::list<PyObject*>::iterator iter = args->callbacks->begin(); iter != args->callbacks->end(); ++iter)
	{
		PyGILState_STATE state = PyGILState_Ensure();
		try
		{
			call<PyObject*>(*iter, args->params);
		}
		catch(...)
		{
			PyErr_Print();
		}
		PyGILState_Release(state);
	}
	
	delete args->callbacks;
	delete args;
}

bool ScintillaWrapper::callback(PyObject* callback, boost::python::list events)
{
	if (PyCallable_Check(callback))
	{
		int eventCount = len(events);
		for(int i = 0; i < eventCount; ++i)
		{
			m_callbacks.insert(pair<int, PyObject*>(extract<int>(events[i]), callback));
			Py_INCREF(callback);
		}
		
		m_notificationsEnabled = true;
		startConsumer();
	    return true;
	}
	else
	{
		return false;
	}
}

void ScintillaWrapper::clearCallbackFunction(PyObject* callback)
{
	for(callbackT::iterator it = m_callbacks.begin(); it != m_callbacks.end();)
	{
		if (callback == it->second)
		{
			Py_DECREF(it->second);
			it = m_callbacks.erase(it);
		}
		else
		{
			++it;
		}
	}

	if (m_callbacks.empty())
	{
		m_notificationsEnabled = false;
	}
}
	
void ScintillaWrapper::clearCallbackEvents(boost::python::list events)
{
	for(callbackT::iterator it = m_callbacks.begin(); it != m_callbacks.end(); )
	{
		if(extract<bool>(events.contains(it->first)) == true)
		{
			Py_DECREF(it->second);
			it = m_callbacks.erase(it);
		}
		else
		{
			++it;
		}
	}

	if (m_callbacks.empty())
	{
		m_notificationsEnabled = false;
	}
}
	

void ScintillaWrapper::clearCallback(PyObject* callback, boost::python::list events)
{
	for(callbackT::iterator it = m_callbacks.begin(); it != m_callbacks.end(); )
	{
		if(it->second == callback && extract<bool>(events.contains(it->first)) == true)
		{
			Py_DECREF(it->second);
			it = m_callbacks.erase(it);
		}
		else 
		{
			++it;
		}
	}
	if (m_callbacks.empty())
	{
		m_notificationsEnabled = false;
	}
}

void ScintillaWrapper::clearAllCallbacks()
{
	for(callbackT::iterator it = m_callbacks.begin(); it != m_callbacks.end(); )
	{
		Py_DECREF(it->second);
		it = m_callbacks.erase(it);
	}
	
	
	if (m_callbacks.empty())
	{
		m_notificationsEnabled = false;
	}
}



void ScintillaWrapper::forEachLine(PyObject* function)
{
	if (PyCallable_Check(function))
	{	
		BeginUndoAction();
		
		long lineCount = GetLineCount();
		for(int line = 0; line < lineCount;)
		{
			
			object result = call<object>(function, GetLine(line), line, lineCount);
				
			if (result.is_none() || !PyInt_Check(result.ptr()))
			{
				++line;
			}
			else
			{
				line += PyInt_AsLong(result.ptr());
			}
			
			lineCount = GetLineCount();
		}

		EndUndoAction();
	}
}



void ScintillaWrapper::deleteLine(int lineNumber)
{
	int start = 0;
	int lineCount = GetLineCount();
	if (0 != lineNumber && lineCount != 1)
	{
		start = GetLineEndPosition(lineNumber - 1);
	}
	int end = GetLineEndPosition(lineNumber);
	setTarget(start, end);
	this->ReplaceTarget(str(""));
}




void ScintillaWrapper::replaceLine(int lineNumber, boost::python::object newContents)
{
	
	int start = PositionFromLine(lineNumber);	
	int end   = GetLineEndPosition(lineNumber);
	setTarget(start, end);
	ReplaceTarget(newContents);
}

void ScintillaWrapper::replaceWholeLine(int lineNumber, boost::python::object newContents)
{
	
	int start = PositionFromLine(lineNumber);	
	int end;
	if (GetLineCount() > lineNumber)
	{
		end = PositionFromLine(lineNumber + 1);
	}
	else
	{
		end = GetLength();
	}

	setTarget(start, end);
	ReplaceTarget(newContents);
}


boost::python::tuple ScintillaWrapper::getUserLineSelection()
{
	int start = GetSelectionStart();
	int end   = GetSelectionEnd();
	if (start == end)
	{
		start = 0;
		end = GetLineCount() - 1;
	}
	else
	{
		start = LineFromPosition(start);
		end   = LineFromPosition(end);
	}

	return make_tuple(start, end);
}



boost::python::tuple ScintillaWrapper::getUserCharSelection()
{
	int start = GetSelectionStart();
	int end   = GetSelectionEnd();

	if (start == end)
	{
		start = 0;
		end = GetLength();
	}

	return make_tuple(start, end);

}


void ScintillaWrapper::setTarget(int start, int end)
{
	SetTargetStart(start);
	SetTargetEnd(end);
}


void ScintillaWrapper::replace(boost::python::object searchStr, boost::python::object replaceStr, boost::python::object flags)
{
	int start = 0;
	int end = GetLength();
	int iFlags = 0;


	if (!flags.is_none())
	{
		iFlags |= extract<int>(flags);
	}


	const char *replaceChars = extract<const char*>(replaceStr.attr("__str__")());
	
	int replaceLength = strlen(replaceChars);

	Sci_TextToFind src;

	src.lpstrText = const_cast<char*>((const char *)extract<const char *>(searchStr.attr("__str__")()));
	
	BeginUndoAction();
	int result = 0;

	while(result != -1)
	{
		src.chrg.cpMin = start;
		src.chrg.cpMax = end;
		result = callScintilla(SCI_FINDTEXT, iFlags, reinterpret_cast<LPARAM>(&src));
		
		// If nothing found, then just finish
		if (-1 == result)
		{
			return;
		}
		else
		{
			// Replace the location found with the replacement text
			SetTargetStart(src.chrgText.cpMin);
			SetTargetEnd(src.chrgText.cpMax);
			callScintilla(SCI_REPLACETARGET, replaceLength, reinterpret_cast<LPARAM>(replaceChars));
			start = src.chrgText.cpMin + replaceLength;
			end = end + (replaceLength - (src.chrgText.cpMax - src.chrgText.cpMin));
		}

	}
	EndUndoAction();
}

void ScintillaWrapper::rereplace(boost::python::object searchExp, boost::python::object replaceStr, boost::python::object flags)
{
	int start = 0;
	int end = GetLength();
	int iFlags = SCFIND_REGEXP | SCFIND_POSIX;
	if (!flags.is_none())
	{
		iFlags |= extract<int>(flags);
	}

	const char *replaceChars = extract<const char*>(replaceStr.attr("__str__")());

	int replaceLength = strlen(replaceChars);

	Sci_TextToFind src;

	src.lpstrText = const_cast<char*>((const char *)extract<const char *>(searchExp.attr("__str__")()));
	
	BeginUndoAction();
	int result = 0;
	while(result != -1)
	{
		src.chrg.cpMin = start;
		src.chrg.cpMax = end;
		result = callScintilla(SCI_FINDTEXT, iFlags, reinterpret_cast<LPARAM>(&src));
		
		// If nothing found, then just finish
		if (-1 == result)
		{
			return;
		}
		else
		{
			// Replace the location found with the replacement text
			SetTargetStart(src.chrgText.cpMin);
			SetTargetEnd(src.chrgText.cpMax);
			int replacementLength = callScintilla(SCI_REPLACETARGETRE, replaceLength, reinterpret_cast<LPARAM>(replaceChars));
			start = src.chrgText.cpMin + replacementLength;
			end = GetLength();
		}

	}

	EndUndoAction();
}



void ScintillaWrapper::pymlreplace(boost::python::object searchExp, boost::python::object replaceStr, boost::python::object count, boost::python::object flags, boost::python::object startPosition, boost::python::object endPosition)
{
	str contents;
	int currentOffset = 0;	

	if (startPosition.is_none() && endPosition.is_none())
	{
		contents = GetText();
	}

	else
	{
		Sci_TextRange range;
		if (!startPosition.is_none())
		{
			range.chrg.cpMin = extract<int>(startPosition);
		}
		else
		{
			range.chrg.cpMin = 0;
		}

		if (!endPosition.is_none())
		{
			range.chrg.cpMax = extract<int>(endPosition);
		}
		else
		{
			range.chrg.cpMax = GetLength();
		}
		
		currentOffset = range.chrg.cpMin;

		range.lpstrText = new char[(range.chrg.cpMax - range.chrg.cpMin) + 1];
		callScintilla(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&range));
		contents = str(const_cast<const char *>(range.lpstrText));
		delete[] range.lpstrText;
	}



	object re_module( (handle<>(PyImport_ImportModule("re"))) );

	int iFlags = 0;
	int iCount = 0;
	if (!flags.is_none())
	{
		iFlags = extract<int>(flags);
	}
	if (!count.is_none())
	{
		iCount = extract<int>(count);
	}

	if (0 == iCount)
		iCount = -1;
	
	

	object re = re_module.attr("compile")(searchExp, iFlags | extract<int>(re_module.attr("MULTILINE")));
	if (!re_module.is_none())
	{
		object match;
		BeginUndoAction();
		object oreplacement;
		int replacementLength, matchStart, matchEnd;
		int startPos = 0;
		

		do
		{
			match = re.attr("search")(contents, startPos);
			if (!match.is_none())
			{
				// Get expanded replacement string
				oreplacement = match.attr("expand")(replaceStr);
				
				
				// Calculate offsets
				matchStart = extract<int>(match.attr("start")());
				matchEnd   = extract<int>(match.attr("end")());


				// Extract text replacement
				const char *replacement = extract<const char *>(oreplacement);
				replacementLength = len(oreplacement);

				// Replace text in Scintilla
				callScintilla(SCI_SETTARGETSTART, matchStart + currentOffset);
				callScintilla(SCI_SETTARGETEND, matchEnd + currentOffset);
				callScintilla(SCI_REPLACETARGET, replacementLength, reinterpret_cast<LPARAM>(replacement));
				

				// Calculate the difference between the old string, 
				// and the new replacement, and add it to the currentOffset
				currentOffset += replacementLength - (matchEnd - matchStart);


				// Set startPos (the start of the last match (absolute from the start of the string,
				// ignoring the old startPos), plus the length of the replacement
				startPos = matchStart + replacementLength;


			}
		} while(!match.is_none() && (iCount == -1 || --iCount > 0));
		
		EndUndoAction();
	}

}



void ScintillaWrapper::pyreplace(boost::python::object searchExp, boost::python::object replaceStr, boost::python::object count, boost::python::object flags, boost::python::object startLine, boost::python::object endLine)
{
	
	object re_module( (handle<>(PyImport_ImportModule("re"))) );
	if (!re_module.is_none())
	{
		BeginUndoAction();
		const char *strCount = extract<const char *>(count.attr("__str__")());
		int iCount;
		int iFlags = 0;
		
		if (!flags.is_none())
		{
			iFlags = extract<int>(flags);
		}

		int start = 0;
		if (!startLine.is_none())
		{
			start = extract<int>(startLine);
		}

		int end = -1;
		if (!startLine.is_none())
		{
			 end = extract<int>(endLine);
		}

		iCount = atoi(strCount);
		bool ignoreCount = (iCount == 0);
		bool includeLineEndings = (iFlags & RE_INCLUDELINEENDINGS) == RE_INCLUDELINEENDINGS;

		long lineCount = GetLineCount();
		object re = re_module.attr("compile")(searchExp, flags);
		
		int bufferLength = 0;
		Sci_TextRange range;
		range.chrg.cpMin = 0;
		range.lpstrText = NULL;
		tuple result;
		int currentStartPosition;
		int infiniteLoopCheck = 0;
		int previousLine = -1;
		for(int line = start; line < lineCount && (ignoreCount || iCount > 0) && (-1 == end || line <= end); ++line)
		{
			if (line == previousLine)
			{
				if (++infiniteLoopCheck >= 1000)
				{
					EndUndoAction();
					PyErr_SetString(PyExc_SystemError, "Infinite loop detected in pyreplace");
					if (range.lpstrText)
					{
						delete[] range.lpstrText;
					}

					throw boost::python::error_already_set();
				}
			}
			previousLine = line;

			if (includeLineEndings)
			{
				result = extract<tuple>(re.attr("subn")(replaceStr, GetLine(line), ignoreCount ? 0 : iCount));
			}
			else
			{
				range.chrg.cpMin = PositionFromLine(line);
				range.chrg.cpMax = GetLineEndPosition(line);
			
				if (bufferLength < ((range.chrg.cpMax - range.chrg.cpMin) + 1))
				{
					if (range.lpstrText)
						delete [] range.lpstrText;
					bufferLength = (range.chrg.cpMax - range.chrg.cpMin) + 1;
					range.lpstrText = new char[bufferLength + 1];
				}
			
				callScintilla(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&range));

				result = extract<tuple>(re.attr("subn")(replaceStr, const_cast<const char *>(range.lpstrText), ignoreCount ? 0 : iCount));
			}

			int numSubs = extract<int>(result[1]);
			if (numSubs != 0)
			{
				int resultLength = len(result[0]);
				if (includeLineEndings)
				{
					currentStartPosition = PositionFromLine(line);
					replaceWholeLine(line, result[0]);
				}
				else
				{
					currentStartPosition = range.chrg.cpMin;
					replaceLine(line, result[0]);
				}

				int newLine = LineFromPosition(currentStartPosition + resultLength);
				
				// If the line number has moved on more than one
				// there must have been one or more new lines in the 
				// replacement, or no newline, hence the lines have become less
				if ((newLine - line) != (includeLineEndings ? 1 : 0))
				{
					line = newLine - (includeLineEndings ? 1 : 0);
					lineCount = GetLineCount();
				}
				iCount -= numSubs;
			}
		}	

		if (range.lpstrText)
			delete[] range.lpstrText;
		EndUndoAction();
	}

}



void ScintillaWrapper::pysearch(boost::python::object searchExp, boost::python::object callback, boost::python::object flags, boost::python::object startLine, boost::python::object endLine)
{
	
	object re_module( (handle<>(PyImport_ImportModule("re"))) );
	if (!re_module.is_none())
	{
		
		int start = 0;
		if (!startLine.is_none())
		{
			start = extract<int>(startLine);
		}

		int end;
		int lineCount = GetLineCount();
		bool endFixed = false;

		if (!endLine.is_none())
		{
			endFixed = true;
			end = extract<int>(endLine);
		}
		else
		{

			end = lineCount - 1;
		}

		object re = re_module.attr("compile")(searchExp, flags);
		bool called;
		object match;

		for(int line = start; line <= end && line < lineCount; ++line)
		{
			int pos = 0;
			
			called = false;
			do 
			{
				
				match = re.attr("search")(GetLine(line), pos);
			
				// If nothing found, then continue to next line
				if (!match.is_none())
				{

					object result = callback(line, match);
			
					// If return value was false, then stop the search
					if (!result.is_none() && extract<bool>(result) == false)
						return;
					pos = extract<int>(match.attr("end")());
					called = true;
				}

			} while (!match.is_none());
			
			// If we called the user function, update the lineCount
			// (...Who knows what they've done!) :)
			if (called)
			{
				lineCount = GetLineCount();
				if (!endFixed)
					end = lineCount - 1;
			}
		} // end line loop

	} // end re_module check


}




void ScintillaWrapper::pymlsearch(boost::python::object searchExp, boost::python::object callback, boost::python::object flags, boost::python::object startPosition, boost::python::object endPosition)
{
	
	object re_module( (handle<>(PyImport_ImportModule("re"))) );
	if (!re_module.is_none())
	{
		str contents;

		contents = GetText();
		
		int iFlags = 0;
		if (!flags.is_none())
		{
			iFlags = extract<int>(flags);
		}
		
		iFlags |= extract<int>(re_module.attr("MULTILINE"));

		object re = re_module.attr("compile")(searchExp, iFlags);
		object match;

		int pos = 0;
		if (!startPosition.is_none())
		{
			pos = extract<int>(startPosition);
		}

		int endPos = 0;

		if (!endPosition.is_none())
		{
			endPos = extract<int>(endPosition);
		}

		bool endPosFixed = true;

		if (endPos == 0)
		{
			endPos = GetLength();
			endPosFixed = false;
		}



		int line;
		do 
		{
			match = re.attr("search")(contents, pos, endPos);
			
			// If nothing found, then continue to next line
			if (!match.is_none())
			{
				pos = extract<int>(match.attr("start")());
				line = LineFromPosition(pos);
				object result = callback(line, match);
			
				// If return value was false, then stop the search
				if (!result.is_none() && extract<bool>(result) == false)
					return;

				if (!endPosFixed)
				{
					endPos = GetLength();
				}

				pos = extract<int>(match.attr("end")());
			}

		} while (!match.is_none());
			
	} // end re_module check


}



str ScintillaWrapper::getWord(object position, object useOnlyWordChars /* = true */)
{
	int pos;
	if (position.is_none())
	{
		pos = callScintilla(SCI_GETCURRENTPOS);
	}
	else
	{
		pos = extract<int>(position);
	}

	bool wordChars;
	if (useOnlyWordChars.is_none())
	{
		wordChars = true;
	}
	else
	{
		wordChars = extract<bool>(useOnlyWordChars);
	}

	int startPos = callScintilla(SCI_WORDSTARTPOSITION, pos, wordChars);
	int endPos = callScintilla(SCI_WORDENDPOSITION, pos, wordChars);
	Sci_TextRange tr;
	tr.chrg.cpMin = startPos;
	tr.chrg.cpMax = endPos;
	tr.lpstrText = new char[(endPos - startPos) + 1];
	callScintilla(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));
	str retVal(const_cast<const char *>(tr.lpstrText));
	delete[] tr.lpstrText;
	return retVal;
}
