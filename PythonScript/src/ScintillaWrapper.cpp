#include "stdafx.h"

#include "ScintillaWrapper.h"
#include "Scintilla.h"

#include <boost/python.hpp>

#include "ScintillaCells.h"


using namespace std;
using namespace boost::python;


ScintillaWrapper::ScintillaWrapper(const HWND handle)
	: m_handle(handle),
	  m_tempString(NULL),
	  m_tempStringLength(0)
{
}

namespace PythonScript
{
void translateOutOfBounds(out_of_bounds_exception const& e)
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

		while (callbackIter.first != callbackIter.second)
		{
			PyGILState_STATE state = PyGILState_Ensure();
			try
			{
				call<PyObject*>(callbackIter.first->second, params);
			}
			catch(...)
			{
				PyErr_Print();
			}
			PyGILState_Release(state);
			++callbackIter.first;
		}
	}
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


void ScintillaWrapper::replace(boost::python::object searchStr, boost::python::object replaceStr)
{
	int start = 0;
	int end = GetLength();
	int flags = 0;

	const char *replaceChars = extract<const char*>(replaceStr.attr("__str__")());

	int replaceLength = strlen(replaceChars);

	Sci_TextToFind src;

	src.lpstrText = const_cast<char*>((const char *)extract<const char *>(searchStr.attr("__str__")()));
	
	BeginUndoAction();
	while(true)
	{
		src.chrg.cpMin = start;
		src.chrg.cpMax = end;
		int result = callScintilla(SCI_FINDTEXT, flags, reinterpret_cast<LPARAM>(&src));
		
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

void ScintillaWrapper::rereplace(boost::python::object searchExp, boost::python::object replaceStr)
{
	int start = 0;
	int end = GetLength();
	int flags = SCFIND_REGEXP | SCFIND_POSIX;

	const char *replaceChars = extract<const char*>(replaceStr.attr("__str__")());

	int replaceLength = strlen(replaceChars);

	Sci_TextToFind src;

	src.lpstrText = const_cast<char*>((const char *)extract<const char *>(searchExp.attr("__str__")()));
	
	BeginUndoAction();

	while(true)
	{
		src.chrg.cpMin = start;
		src.chrg.cpMax = end;
		int result = callScintilla(SCI_FINDTEXT, flags, reinterpret_cast<LPARAM>(&src));
		
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



void ScintillaWrapper::pymlreplace(boost::python::object searchExp, boost::python::object replaceStr, boost::python::object count)
{
	str contents = GetText();
	object re_module( (handle<>(PyImport_ImportModule("re"))) );
	if (!re_module.is_none())
	{
		tuple result = extract<tuple>(re_module.attr("subn")(searchExp, replaceStr, contents, count));
		if (extract<int>(result[1]) != 0)
		{
			SetText(extract<str>(result[0]));
		}
	}

}



void ScintillaWrapper::pyreplace(boost::python::object searchExp, boost::python::object replaceStr, boost::python::object count)
{
	
	object re_module( (handle<>(PyImport_ImportModule("re"))) );
	if (!re_module.is_none())
	{
		BeginUndoAction();
		const char *strCount = extract<const char *>(count.attr("__str__")());
		int iCount;
		iCount = atoi(strCount);
		bool ignoreCount = (iCount == 0);

		long lineCount = GetLineCount();

		for(int line = 0; line < lineCount && (ignoreCount || iCount > 0); ++line)
		{
			tuple result = extract<tuple>(re_module.attr("subn")(searchExp, replaceStr, GetLine(line), ignoreCount ? 0 : iCount));
			int numSubs = extract<int>(result[1]);
			if (numSubs != 0)
			{
				replaceWholeLine(line, result[0]);
				iCount -= numSubs;
				// In case there's a newline in the replacement, we'll update the lineCount
				lineCount = GetLineCount();
			}
		}	

		EndUndoAction();
	}

}
