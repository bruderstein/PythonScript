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
#include "ScintillaCallback.h"
#include "MainThread.h"
#include "MutexHolder.h"
#include "ScintillaCallbackCounter.h"
#include "NotAllowedInCallbackException.h"

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
		boost::python::object utf8Text = o.attr("__str__")();
		raw = std::string(boost::python::extract<const char *>(utf8Text));
	}
	else if (PyBytes_CheckExact(o.ptr()))
	{
		raw.assign(PyBytes_AsString(o.ptr()), PyBytes_Size(o.ptr()));
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
			std::shared_ptr<CallbackExecArgs> callbackExec(new CallbackExecArgs());
			std::shared_ptr<CallbackExecArgs> asyncCallbackExec(new CallbackExecArgs());
			boost::python::dict params;

			// Create the parameters for the callback
			params["code"] = notifyCode->nmhdr.code;
			params["idFrom"] = notifyCode->nmhdr.idFrom;
			params["hwndFrom"] = reinterpret_cast<intptr_t>(notifyCode->nmhdr.hwndFrom);


			switch(notifyCode->nmhdr.code)
			{

			case SCN_STYLENEEDED:
				params["position"] = notifyCode->position;
				break;

			case SCN_CHARADDED:
				params["ch"] = notifyCode->ch;
				params["characterSource"] = notifyCode->characterSource;
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
			case SCN_MARGINRIGHTCLICK:
				params["margin"] = notifyCode->margin;
				params["position"] = notifyCode->position;
				params["modifiers"] = notifyCode->modifiers;
				break;

			case SCN_NEEDSHOWN:
				params["position"] = notifyCode->position;
				params["length"] = notifyCode->length;
				break;

			case SCN_PAINTED:
				break;

			case SCN_USERLISTSELECTION:
				params["position"] = notifyCode->position;
				params["ch"] = notifyCode->ch;
				params["text"] = notifyCode->text;
				params["listType"] = notifyCode->listType;
				params["listCompletionMethod"] = notifyCode->listCompletionMethod;
				break;

			case SCN_URIDROPPED:
				params["text"] = notifyCode->text;
				break;

			case SCN_DWELLSTART:
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
			case SCN_INDICATORRELEASE:
				params["position"] = notifyCode->position;
				params["modifiers"] = notifyCode->modifiers;
				break;

			case SCN_CALLTIPCLICK:
				params["position"] = notifyCode->position;
				break;

			case SCN_AUTOCSELECTION:
				params["position"] = notifyCode->position;
				params["ch"] = notifyCode->ch;
				params["text"] = notifyCode->text;
				params["listCompletionMethod"] = notifyCode->listCompletionMethod;
				break;

			case SCN_AUTOCCANCELLED:
				break;

			case SCN_AUTOCCHARDELETED:
				break;

			case SCN_FOCUSIN:
			case SCN_FOCUSOUT:
				break;

			case SCN_AUTOCCOMPLETED:
				params["listCompletionMethod"] = notifyCode->listCompletionMethod;
				break;

			case SCN_AUTOCSELECTIONCHANGE:
				params["position"] = notifyCode->position;
				params["text"] = notifyCode->text;
				params["listType"] = notifyCode->listType;

			default:
				// Unknown notification, so just fill in all the parameters.
				params["position"] = notifyCode->position;
				params["ch"] = notifyCode->ch;
				params["modifiers"] = notifyCode->modifiers;
				params["modificationType"] = notifyCode->modificationType;
				if (notifyCode->text)
				{
					// notifyCode->text is not null terminated
					std::string text(notifyCode->text, notifyCode->length);
					params["text"] = text.c_str();
				}
				params["length"] = notifyCode->length;
				params["linesAdded"] = notifyCode->linesAdded;
				params["message"] = notifyCode->message;
				params["wParam"] = notifyCode->wParam;
				params["lParam"] = notifyCode->lParam;
				params["line"] = notifyCode->line;
				params["foldLevelNow"] = notifyCode->foldLevelNow;
				params["foldLevelPrev"] = notifyCode->foldLevelPrev;
				params["margin"] = notifyCode->margin;
				params["listType"] = notifyCode->listType;
				params["x"] = notifyCode->x;
				params["y"] = notifyCode->y;
				params["token"] = notifyCode->token;
				params["annotationLinesAdded"] = notifyCode->annotationLinesAdded;
				params["updated"] = notifyCode->updated;
				params["listCompletionMethod"] = notifyCode->listCompletionMethod;
				params["characterSource"] = notifyCode->characterSource;
				break;
			}

			bool hasSyncCallbacks = false;
			bool hasAsyncCallbacks = false;
			while (callbackIter.first != callbackIter.second)
			{
				if (callbackIter.first->second->isAsync())
				{
					asyncCallbackExec->addCallback(callbackIter.first->second->getCallback());
					hasAsyncCallbacks = true;
				}
				else
				{
					callbackExec->addCallback(callbackIter.first->second->getCallback());
					hasSyncCallbacks = true;
				}
				++callbackIter.first;
			}

			if (hasAsyncCallbacks)
			{
				asyncCallbackExec->setParams(params);
				DEBUG_TRACE(L"Scintilla async callback\n");
				produce(asyncCallbackExec);
			}

			if (hasSyncCallbacks)
			{
				callbackExec->setParams(params);
				DEBUG_TRACE(L"Scintilla Sync callback\n");
				runCallbacks(callbackExec);
			}

		}

	}
}


