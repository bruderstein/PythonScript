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
			try
			{
				call<PyObject*>(callbackIter.first->second, params);
				++callbackIter.first;
			}
			catch(...)
			{
				PyErr_Print();
			}
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


