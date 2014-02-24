#include "stdafx.h"

#include "Scintilla.h"
#include "ScintillaCells.h"
#include "ScintillaWrapper.h"
#include "Replacer.h"
#include "Match.h"
#include "ReplacementContainer.h"
#include "NotSupportedException.h"
#include "ArgumentException.h"
#include "PythonScript/NppPythonScript.h"
#include "MutexHolder.h"
#include "GILManager.h"
#include "CallbackExecArgs.h"

namespace NppPythonScript
{
	void translateOutOfBounds(out_of_bounds_exception const& /* e */)
	{
		PyErr_SetString(PyExc_IndexError, "Out of Bounds");
	}


ScintillaWrapper::ScintillaWrapper(const HWND handle, const HWND notepadHandle)
	: PyProducerConsumer<CallbackExecArgs>(),
	  m_handle(handle),
      m_hNotepad(notepadHandle),
	  m_notificationsEnabled(false),
      m_callbackMutex(::CreateMutex(NULL, FALSE, NULL))
{
}

ScintillaWrapper::~ScintillaWrapper()
{
	// m_handle isn't allocated here. Let's just NULL out reference to it, then.
	m_handle = NULL;
}


boost::python::object deprecated_replace_function(boost::python::tuple /* args */, boost::python::dict /* kwargs */)
{
    throw NppPythonScript::NotSupportedException("The pyreplace(), pymlreplace(), pysearch() and pymlsearch() functions have been deprecated.\n"
		"The new replace(), rereplace(), search(), and research() functions have all the same functionality, but are faster, more reliable and have better support for unicode.");
}



std::string ScintillaWrapper::getStringFromObject(boost::python::object o)
{
    std::string raw;
    if (PyUnicode_Check(o.ptr()))
	{
        boost::python::object utf8Text = o.attr("encode")("utf-8");
        raw = std::string(boost::python::extract<const char *>(utf8Text), _len(utf8Text));
	} 
	else
	{
        boost::python::object rawString = o.attr("__str__")();
        raw = std::string(boost::python::extract<const char *>(rawString), _len(rawString));
	}

    return raw;
}

void ScintillaWrapper::notify(SCNotification *notifyCode)
{
	if (!m_notificationsEnabled)
		return;
    
	{
		NppPythonScript::GILLock gilLock;

        NppPythonScript::MutexHolder hold(m_callbackMutex);

		std::pair<callbackT::iterator, callbackT::iterator> callbackIter 
			= m_callbacks.equal_range(notifyCode->nmhdr.code);

		if (callbackIter.first != callbackIter.second)
		{
			std::shared_ptr<CallbackExecArgs> args(new CallbackExecArgs());

            boost::python::dict params;

			// Create the parameters for the callback
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
				params["updated"] = notifyCode->updated;
				break;

			case SCN_MODIFIED:
				params["position"] = notifyCode->position;
				params["modificationType"] = notifyCode->modificationType;
                if (notifyCode->text)
                {
					// notifyCode->text is not null terminated
				    std::string text(notifyCode->text, notifyCode->length);
				    params["text"] = text.c_str();
				}
				else
				{
					params["text"] = "";
				}

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
                params["token"] = notifyCode->token;
                params["annotationLinesAdded"] = notifyCode->annotationLinesAdded;
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
                params["position"] = notifyCode->position;
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
			case SCN_HOTSPOTDOUBLECLICK:
			case SCN_HOTSPOTRELEASECLICK:
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
                params["position"] = notifyCode->position;
				break;

			case SCN_AUTOCCANCELLED:
				break;

			case SCN_AUTOCCHARDELETED:
				break;

			case SCN_FOCUSIN:
			case SCN_FOCUSOUT:
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
				args->addCallback(callbackIter.first->second);
				++callbackIter.first;
			}
            
            args->setParams(params);
		    DEBUG_TRACE(L"Scintilla notification\n");
		    produce(args);
		}
       
	}
}