void ScintillaWrapper::consume(std::shared_ptr<CallbackExecArgs> args)
{
	NppPythonScript::GILLock gilLock;

	runCallbacks(args);
	// Clear the callbackExecArgs and delete all objects whilst we still have the GIL
	args.reset();
}

// The GIL must be owned when calling this method
void ScintillaWrapper::runCallbacks(std::shared_ptr<CallbackExecArgs> args)
{
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

bool ScintillaWrapper::addSyncCallback(boost::python::object callback, boost::python::list events)
{
	return addCallbackImpl(callback, events, false);
}

bool ScintillaWrapper::addAsyncCallback(boost::python::object callback, boost::python::list events)
{
	return addCallbackImpl(callback, events, true);
}


bool ScintillaWrapper::addCallbackImpl(boost::python::object callback, boost::python::list events, bool isAsync)
{
	if (PyCallable_Check(callback.ptr()))
	{

		{
			NppPythonScript::MutexHolder hold(m_callbackMutex);

			size_t eventCount = _len(events);
			for(idx_t i = 0; i < eventCount; ++i)
			{
				Py_INCREF(callback.ptr());
				m_callbacks.insert(std::pair<int, boost::shared_ptr<ScintillaCallback> >(boost::python::extract<int>(events[i]),
					boost::shared_ptr<ScintillaCallback>(new ScintillaCallback(callback, isAsync))));
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

void ScintillaWrapper::clearCallbackFunction(boost::python::object callback)
{
	NppPythonScript::MutexHolder hold(m_callbackMutex);
	for(callbackT::iterator it = m_callbacks.begin(); it != m_callbacks.end();)
	{
		if (callback == it->second->getCallback())
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


void ScintillaWrapper::clearCallback(boost::python::object callback, boost::python::list events)
{
	NppPythonScript::MutexHolder hold(m_callbackMutex);

	for(callbackT::iterator it = m_callbacks.begin(); it != m_callbacks.end(); )
	{
		if(it->second->getCallback() == callback && boost::python::extract<bool>(events.contains(it->first)))
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

void ScintillaWrapper::forEachLine(PyObject* function)
{
	if (PyCallable_Check(function))
	{
		BeginUndoAction();

		intptr_t lineCount = GetLineCount();
		for(int line = 0; line < lineCount;)
		{

			boost::python::object result = boost::python::call<boost::python::object>(function, GetLine(line), line, lineCount);

			if (result.is_none() || !PyLong_Check(result.ptr()))
			{
				++line;
			}
			else
			{
				line += PyLong_AsLong(result.ptr());
			}

			lineCount = GetLineCount();
		}

		EndUndoAction();
	}
}



void ScintillaWrapper::deleteLine(int lineNumber)
{
	intptr_t start = PositionFromLine(lineNumber);
	intptr_t lineCount = GetLineCount();
	intptr_t end;
	if (lineCount > lineNumber)
	{
		end = PositionFromLine(lineNumber + 1);
	}
	else
	{
		end = GetLineEndPosition(lineNumber);
	}

	setTarget(start, end);
	this->ReplaceTarget(boost::python::str(""));
}




void ScintillaWrapper::replaceLine(int lineNumber, boost::python::object newContents)
{

	intptr_t start = PositionFromLine(lineNumber);
	intptr_t end   = GetLineEndPosition(lineNumber);
	setTarget(start, end);
	ReplaceTarget(newContents);
}

void ScintillaWrapper::replaceWholeLine(int lineNumber, boost::python::object newContents)
{

	intptr_t start = PositionFromLine(lineNumber);
	intptr_t end;
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
	intptr_t start = GetSelectionStart();
	intptr_t end   = GetSelectionEnd();
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
	intptr_t start = GetSelectionStart();
	intptr_t end   = GetSelectionEnd();

	if (start == end)
	{
		start = 0;
		end = GetLength();
	}

	return boost::python::make_tuple(start, end);

}


void ScintillaWrapper::setTarget(intptr_t start, intptr_t end)
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

std::string ScintillaWrapper::extractEncodedString(boost::python::object str, intptr_t toCodePage)
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

		//TODO how to get str.attr("encode")(codePageName) working again here
		//currently this is always unicode (utf16 or utf8??), the internal representation of python3
		boost::python::object searchUtf8(str.attr("__str__")());
		resultStr.append(boost::python::extract<const char*>(searchUtf8));
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
	std::string replacementStr = boost::python::extract<const char *>(replacement);

	NppPythonScript::ReplaceEntry *entry = new NppPythonScript::ReplaceEntry(wholeGroup->start(), wholeGroup->end(), replacementStr.c_str(), replacementStr.size());
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
	intptr_t currentDocumentCodePage = this->GetCodePage();

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
	intptr_t length = callScintilla(SCI_GETLENGTH);

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

	CommunicationInfo commInfo{};
	commInfo.internalMsg = PYSCR_RUNREPLACE;
	commInfo.srcModuleName = _T("PythonScript.dll");
	TCHAR pluginName[] = _T("PythonScript.dll");

	commInfo.info = reinterpret_cast<void*>(&replacementContainer);
	GILRelease release;
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
	intptr_t currentDocumentCodePage = this->GetCodePage();

	std::string searchChars = extractEncodedString(searchStr, currentDocumentCodePage);

	if (!PyCallable_Check(matchFunction.ptr()))
	{
		throw NppPythonScript::ArgumentException("match parameter must be callable, i.e. either a function or a lambda expression");
	}


	const char *text = reinterpret_cast<const char *>(callScintilla(SCI_GETCHARACTERPOINTER));
	intptr_t length = callScintilla(SCI_GETLENGTH);

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


boost::python::str ScintillaWrapper::getWord(boost::python::object position, boost::python::object useOnlyWordChars /* = true */)
{
	intptr_t pos;
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

	intptr_t startPos = callScintilla(SCI_WORDSTARTPOSITION, pos, wordChars);
	intptr_t endPos = callScintilla(SCI_WORDENDPOSITION, pos, wordChars);
	Sci_TextRange tr{};
	tr.chrg.cpMin = startPos;
	tr.chrg.cpMax = endPos;
	tr.lpstrText = new char[size_t((endPos - startPos) + 1)];
	callScintilla(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));
	boost::python::str retVal(const_cast<const char *>(tr.lpstrText));
	delete[] tr.lpstrText;
	return retVal;
}

void ScintillaWrapper::notAllowedInCallback(const char *message)
{
	DWORD currentThreadID = ::GetCurrentThreadId();

	if (currentThreadID == g_mainThreadID && ScintillaCallbackCounter::isInCallback())
	{
		throw NotAllowedInCallbackException(message);
	}

}

void ScintillaWrapper::swapColours()
{
	intptr_t foreground = 0;
	intptr_t background = 0;
	SendMessage(m_handle, WM_SETREDRAW, FALSE, 0);
	for(int i = 255; i >= 0; --i)
	{
		foreground = callScintilla(SCI_STYLEGETFORE, i);
		background = callScintilla(SCI_STYLEGETBACK, i);
		SendMessage(m_handle, SCI_STYLESETFORE, i, background);
		SendMessage(m_handle, SCI_STYLESETBACK, i, foreground);
	}
	SendMessage(m_handle, WM_SETREDRAW, TRUE, 0);
	InvalidateRect (m_handle, NULL, TRUE);
	UpdateWindow (m_handle);
}

void ScintillaWrapper::flashMilliseconds(int milliseconds)
{
	GILRelease release;
	swapColours();
	::Sleep(milliseconds);
	swapColours();
}


std::string ScintillaWrapper::iso_latin_1_to_utf8(const std::string& ansi_input)
{
	std::string output;

	for (const unsigned char &c : ansi_input)
	{
		if (c < 128) {
			output += c;
		}
		else {
			output += (0xC0 | (c >> 6));
			output += (0x80 | (c & 0x3f));
		}
	}
	return output;
}

}