void ScintillaWrapper::consume(std::shared_ptr<CallbackExecArgs> args)
{
	NppPythonScript::GILLock gilLock;
    DEBUG_TRACE(L"Consuming scintilla callbacks (beginning callback loop)\n");
	for (std::list<boost::python::object>::iterator iter = args->getCallbacks()->begin(); iter != args->getCallbacks()->end(); ++iter)
	{
		
        DEBUG_TRACE(L"Scintilla callback, got GIL, calling callback\n");
		try
		{
            // Perform the callback with a single argument - the dictionary of parameters for the notification
            boost::python::object callback(*iter);
			callback(*(args->getParams()));
		}
		catch(...)
		{
           if (PyErr_Occurred())
			{
                DEBUG_TRACE(L"Python Error calling python callback");
			    PyErr_Print();
			}
			else
			{
                DEBUG_TRACE(L"Non-Python exception occurred calling python callback");
			}
		}
        DEBUG_TRACE(L"Scintilla callback, end of callback, releasing GIL\n");
	}
    DEBUG_TRACE(L"Finished consuming scintilla callbacks\n");
}

bool ScintillaWrapper::addCallback(PyObject* callback, boost::python::list events)
{
	if (PyCallable_Check(callback))
	{
		
		{
            NppPythonScript::MutexHolder hold(m_callbackMutex);

			size_t eventCount = _len(events);
			for(idx_t i = 0; i < eventCount; ++i)
			{
                Py_INCREF(callback);
				m_callbacks.insert(std::pair<int, boost::python::object >(boost::python::extract<int>(events[i]), boost::python::object(boost::python::handle<>(callback))));
			}
			m_notificationsEnabled = true;
		}
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
	NppPythonScript::MutexHolder hold(m_callbackMutex);
	for(callbackT::iterator it = m_callbacks.begin(); it != m_callbacks.end();)
	{
		if (callback == it->second.ptr())
		{
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
	NppPythonScript::MutexHolder hold(m_callbackMutex);

	for(callbackT::iterator it = m_callbacks.begin(); it != m_callbacks.end(); )
	{
		if(boost::python::extract<bool>(events.contains(it->first)))
		{
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
    NppPythonScript::MutexHolder hold(m_callbackMutex);

	for(callbackT::iterator it = m_callbacks.begin(); it != m_callbacks.end(); )
	{
		if(it->second.ptr() == callback && boost::python::extract<bool>(events.contains(it->first)))
		{
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
    NppPythonScript::MutexHolder hold(m_callbackMutex);

	for(callbackT::iterator it = m_callbacks.begin(); it != m_callbacks.end(); )
	{
		it = m_callbacks.erase(it);
	}


	if (m_callbacks.empty())
	{
		m_notificationsEnabled = false;
	}
}

boost::python::str ScintillaWrapper::GetCharacterPointer()
{
    GILRelease release;
    const char *charPtr = reinterpret_cast<const char*>(callScintilla(SCI_GETCHARACTERPOINTER));
    release.reacquire();
	return boost::python::str(charPtr);
}

boost::python::str ScintillaWrapper::GetRangePointer(int position, int rangeLength)
{
    GILRelease release;
    const char *charPtr = reinterpret_cast<const char*>(callScintilla(SCI_GETRANGEPOINTER, position, rangeLength));
    release.reacquire();
	return boost::python::str(charPtr, charPtr + rangeLength);
}

void ScintillaWrapper::forEachLine(PyObject* function)
{
	if (PyCallable_Check(function))
	{	
		BeginUndoAction();
		
		long lineCount = GetLineCount();
		for(int line = 0; line < lineCount;)
		{
			
			boost::python::object result = boost::python::call<boost::python::object>(function, GetLine(line), line, lineCount);
				
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
	this->ReplaceTarget(boost::python::str(""));
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

	return boost::python::make_tuple(start, end);
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

	return boost::python::make_tuple(start, end);

}


void ScintillaWrapper::setTarget(int start, int end)
{
	SetTargetStart(start);
	SetTargetEnd(end);
}


void deleteReplaceEntry(NppPythonScript::ReplaceEntry* entry)
{
    delete entry;
}


const char *ScintillaWrapper::getCurrentAnsiCodePageName()
{
    UINT currentAcp = ::GetACP();
    switch(currentAcp)
	{
	case 1250:
        return "cp1250";

	case 1251:
		return "cp1251";
	case 1252:
		return "cp1252";
	case 1253:
		return "cp1253";
	case 1254:
		return "cp1254";
	case 1255:
		return "cp1255";
	case 1256:
		return "cp1256";
	case 1257:
		return "cp1257";
	case 1258:
		return "cp1258";

	case 50220:
		return "iso-2022-jp";
	case 28591:
		return "iso-8859-1";

	case 28592:
		return "iso-8859-2";

	case 28593:
		return "iso-8859-3";

	case 28594:
		return "iso-8859-4";

	case 28595:
		return "iso-8859-5";

	case 28596:
		return "iso-8859-6";

	case 28597:
		return "iso-8859-7";

	case 28598:
		return "iso-8859-8";

	case 28599:
		return "iso-8859-9";

	case 28603:
		return "iso-8859-13";

	case 28605:
		return "iso-8859-15";

	default:
        // Windows-1252 is a reasonable "english" default.  If there's more standard codepages that python supports,
        // we can add them in as requests come in
        return "windows-1252";
	}
}

std::string ScintillaWrapper::extractEncodedString(boost::python::object str, int toCodePage)
{
    std::string resultStr;
    int searchLength;
    if (PyUnicode_Check(str.ptr()))
	{
        const char *codePageName = "utf-8";

        if (CP_UTF8 != toCodePage)
		{
            codePageName = getCurrentAnsiCodePageName();
		}

        boost::python::object searchUtf8(str.attr("encode")(codePageName));
        searchLength = boost::python::extract<int>(searchUtf8.attr("__len__")());
        resultStr.append(boost::python::extract<const char*>(searchUtf8), searchLength);
	}
	else
	{
        // It's not a unicode string, so just take the string representation of it
        boost::python::object searchStringObject(str.attr("__str__")());
        searchLength = boost::python::extract<int>(searchStringObject.attr("__len__")());
        resultStr.append(boost::python::extract<const char*>(searchStringObject), searchLength);
	}

    return resultStr;
}

NppPythonScript::ReplaceEntry *ScintillaWrapper::convertWithPython(const char * /* text */, NppPythonScript::Match *match, void *state)
{
    ScintillaWrapper* instance = reinterpret_cast<ScintillaWrapper*>(state);
    NppPythonScript::GroupDetail *wholeGroup = match->group(0);
    boost::python::str replacement(instance->m_pythonReplaceFunction(boost::ref(match)));

    NppPythonScript::ReplaceEntry *entry = new NppPythonScript::ReplaceEntry(wholeGroup->start(), wholeGroup->end(), boost::python::extract<const char *>(replacement), boost::python::extract<int>(replacement.attr("__len__")()));
    return entry;
}

bool ScintillaWrapper::searchPythonHandler(const char * /* text */, NppPythonScript::Match *match, void *state)
{
    ScintillaWrapper* instance = reinterpret_cast<ScintillaWrapper*>(state);
    boost::python::object result = instance->m_pythonMatchHandler(boost::ref(match));

    // Should not continue, if and only if the result returned was === False 
    if (!result.is_none() && PyBool_Check(result.ptr()) && false == boost::python::extract<bool>(result))
	{
        return false;
	}

    return true;
}


void ScintillaWrapper::replacePlain(boost::python::object searchStr, boost::python::object replaceStr)
{
    replacePlainFlags(searchStr, replaceStr, NppPythonScript::python_re_flag_literal);

}

void ScintillaWrapper::replacePlainFlags(boost::python::object searchStr, boost::python::object replaceStr, int flags)
{
    replacePlainFlagsStartEndMaxCount(searchStr, replaceStr, flags, -1, -1, 0);
}


void ScintillaWrapper::replacePlainFlagsStart(boost::python::object searchStr, boost::python::object replaceStr, int flags, int startPosition)
{
    replacePlainFlagsStartEndMaxCount(searchStr, replaceStr, flags, startPosition, -1, 0);
}

void ScintillaWrapper::replacePlainFlagsStartEnd(boost::python::object searchStr, boost::python::object replaceStr, int flags, int startPosition, int endPosition)
{
    replacePlainFlagsStartEndMaxCount(searchStr, replaceStr, flags, startPosition, endPosition, 0);
}

void ScintillaWrapper::replacePlainFlagsStartEndMaxCount(boost::python::object searchStr, boost::python::object replaceStr, int flags, int startPosition, int endPosition, int maxCount)
{
    NppPythonScript::python_re_flags resultFlags = NppPythonScript::python_re_flag_literal;

    // Mask off everything but ignorecase
    resultFlags = (NppPythonScript::python_re_flags)(resultFlags | (flags & NppPythonScript::python_re_flag_ignorecase));


    replaceImpl(searchStr, replaceStr, 
        maxCount,
        resultFlags, 
        startPosition,
        endPosition
    );

}


void ScintillaWrapper::replaceRegex(boost::python::object searchStr, boost::python::object replaceStr)
{
    replaceImpl(searchStr, replaceStr, 0, NppPythonScript::python_re_flag_normal, -1, -1);
}

void ScintillaWrapper::replaceRegexFlags(boost::python::object searchStr, boost::python::object replaceStr, int flags)
{
    replaceImpl(searchStr, replaceStr, 0, (NppPythonScript::python_re_flags)flags, -1, -1);
}


void ScintillaWrapper::replaceRegexFlagsStart(boost::python::object searchStr, boost::python::object replaceStr, int flags, int start)
{
    replaceImpl(searchStr, replaceStr, 0, (NppPythonScript::python_re_flags)flags, start, -1);
}


void ScintillaWrapper::replaceRegexFlagsStartEnd(boost::python::object searchStr, boost::python::object replaceStr, int flags, int start, int end)
{
    replaceImpl(searchStr, replaceStr, 0, (NppPythonScript::python_re_flags)flags, start, end);
}


void ScintillaWrapper::replaceRegexFlagsStartEndMaxCount(boost::python::object searchStr, boost::python::object replaceStr, int flags, int start, int end, int count)
{
    replaceImpl(searchStr, replaceStr, count, (NppPythonScript::python_re_flags)flags, start, end);
}


void ScintillaWrapper::replaceImpl(boost::python::object searchStr, boost::python::object replaceStr, 
            int maxCount,
			NppPythonScript::python_re_flags flags, 
			int startPosition, 
			int endPosition)
{
    int currentDocumentCodePage = this->GetCodePage();

    std::string searchChars = extractEncodedString(searchStr, currentDocumentCodePage);
    std::string replaceChars;
    bool isPythonReplaceFunction = true;

    if (!PyFunction_Check(replaceStr.ptr()))
	{
        isPythonReplaceFunction = false;
        replaceChars = extractEncodedString(replaceStr, currentDocumentCodePage);
	}

    std::list<NppPythonScript::ReplaceEntry*> replacements;

    const char *text = reinterpret_cast<const char *>(callScintilla(SCI_GETCHARACTERPOINTER));
    int length = callScintilla(SCI_GETLENGTH);

    if (startPosition < 0) 
	{
        startPosition = 0;
	}

    if (endPosition > 0 && endPosition < length)
	{
        length = endPosition;
	}


    if (CP_UTF8 == currentDocumentCodePage)
	{
        NppPythonScript::Replacer<NppPythonScript::Utf8CharTraits> replacer;

        if (isPythonReplaceFunction)
		{
            m_pythonReplaceFunction = replaceStr;
            replacer.startReplace(text, length, startPosition,  maxCount, searchChars.c_str(), &ScintillaWrapper::convertWithPython, reinterpret_cast<void*>(this), flags, replacements); 
		}
		else
		{
            replacer.startReplace(text, length, startPosition, maxCount, searchChars.c_str(), replaceChars.c_str(), flags, replacements);
		}
	}
	else
	{
        NppPythonScript::Replacer<NppPythonScript::AnsiCharTraits> replacer;

        if (isPythonReplaceFunction)
		{
            m_pythonReplaceFunction = replaceStr;
            replacer.startReplace(text, length, startPosition, maxCount, searchChars.c_str(), &ScintillaWrapper::convertWithPython, reinterpret_cast<void*>(this), flags, replacements); 
		}
		else 
		{
            replacer.startReplace(text, length, startPosition, maxCount, searchChars.c_str(), replaceChars.c_str(), flags, replacements);
		}
	}

    NppPythonScript::ReplacementContainer replacementContainer(&replacements, this);

    BeginUndoAction();

    CommunicationInfo commInfo;
	commInfo.internalMsg = PYSCR_RUNREPLACE;
	commInfo.srcModuleName = _T("PythonScript.dll");
	TCHAR pluginName[] = _T("PythonScript.dll");

    commInfo.info = reinterpret_cast<void*>(&replacementContainer);
	::SendMessage(m_hNotepad, NPPM_MSGTOPLUGIN, reinterpret_cast<WPARAM>(pluginName), reinterpret_cast<LPARAM>(&commInfo));

    EndUndoAction();

    for_each(replacements.begin(), replacements.end(), deleteReplaceEntry);

}

void ScintillaWrapper::searchPlain(boost::python::object searchStr, boost::python::object matchFunction)
{
    searchPlainImpl(searchStr, matchFunction, 0, 0, -1, -1);
}

void ScintillaWrapper::searchRegex(boost::python::object searchStr, boost::python::object matchFunction)
{
	searchImpl(searchStr, matchFunction, 0, NppPythonScript::python_re_flag_normal, -1, -1);
}

void ScintillaWrapper::searchRegexFlags(boost::python::object searchStr, boost::python::object matchFunction, int flags)
{
    searchImpl(searchStr, matchFunction, 0, (NppPythonScript::python_re_flags)flags, -1, -1);
}

void ScintillaWrapper::searchRegexFlagsStart(boost::python::object searchStr, boost::python::object matchFunction, int flags, int startPosition)
{
    searchImpl(searchStr, matchFunction, 0, (NppPythonScript::python_re_flags)flags, startPosition, -1);
}

void ScintillaWrapper::searchRegexFlagsStartEnd(boost::python::object searchStr, boost::python::object matchFunction, int flags, int startPosition, int endPosition)
{
    searchImpl(searchStr, matchFunction, 0, (NppPythonScript::python_re_flags)flags, startPosition, endPosition);
}

void ScintillaWrapper::searchRegexFlagsStartEndCount(boost::python::object searchStr, boost::python::object matchFunction, int flags, int startPosition, int endPosition, int maxCount)
{
    searchImpl(searchStr, matchFunction, maxCount, (NppPythonScript::python_re_flags)flags, startPosition, endPosition);
}


void ScintillaWrapper::searchPlainFlags(boost::python::object searchStr, boost::python::object matchFunction, int flags)
{
	searchPlainImpl(searchStr, matchFunction, 0, (NppPythonScript::python_re_flags)flags, -1, -1);
}

void ScintillaWrapper::searchPlainFlagsStart(boost::python::object searchStr, boost::python::object matchFunction, int flags, int startPosition)
{
    searchPlainImpl(searchStr, matchFunction, 0, (NppPythonScript::python_re_flags)flags, startPosition, -1);
}

void ScintillaWrapper::searchPlainFlagsStartEnd(boost::python::object searchStr, boost::python::object matchFunction, int flags, int startPosition, int endPosition)
{
    searchPlainImpl(searchStr, matchFunction, 0, (NppPythonScript::python_re_flags)flags, startPosition, endPosition);
}

void ScintillaWrapper::searchPlainFlagsStartEndCount(boost::python::object searchStr, boost::python::object matchFunction, int flags, int startPosition, int endPosition, int maxCount)
{
    searchPlainImpl(searchStr, matchFunction, maxCount, (NppPythonScript::python_re_flags)flags, startPosition, endPosition);
}

void ScintillaWrapper::searchPlainImpl(boost::python::object searchStr, boost::python::object matchFunction, int maxCount, int flags, int startPosition, int endPosition)
{
    // Include literal flag, and mask off from the user flags everything but ignorecase
    NppPythonScript::python_re_flags resultFlags = (NppPythonScript::python_re_flags)
		    (NppPythonScript::python_re_flag_literal 
			 | (flags & NppPythonScript::python_re_flag_ignorecase)
			 );

    searchImpl(searchStr, matchFunction, maxCount, resultFlags, startPosition, endPosition);
}


void ScintillaWrapper::searchImpl(boost::python::object searchStr, 
            boost::python::object matchFunction,
            int maxCount,
			NppPythonScript::python_re_flags flags, 
			int startPosition, 
			int endPosition)
{
    int currentDocumentCodePage = this->GetCodePage();

    std::string searchChars = extractEncodedString(searchStr, currentDocumentCodePage);
    
    if (!PyCallable_Check(matchFunction.ptr()))
	{
        throw NppPythonScript::ArgumentException("match parameter must be callable, i.e. either a function or a lambda expression");
	}


    const char *text = reinterpret_cast<const char *>(callScintilla(SCI_GETCHARACTERPOINTER));
    int length = callScintilla(SCI_GETLENGTH);

    if (startPosition < 0) 
	{
        startPosition = 0;
	}

    if (endPosition > 0 && endPosition < length)
	{
        length = endPosition;
	}

    m_pythonMatchHandler = matchFunction;

    if (CP_UTF8 == currentDocumentCodePage)
	{
        NppPythonScript::Replacer<NppPythonScript::Utf8CharTraits> replacer;

        replacer.search(text, length, startPosition,  maxCount, searchChars.c_str(), &ScintillaWrapper::searchPythonHandler, reinterpret_cast<void*>(this), flags); 
	}
	else
	{
        NppPythonScript::Replacer<NppPythonScript::AnsiCharTraits> replacer;

        replacer.search(text, length, startPosition,  maxCount, searchChars.c_str(), &ScintillaWrapper::searchPythonHandler, reinterpret_cast<void*>(this), flags); 
	}


}


/*
void ScintillaWrapper::replace(boost::python::object searchStr, boost::python::object replaceStr, boost::python::object flags)
{
	int start = 0;
	int end = GetLength();
	int iFlags = 0;


	if (!flags.is_none())
	{
		iFlags |= boost::python::extract<int>(flags);
	}


	const char *replaceChars = boost::python::extract<const char*>(replaceStr.attr("__str__")());
	
	size_t replaceLength = strlen(replaceChars);

	Sci_TextToFind src;

	src.lpstrText = const_cast<char*>((const char *)boost::python::extract<const char *>(searchStr.attr("__str__")()));
	int originalEventMask = callScintilla(SCI_GETMODEVENTMASK);
	callScintilla(SCI_SETMODEVENTMASK, 0);
	BeginUndoAction();
	int result = 0;
	std::wstringstream debug;
	while(result != -1)
	{
		src.chrg.cpMin = start;
		src.chrg.cpMax = end;
		debug.str(std::wstring());
		debug << L"Searching ";
		debug << start << L" " << end;


		OutputDebugString(debug.str().c_str());
		result = callScintilla(SCI_FINDTEXT, iFlags, reinterpret_cast<LPARAM>(&src));
		
		// If nothing found, then just finish
		if (-1 == result)
		{
			OutputDebugString(L"Got -1, no more matches");
			break;
		}
		else
		{
			// Replace the location found with the replacement text
			SetTargetStart(src.chrgText.cpMin);
			SetTargetEnd(src.chrgText.cpMax);
			debug.str(std::wstring());
			debug << L"Got result ";
			debug << src.chrgText.cpMin << L" " << src.chrgText.cpMax;
			OutputDebugString(debug.str().c_str());
			callScintilla(SCI_REPLACETARGET, replaceLength, reinterpret_cast<LPARAM>(replaceChars));
			start = src.chrgText.cpMin + (int)replaceLength;
			end = end + ((int)replaceLength - (src.chrgText.cpMax - src.chrgText.cpMin));
		}

	}
	callScintilla(SCI_SETMODEVENTMASK, originalEventMask);
	EndUndoAction();
}

void ScintillaWrapper::rereplace(boost::python::object searchExp, boost::python::object replaceStr, boost::python::object flags)
{
	int start = 0;
	int end = GetLength();
	int iFlags = SCFIND_REGEXP | SCFIND_POSIX;
	if (!flags.is_none())
	{
		iFlags |= boost::python::extract<int>(flags);
	}

	const char *replaceChars = boost::python::extract<const char*>(replaceStr.attr("__str__")());

	size_t replaceLength = strlen(replaceChars);

	Sci_TextToFind src;

	src.lpstrText = const_cast<char*>((const char *)boost::python::extract<const char *>(searchExp.attr("__str__")()));
	
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
			break;
		}
		else
		{
			// Replace the location found with the replacement text
			SetTargetStart(src.chrgText.cpMin);
			SetTargetEnd(src.chrgText.cpMax);
			int replacementLength = callScintilla(SCI_REPLACETARGETRE, replaceLength, reinterpret_cast<LPARAM>(replaceChars));
			start = src.chrgText.cpMin + replacementLength;
			end = end + ((int)replaceLength - (src.chrgText.cpMax - src.chrgText.cpMin));
		}

	}

	EndUndoAction();
}



void ScintillaWrapper::pymlreplace(boost::python::object searchExp, boost::python::object replaceStr, boost::python::object count, boost::python::object flags, boost::python::object startPosition, boost::python::object endPosition)
{
	boost::python::str contents;
	offset_t currentOffset = 0;

	if (startPosition.is_none() && endPosition.is_none())
	{
		contents = GetCharacterPointer();
	}

	else
	{
		Sci_TextRange range;
		if (!startPosition.is_none())
		{
			range.chrg.cpMin = boost::python::extract<int>(startPosition);
		}
		else
		{
			range.chrg.cpMin = 0;
		}

		if (!endPosition.is_none())
		{
			range.chrg.cpMax = boost::python::extract<int>(endPosition);
		}
		else
		{
			range.chrg.cpMax = GetLength();
		}
		
		currentOffset = (offset_t)range.chrg.cpMin;

		range.lpstrText = new char[size_t((range.chrg.cpMax - range.chrg.cpMin) + 1)];
		callScintilla(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&range));
		contents = boost::python::str(const_cast<const char *>(range.lpstrText));
		delete[] range.lpstrText;
	}



	boost::python::object re_module( (boost::python::handle<>(PyImport_ImportModule("re"))) );

	int iFlags = 0;
	int iCount = 0;
	if (!flags.is_none())
	{
		iFlags = boost::python::extract<int>(flags);
	}
	if (!count.is_none())
	{
		iCount = boost::python::extract<int>(count);
	}

	if (0 == iCount)
		iCount = -1;
	
	

	boost::python::object re = re_module.attr("compile")(searchExp, iFlags | boost::python::extract<int>(re_module.attr("MULTILINE")));
	if (!re_module.is_none())
	{
		boost::python::object match;
		BeginUndoAction();
		boost::python::object oreplacement;
		size_t replacementLength;
		idx_t matchStart, matchEnd;
		idx_t startPos = 0;
		

		do
		{
			match = re.attr("search")(contents, startPos);
			if (!match.is_none())
			{
				// Get expanded replacement string
				oreplacement = match.attr("expand")(replaceStr);
				
				
				// Calculate offsets
				matchStart = (idx_t)boost::python::extract<int>(match.attr("start")());
				matchEnd   = (idx_t)boost::python::extract<int>(match.attr("end")());


				// Extract text replacement
				const char *replacement = boost::python::extract<const char *>(oreplacement);
				replacementLength = _len(oreplacement);

				// Replace text in Scintilla
				callScintilla(SCI_SETTARGETSTART, static_cast<offset_t>(matchStart) + currentOffset);
				callScintilla(SCI_SETTARGETEND, static_cast<offset_t>(matchEnd) + currentOffset);
				callScintilla(SCI_REPLACETARGET, replacementLength, reinterpret_cast<LPARAM>(replacement));
				

				// Calculate the difference between the old string, 
				// and the new replacement, and add it to the currentOffset
				currentOffset += static_cast<offset_t>(replacementLength - (matchEnd - matchStart));


				// Set startPos to the end of the last match - startPos is with the original document
				startPos = matchEnd; 


			}
		} while(!match.is_none() && (iCount == -1 || --iCount > 0));
		
		EndUndoAction();
	}

}



void ScintillaWrapper::pyreplace(boost::python::object searchExp, boost::python::object replaceStr, boost::python::object count, boost::python::object flags, boost::python::object startLine, boost::python::object endLine)
{
	
	boost::python::object re_module( (boost::python::handle<>(PyImport_ImportModule("re"))) );
	if (!re_module.is_none())
	{
		BeginUndoAction();
		const char *strCount = boost::python::extract<const char *>(count.attr("__str__")());
		int iCount;
		int iFlags = 0;
		
		if (!flags.is_none())
		{
			iFlags = boost::python::extract<int>(flags);
		}

		int start = 0;
		if (!startLine.is_none())
		{
			start = boost::python::extract<int>(startLine);
		}

		int end = -1;
		if (!startLine.is_none())
		{
			 end = boost::python::extract<int>(endLine);
		}

		iCount = atoi(strCount);
		bool ignoreCount = (iCount == 0);
		bool includeLineEndings = (iFlags & RE_INCLUDELINEENDINGS) == RE_INCLUDELINEENDINGS;

		long lineCount = GetLineCount();
		boost::python::object re = re_module.attr("compile")(searchExp, flags);
		
		size_t bufferLength = 0;
		Sci_TextRange range;
		range.chrg.cpMin = 0;
		range.lpstrText = NULL;
		boost::python::tuple result;
		idx_t currentStartPosition;
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
				result = boost::python::extract<boost::python::tuple>(re.attr("subn")(replaceStr, GetLine(line), ignoreCount ? 0 : iCount));
			}
			else
			{
				range.chrg.cpMin = PositionFromLine(line);
				range.chrg.cpMax = GetLineEndPosition(line);
			
				if (bufferLength < (size_t)((range.chrg.cpMax - range.chrg.cpMin) + 1))
				{
					if (range.lpstrText)
						delete [] range.lpstrText;
					bufferLength = (size_t)((range.chrg.cpMax - range.chrg.cpMin) + 1);
					range.lpstrText = new char[bufferLength + 1];
				}
			
				callScintilla(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&range));

				result = boost::python::extract<boost::python::tuple>(re.attr("subn")(replaceStr, const_cast<const char *>(range.lpstrText), ignoreCount ? 0 : iCount));
			}

			int numSubs = boost::python::extract<int>(result[1]);
			if (numSubs != 0)
			{
				size_t resultLength = _len(result[0]);
				if (includeLineEndings)
				{
					currentStartPosition = (idx_t)PositionFromLine(line);
					replaceWholeLine(line, result[0]);
				}
				else
				{
					currentStartPosition = (idx_t)range.chrg.cpMin;
					replaceLine(line, result[0]);
				}

				int newLine = LineFromPosition((int)(currentStartPosition + resultLength));
				
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
	
	boost::python::object re_module( (boost::python::handle<>(PyImport_ImportModule("re"))) );
	if (!re_module.is_none())
	{
		
		int start = 0;
		if (!startLine.is_none())
		{
			start = boost::python::extract<int>(startLine);
		}

		int end;
		int lineCount = GetLineCount();
		bool endFixed = false;

		if (!endLine.is_none())
		{
			endFixed = true;
			end = boost::python::extract<int>(endLine);
		}
		else
		{

			end = lineCount - 1;
		}

		boost::python::object re = re_module.attr("compile")(searchExp, flags);
		bool called;
		boost::python::object match;

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

					boost::python::object result = callback(line, match);
			
					// If return value was false, then stop the search
					if (!result.is_none() && !boost::python::extract<bool>(result))
					{
						return;
					}
					pos = boost::python::extract<int>(match.attr("end")());
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
	
	boost::python::object re_module( (boost::python::handle<>(PyImport_ImportModule("re"))) );
	if (!re_module.is_none())
	{
		boost::python::str contents;

		contents = GetText();
		
		int iFlags = 0;
		if (!flags.is_none())
		{
			iFlags = boost::python::extract<int>(flags);
		}
		
		iFlags |= boost::python::extract<int>(re_module.attr("MULTILINE"));

		boost::python::object re = re_module.attr("compile")(searchExp, iFlags);
		boost::python::object match;

		int pos = 0;
		if (!startPosition.is_none())
		{
			pos = boost::python::extract<int>(startPosition);
		}

		int endPos = 0;

		if (!endPosition.is_none())
		{
			endPos = boost::python::extract<int>(endPosition);
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
			
			// If nothing found, then skip
			if (!match.is_none())
			{
				pos = boost::python::extract<int>(match.attr("start")());
				line = LineFromPosition(pos);
				boost::python::object result = callback(line, match);
			
				// If return value was false, then stop the search
				if (!result.is_none() && !boost::python::extract<bool>(result))
				{
					return;
				}

				if (!endPosFixed)
				{
					endPos = GetLength();
				}

				pos = boost::python::extract<int>(match.attr("end")());
			}

		} while (!match.is_none());
			
	} // end re_module check


}


*/


boost::python::str ScintillaWrapper::getWord(boost::python::object position, boost::python::object useOnlyWordChars /* = true */)
{
	int pos;
	if (position.is_none())
	{
		pos = callScintilla(SCI_GETCURRENTPOS);
	}
	else
	{
		pos = boost::python::extract<int>(position);
	}

	bool wordChars;
	if (useOnlyWordChars.is_none())
	{
		wordChars = true;
	}
	else
	{
		wordChars = boost::python::extract<bool>(useOnlyWordChars);
	}

	int startPos = callScintilla(SCI_WORDSTARTPOSITION, pos, wordChars);
	int endPos = callScintilla(SCI_WORDENDPOSITION, pos, wordChars);
	Sci_TextRange tr;
	tr.chrg.cpMin = startPos;
	tr.chrg.cpMax = endPos;
	tr.lpstrText = new char[size_t((endPos - startPos) + 1)];
	callScintilla(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));
	boost::python::str retVal(const_cast<const char *>(tr.lpstrText));
	delete[] tr.lpstrText;
	return retVal;
}


}
