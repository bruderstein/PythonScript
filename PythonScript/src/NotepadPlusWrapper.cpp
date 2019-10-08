#include "stdafx.h"

#include "Scintilla.h"
#include "WcharMbcsConverter.h"
#include "NotepadPlusWrapper.h"
#include "PromptDialog.h"
#include "MenuManager.h"
#include "PluginInterface.h"
#include "ScintillaWrapper.h"
#include "PythonScriptVersion.h"
#include "GILManager.h"
#include "MutexHolder.h"
#include "NotAllowedInCallbackException.h"
#include "MainThread.h"
#include "ScintillaCallbackCounter.h"
#include "InvalidValueProvidedException.h"

namespace NppPythonScript
{

NotepadPlusWrapper::NotepadPlusWrapper(HINSTANCE hInst, HWND nppHandle)
	: m_nppHandle(nppHandle),
      m_hInst(hInst),
	  m_notificationsEnabled(false),
      m_callbackMutex(::CreateMutex(NULL, FALSE, NULL))
{ }

NotepadPlusWrapper::~NotepadPlusWrapper()
{
	try
	{
		m_callbacks.clear();
		m_notificationsEnabled = false;
	}
	catch (...)
	{
		// I don't know what to do with that, but a destructor should never throw, so...
	}

	// To please Lint, let's NULL these handles and pointers
	m_nppHandle = NULL;
	m_hInst = NULL;
}

void NotepadPlusWrapper::notify(SCNotification *notifyCode)
{
	if (!m_notificationsEnabled)
		return;


    // Optimisation. Count the number of callbacks registered for this code,
    // if there are none, then we can simply return without claiming the GIL.
    // This is especially helpful as N++ forwards WM_NOTIFY messages from child windows, so we
    // get all manor of garbage from RebarWindows etc, that we just don't care about.
    // Overall, it's slightly slower in the case of a real callback (we count them, then
    // find them all with the equal_range() call below, but much quicker in the case of a
    // notification that we don't care about, as we don't need to grab the GIL then give it back.
    // We use count, because *ANY* operation that involves the boost::python::object (e.g. creating
    // an iterator) requires the GIL to manage the refcounts. A count() only involves the integer keys,
    // so we're safe to do that without the GIL.
    callbackT::size_type count = m_callbacks.count(notifyCode->nmhdr.code);
	if (0 == count)
        return;

    DEBUG_TRACE_S(("Notepad notify with code %d\n", notifyCode->nmhdr.code));



    GILLock gilLock;


	std::pair<callbackT::iterator, callbackT::iterator> callbackIter
		= m_callbacks.equal_range(notifyCode->nmhdr.code);

	if (callbackIter.first != callbackIter.second)
	{
		// Create the parameters for the callback
		boost::python::dict params;
		params["code"] = notifyCode->nmhdr.code;
		switch(notifyCode->nmhdr.code)
		{
		case NPPN_FILEBEFORECLOSE:
		case NPPN_FILEOPENED:
		case NPPN_FILECLOSED:
		case NPPN_FILEBEFOREOPEN:
		case NPPN_FILEBEFORESAVE:
		case NPPN_FILESAVED:
		case NPPN_BUFFERACTIVATED:
		case NPPN_LANGCHANGED:
		case NPPN_WORDSTYLESUPDATED:
		case NPPN_FILELOADFAILED:
		case NPPN_SNAPSHOTDIRTYFILELOADED:
		case NPPN_FILEBEFORERENAME:
		case NPPN_FILERENAMECANCEL:
		case NPPN_FILERENAMED:
		case NPPN_FILEBEFOREDELETE:
		case NPPN_FILEDELETEFAILED:
		case NPPN_FILEDELETED:
			params["bufferID"] = notifyCode->nmhdr.idFrom;
			break;

		case NPPN_FILEBEFORELOAD:
		case NPPN_READY:
		case NPPN_SHUTDOWN:
		case NPPN_TBMODIFICATION:  // I hope no-one ever has to use this!
		case NPPN_BEFORESHUTDOWN:
		case NPPN_CANCELSHUTDOWN:
			// No additional parameters
			break;

		case NPPN_READONLYCHANGED:
			params["bufferID"] = reinterpret_cast<uintptr_t>(notifyCode->nmhdr.hwndFrom);
			params["readonly"] = (notifyCode->nmhdr.idFrom & 1) ? true : false;
			params["dirty"] = (notifyCode->nmhdr.idFrom & 2) ? true : false;
			break;


		case NPPN_SHORTCUTREMAPPED:
			{
				params["commandID"] = notifyCode->nmhdr.idFrom;
				ShortcutKey *key = reinterpret_cast<ShortcutKey*>(notifyCode->nmhdr.hwndFrom);

				params["key"] = key->_key;
				params["isCtrl"] = key->_isCtrl;
				params["isAlt"] = key->_isAlt;
				params["isShift"] = key->_isShift;
			}
			break;

		case NPPN_DOCORDERCHANGED:
			params["newIndex"] = reinterpret_cast<uintptr_t>(notifyCode->nmhdr.hwndFrom);
			params["bufferID"] = notifyCode->nmhdr.idFrom;
			break;

		default:
			// Unknown notification, so just fill in the parameters as integers.
			params["idFrom"] = notifyCode->nmhdr.idFrom;
			params["hwndFrom"] = reinterpret_cast<uintptr_t>(notifyCode->nmhdr.hwndFrom);
			break;
		}


		std::list<boost::python::object> callbacks;

		while (callbackIter.first != callbackIter.second)
		{
			callbacks.push_back(callbackIter.first->second);
			++callbackIter.first;
		}


		for (std::list<boost::python::object>::iterator listIter = callbacks.begin(); listIter != callbacks.end(); ++listIter)
		{
			try
			{
                // Call the callback
                (*listIter)(params);
			}
			catch(...)
			{
                if (PyErr_Occurred())
				{
                    DEBUG_TRACE(L"Python error occurred in Notepad++ callback");
				    PyErr_Print();
				}
				else
				{
                    DEBUG_TRACE(L"Non-Python error occurred in Notepad++ callback");
				}
			}
		}

	}
}

bool NotepadPlusWrapper::addCallback(boost::python::object callback, boost::python::list events)
{
    MutexHolder hold(m_callbackMutex);
	if (PyCallable_Check(callback.ptr()))
	{
		size_t eventCount = _len(events);
		for(idx_t i = 0; i < eventCount; i++)
		{
			m_callbacks.insert(std::pair<int, boost::python::object>(boost::python::extract<int>(events[i]), callback));
		}

		m_notificationsEnabled = true;

	    return true;
	}
	else
	{
		return false;
	}
}

void NotepadPlusWrapper::save()
{
	callNotepad(NPPM_SAVECURRENTFILE);

}

void NotepadPlusWrapper::newDocument()
{
    DEBUG_TRACE(L"NotepadPlusWrapper::newDocument\n");
    notAllowedInScintillaCallback("new() cannot be called in a synchronous editor callback. "
		"Use an asynchronous callback, or avoid using new() in the callback handler");
	callNotepad(NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);

}

void NotepadPlusWrapper::newDocumentWithFilename(const char *filename)
{
    notAllowedInScintillaCallback("new() cannot be called in a synchronous editor callback. "
		"Use an asynchronous callback, or avoid using new() in the callback handler");

	callNotepad(NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);
	std::shared_ptr<TCHAR> tFilename = WcharMbcsConverter::char2tchar(filename);
	callNotepad(NPPM_SAVECURRENTFILEAS, 0, reinterpret_cast<LPARAM>(tFilename.get()));

}

void NotepadPlusWrapper::saveAs(const char *filename)
{
	callNotepad(NPPM_SAVECURRENTFILEAS, FALSE, reinterpret_cast<LPARAM>(WcharMbcsConverter::char2tchar(filename).get()));

}

void NotepadPlusWrapper::saveAsCopy(const char *filename)
{
	callNotepad(NPPM_SAVECURRENTFILEAS, TRUE, reinterpret_cast<LPARAM>(WcharMbcsConverter::char2tchar(filename).get()));

}

void NotepadPlusWrapper::open(const char *filename)
{
    notAllowedInScintillaCallback("open() cannot be called in a synchronous editor callback. "
		"Use an asynchronous callback, or avoid using open() in the callback handler");
	callNotepad(NPPM_DOOPEN, 0, reinterpret_cast<LPARAM>(WcharMbcsConverter::char2tchar(filename).get()));

}

bool NotepadPlusWrapper::activateFile(const char *filename)
{
    notAllowedInScintillaCallback("activateFile() cannot be called in a synchronous editor callback. "
		"Use an asynchronous callback, or avoid using activateFile() in the callback handler");
	bool retVal;
	retVal = 0 != callNotepad(NPPM_SWITCHTOFILE, 0, reinterpret_cast<LPARAM>(WcharMbcsConverter::char2tchar(filename).get()));

	return retVal;
}

int NotepadPlusWrapper::getCurrentView()
{
	int currentView;
	callNotepad(NPPM_GETCURRENTSCINTILLA, 0, reinterpret_cast<LPARAM>(&currentView));
	return currentView;
}

LangType NotepadPlusWrapper::getCurrentLangType()
{
	int lang;
	callNotepad(NPPM_GETCURRENTLANGTYPE, 0, reinterpret_cast<LPARAM>(&lang));
	return static_cast<LangType>(lang);
}

void NotepadPlusWrapper::setCurrentLangType(LangType lang)
{
	callNotepad(NPPM_SETCURRENTLANGTYPE, 0, static_cast<LPARAM>(lang));

}

boost::python::str NotepadPlusWrapper::getPluginVersion()
{
	return boost::python::str(PYSCR_VERSION_STRING);
}

boost::python::list NotepadPlusWrapper::getFiles()
{
	idx_t count;
	intptr_t bufferID;

	boost::python::list files;

	int view = 0;
	bool onlyOneView = isSingleView();
	if (onlyOneView) { view = getCurrentView(); }
	int view_end = onlyOneView ? view : 1;
			
	for(view; view <= view_end; view++)
	{
		count = (idx_t)callNotepad(NPPM_GETNBOPENFILES, 0, view ? SECOND_VIEW : PRIMARY_VIEW);

		TCHAR **fileNames = (TCHAR **)new TCHAR*[count];
		for (idx_t i = 0 ; i < count ; i++)
		{
			fileNames[i] = new TCHAR[MAX_PATH];
		}

		if (callNotepad(view ? NPPM_GETOPENFILENAMESSECOND : NPPM_GETOPENFILENAMESPRIMARY,
							reinterpret_cast<WPARAM>(fileNames), static_cast<LPARAM>(count)))
		{
			for(idx_t pos = 0; pos < count; pos++)
			{
				bufferID = callNotepad(NPPM_GETBUFFERIDFROMPOS, pos, view);
				if (bufferID)
				{
#ifdef UNICODE
					std::shared_ptr<char> mbFilename = WcharMbcsConverter::tchar2char(fileNames[pos]);
					files.append(boost::python::make_tuple(const_cast<const char*>(mbFilename.get()), bufferID, pos, view));
#else
					files.append(boost::python::make_tuple(const_cast<const char*>(fileNames[pos]), bufferID, pos, view));
#endif
				}
			}
		}

		for (idx_t i = 0 ; i < count ; i++)
		{
			delete [] fileNames[i];
		}

		delete [] fileNames;
	}

	return files;
}

boost::python::list NotepadPlusWrapper::getSessionFiles(const char *sessionFilename)
{
	boost::python::list result;
#ifdef UNICODE
	std::shared_ptr<TCHAR> converted = WcharMbcsConverter::char2tchar(sessionFilename);
	const TCHAR *convertedSessionFilename = converted.get();
#else
	const TCHAR *convertedSessionFilename = sessionFilename;
#endif
	idx_t count = (idx_t)callNotepad(NPPM_GETNBSESSIONFILES, 0, reinterpret_cast<LPARAM>(convertedSessionFilename));
	if (count > 0)
	{
		TCHAR **fileNames = (TCHAR **)new TCHAR*[count];
		for (idx_t pos = 0 ; pos < count ; pos++)
		{
			fileNames[pos] = new TCHAR[MAX_PATH];
		}

		if (callNotepad(NPPM_GETSESSIONFILES, reinterpret_cast<WPARAM>(fileNames), reinterpret_cast<LPARAM>(convertedSessionFilename)))
		{

			for(idx_t pos = 0; pos < count; pos++)
			{
#ifdef UNICODE
					std::shared_ptr<char> mbFilename = WcharMbcsConverter::tchar2char(fileNames[pos]);
					result.append(const_cast<const char*>(mbFilename.get()));
#else
					result.append(const_cast<const char*>(fileNames[pos]));
#endif
			}

		}

		for (idx_t i = 0 ; i < count ; i++)
		{
			delete [] fileNames[i];
		}

		delete [] fileNames;
	}

	return result;

}

void NotepadPlusWrapper::saveSession(const char *sessionFilename, boost::python::list files)
{

	sessionInfo si;
	std::shared_ptr<TCHAR> tsessionFilename = WcharMbcsConverter::char2tchar(sessionFilename);
	si.sessionFilePathName = tsessionFilename.get();

	size_t filesCount = _len(files);
	si.files = (TCHAR **)new TCHAR*[filesCount];

	// Vector to store temporary list of shared_ptr
	std::vector< std::shared_ptr<TCHAR> > filesList(filesCount);

	for(idx_t pos = 0; pos < filesCount; pos++)
	{
		filesList[pos] = WcharMbcsConverter::char2tchar(static_cast<const char*>(boost::python::extract<const char *>(files[pos])));
		si.files[pos] = filesList[pos].get();
	}

	si.nbFile = (int)filesCount;

	callNotepad(NPPM_SAVESESSION, 0, reinterpret_cast<LPARAM>(&si));


	for(idx_t pos = 0; pos < filesCount; pos++)
	{
		filesList[pos].reset();
	}

}

void NotepadPlusWrapper::saveCurrentSession(const char *filename)
{
	callNotepad(NPPM_SAVECURRENTSESSION, 0, reinterpret_cast<LPARAM>(WcharMbcsConverter::char2tchar(filename).get()));

}

boost::shared_ptr<ScintillaWrapper> NotepadPlusWrapper::createScintilla()
{
    notAllowedInScintillaCallback("createScintilla() is not allowed in a synchronous editor callback.  Use an asynchronous callback, or avoid calling createScintilla() in the callback handler.");
	LRESULT handle = callNotepad(NPPM_CREATESCINTILLAHANDLE, 0, NULL);

	// return copy
	return boost::shared_ptr<ScintillaWrapper>(new ScintillaWrapper((HWND)handle, m_nppHandle));
}

void NotepadPlusWrapper::destroyScintilla(boost::shared_ptr<ScintillaWrapper> buffer)
{
	if (buffer)
	{
		notAllowedInScintillaCallback("destroyScintilla() is not allowed in a synchronous editor callback.  Use an asynchronous callback, or avoid calling destroyScintilla() in the callback handler.");
		callNotepad(NPPM_DESTROYSCINTILLAHANDLE, 0, reinterpret_cast<LPARAM>(buffer->getHandle()));
		buffer->invalidateHandle();
	}
}

idx_t NotepadPlusWrapper::getCurrentDocIndex(int view)
{
	return callNotepad(NPPM_GETCURRENTDOCINDEX, 0, static_cast<LPARAM>(view));
}

void NotepadPlusWrapper::setStatusBar(StatusBarSection section, const char *text)
{
#ifdef UNICODE
		std::shared_ptr<TCHAR> s = WcharMbcsConverter::char2tchar(text);
	callNotepad(NPPM_SETSTATUSBAR, static_cast<WPARAM>(section), reinterpret_cast<LPARAM>(s.get()));
#else
	callNotepad(NPPM_SETSTATUSBAR, static_cast<WPARAM>(section), reinterpret_cast<LPARAM>(text));
#endif

}

LRESULT NotepadPlusWrapper::getPluginMenuHandle()
{
	return callNotepad(NPPM_GETMENUHANDLE, 0, 0);
}

void NotepadPlusWrapper::activateIndex(int view, int index)
{
	callNotepad(NPPM_ACTIVATEDOC, static_cast<WPARAM>(view), static_cast<LPARAM>(index));

}

void NotepadPlusWrapper::loadSession(boost::python::str filename)
{
	notAllowedInScintillaCallback("loadSession() cannot be called in a synchronous editor callback. "
		"Use an asynchronous callback, or avoid using loadSession() in the callback handler");
#ifdef UNICODE
	std::shared_ptr<TCHAR> s = WcharMbcsConverter::char2tchar((const char*)boost::python::extract<const char*>(filename));
	callNotepad(NPPM_LOADSESSION, 0, reinterpret_cast<LPARAM>(s.get()));
#else
	callNotepad(NPPM_LOADSESSION, 0, reinterpret_cast<LPARAM>((const char*)boost::python::extract<const char*>(filename)));
#endif

}

void NotepadPlusWrapper::activateFileString(boost::python::str filename)
{
	notAllowedInScintillaCallback("activateFile() cannot be called in a synchronous editor callback. "
		"Use an asynchronous callback, or avoid using activateFile() in the callback handler");
	#ifdef UNICODE
	std::shared_ptr<TCHAR> s = WcharMbcsConverter::char2tchar((const char*)boost::python::extract<const char*>(filename));
	callNotepad(NPPM_SWITCHTOFILE, 0, reinterpret_cast<LPARAM>(s.get()));
#else
	callNotepad(NPPM_SWITCHTOFILE, 0, reinterpret_cast<LPARAM>((const char*)boost::python::extract<const char*>(filename)));
#endif

}

void NotepadPlusWrapper::reloadFile(boost::python::str filename, bool alert)
{
#ifdef UNICODE
	callNotepad(NPPM_RELOADFILE, alert ? 1 : 0, reinterpret_cast<LPARAM>(static_cast<const TCHAR *>(WcharMbcsConverter::char2tchar(boost::python::extract<const char *>(filename)).get())));
#else
	callNotepad(NPPM_RELOADFILE, alert ? 1 : 0, reinterpret_cast<LPARAM>(static_cast<const char *>(boost::python::extract<const char *>(filename))));
#endif

}

void NotepadPlusWrapper::saveAllFiles()
{
	callNotepad(NPPM_SAVEALLFILES);

}

boost::python::str NotepadPlusWrapper::getPluginConfigDir()
{
	TCHAR temp[MAX_PATH];
	callNotepad(NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(temp));
	return boost::python::str(const_cast<const char *>(WcharMbcsConverter::tchar2char(temp).get()));
}

boost::python::str NotepadPlusWrapper::getPluginHomePath()
{
	LRESULT size = callNotepad(NPPM_GETPLUGINHOMEPATH, 0, NULL);
	wchar_t* result(new wchar_t[size+1]);
	callNotepad(NPPM_GETPLUGINHOMEPATH, size+1, reinterpret_cast<LPARAM>(result));
	std::shared_ptr<char> homePath = WcharMbcsConverter::tchar2char(result);
	return boost::python::str(const_cast<const char *>(homePath.get()));
}

void NotepadPlusWrapper::menuCommand(int commandID)
{
	callNotepad(NPPM_MENUCOMMAND, 0, commandID);

}

boost::python::tuple NotepadPlusWrapper::getVersion()
{
	LPARAM nppVersion = callNotepad(NPPM_GETNPPVERSION);

	int majorVersion = HIWORD(nppVersion);
	int minorVersion = LOWORD(nppVersion);

	std::vector<int> version(4);

	version[0] = majorVersion;

	char tmp[10];
	_itoa_s(minorVersion, tmp, 10, 10);

	idx_t i;
	for(i = 0; tmp[i] && i < 4; i++)
	{
		version[i+1] = tmp[i] - 48;
	}

	//lint -e864 Expression involving variable 'version' possibly depends on order of evaluation
	// It's perfectly safe the way it's used here.
	switch(i)
	{
		case 3:
			return boost::python::make_tuple<int, int, int, int>(version[0], version[1], version[2], version[3]);

		case 2:
			return boost::python::make_tuple<int, int, int>(version[0], version[1], version[2]);

		case 1:
			return boost::python::make_tuple<int, int>(version[0], version[1]);

		case 0:
		default:
			return boost::python::make_tuple<int>(version[0]);
	}
	//lint +e864
}

void NotepadPlusWrapper::hideTabBar()
{
	callNotepad(NPPM_HIDETABBAR, 0, TRUE);

}

void NotepadPlusWrapper::showTabBar()
{
	callNotepad(NPPM_HIDETABBAR, 0, FALSE);

}

intptr_t NotepadPlusWrapper::getCurrentBufferID()
{
	return callNotepad(NPPM_GETCURRENTBUFFERID);
}

void NotepadPlusWrapper::reloadBuffer(intptr_t bufferID, bool withAlert)
{
	if (checkForValidBuffer(bufferID))
	{
		callNotepad(NPPM_RELOADBUFFERID, static_cast<WPARAM>(bufferID), static_cast<LPARAM>(withAlert));
	}
	else
	{
		invalidValueProvided("invalid aka unknown bufferID provided");
	}
}

LangType NotepadPlusWrapper::getLangType()
{
	return getBufferLangType(callNotepad(NPPM_GETCURRENTBUFFERID));
}

LangType NotepadPlusWrapper::getBufferLangType(intptr_t bufferID)
{
	if (checkForValidBuffer(bufferID))
	{
		return static_cast<LangType>(callNotepad(NPPM_GETBUFFERLANGTYPE, bufferID));
	}
	else
	{
		invalidValueProvided("invalid aka unknown bufferID provided");
		//add some random return value to avoid compiler warning about missing return value
		return LangType::L_TEXT;
	}
}

void NotepadPlusWrapper::setBufferLangType(LangType language, intptr_t bufferID)
{
	callNotepad(NPPM_SETBUFFERLANGTYPE, static_cast<WPARAM>(bufferID), static_cast<LPARAM>(language));

}

BufferEncoding NotepadPlusWrapper::getEncoding()
{
	return getBufferEncoding(callNotepad(NPPM_GETCURRENTBUFFERID));
}

BufferEncoding NotepadPlusWrapper::getBufferEncoding(intptr_t bufferID)
{
	if (checkForValidBuffer(bufferID))
	{
		return static_cast<BufferEncoding>(callNotepad(NPPM_GETBUFFERENCODING, static_cast<WPARAM>(bufferID)));
	}
	else
	{
		invalidValueProvided("invalid aka unknown bufferID provided");
		//add some random return value to avoid compiler warning about missing return value
		return BufferEncoding::uniEnd;
	}
}

void NotepadPlusWrapper::setEncoding(BufferEncoding encoding)
{
	callNotepad(NPPM_SETBUFFERENCODING, static_cast<WPARAM>(callNotepad(NPPM_GETCURRENTBUFFERID)), static_cast<LPARAM>(encoding));

}

void NotepadPlusWrapper::setBufferEncoding(BufferEncoding encoding, intptr_t bufferID)
{
	callNotepad(NPPM_SETBUFFERENCODING, static_cast<WPARAM>(bufferID), static_cast<LPARAM>(encoding));

}

FormatType NotepadPlusWrapper::getFormatType()
{
	return getBufferFormatType(callNotepad(NPPM_GETCURRENTBUFFERID));
}

FormatType NotepadPlusWrapper::getBufferFormatType(intptr_t bufferID)
{
	if (checkForValidBuffer(bufferID))
	{
		return static_cast<FormatType>(callNotepad(NPPM_GETBUFFERFORMAT, static_cast<WPARAM>(bufferID)));
	}
	else
	{
		invalidValueProvided("invalid aka unknown bufferID provided");
		//add some random return value to avoid compiler warning about missing return value
		return FormatType::WIN_FORMAT;
	}
}

void NotepadPlusWrapper::setFormatType(FormatType format)
{
	setBufferFormatType(format, callNotepad(NPPM_GETCURRENTBUFFERID));

}

void NotepadPlusWrapper::setBufferFormatType(FormatType format, intptr_t bufferID)
{
	callNotepad(NPPM_SETBUFFERFORMAT, static_cast<WPARAM>(bufferID), static_cast<LPARAM>(format));

}

void NotepadPlusWrapper::closeDocument()
{
    notAllowedInScintillaCallback("close() cannot be called in a synchronous editor callback. "
		"Use an asynchronous callback, or avoid using close() in the callback handler");
	callNotepad(NPPM_MENUCOMMAND, 0, IDM_FILE_CLOSE);

}

void NotepadPlusWrapper::closeAllDocuments()
{
    notAllowedInScintillaCallback("closeAll() cannot be called in a synchronous editor callback. "
		"Use an asynchronous callback, or avoid using closeAll() in the callback handler");
	callNotepad(NPPM_MENUCOMMAND, 0, IDM_FILE_CLOSEALL);

}

void NotepadPlusWrapper::closeAllButCurrentDocument()
{
    notAllowedInScintillaCallback("closeAllButCurrent() cannot be called in a synchronous editor callback. "
		"Use an asynchronous callback, or avoid using closeAllButCurrent() in the callback handler");
	callNotepad(NPPM_MENUCOMMAND, 0, IDM_FILE_CLOSEALL_BUT_CURRENT);

}

void NotepadPlusWrapper::reloadCurrentDocument()
{
	callNotepad(NPPM_MENUCOMMAND, 0, IDM_FILE_RELOAD);

}

int NotepadPlusWrapper::messageBox(const char *message, const char *title, UINT flags)
{
	if (!message) { message = ""; }
	if (!title) { title = "Python Script for Notepad++"; }
	if (!flags) { flags = 0; }
	int retVal;
	GILRelease release;
	retVal = ::MessageBoxA(m_nppHandle, message, title, flags);


	return retVal;
}

boost::python::object NotepadPlusWrapper::prompt(boost::python::object promptObj, boost::python::object title, boost::python::object initial)
{
	PromptDialog promptDlg(m_hInst, m_nppHandle);
	const char *cPrompt = NULL;
	const char *cTitle = NULL;
	const char *cInitial = NULL;
	if (!promptObj | !promptObj.is_none())
		cPrompt = (const char *)boost::python::extract<const char *>(promptObj.attr("__str__")());
	if (!title | !title.is_none())
		cTitle= (const char *)boost::python::extract<const char *>(title.attr("__str__")());

	if (!initial | !initial.is_none())
		cInitial = (const char *)boost::python::extract<const char *>(initial.attr("__str__")());

	PromptDialog::PROMPT_RESULT result;
	GILRelease release;
	result = promptDlg.showPrompt(cPrompt, cTitle, cInitial);

    release.reacquire();
	if (PromptDialog::RESULT_OK == result)
	{
		return boost::python::str(promptDlg.getText());
	}
	else
	{
		return boost::python::object();
	}

}

void NotepadPlusWrapper::clearCallbackFunction(boost::python::object callback)
{
	for(callbackT::iterator it = m_callbacks.begin(); it != m_callbacks.end();)
	{
		if (callback.ptr() == it->second.ptr())
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

void NotepadPlusWrapper::clearCallbackEvents(boost::python::list events)
{
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

void NotepadPlusWrapper::clearCallback(boost::python::object callback, boost::python::list events)
{
	for(callbackT::iterator it = m_callbacks.begin(); it != m_callbacks.end(); )
	{
		if(it->second == callback && boost::python::extract<bool>(events.contains(it->first)))
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

void NotepadPlusWrapper::clearAllCallbacks()
{
	for(callbackT::iterator it = m_callbacks.begin(); it != m_callbacks.end(); )
	{
		it = m_callbacks.erase(it);
	}


	if (m_callbacks.empty())
	{
		m_notificationsEnabled = false;
	}
}

void NotepadPlusWrapper::activateBufferID(intptr_t bufferID)
{
    notAllowedInScintillaCallback("activateBufferID() cannot be called in a synchronous editor callback. "
		"Use an asynchronous callback, or avoid using activateBufferID() in the callback handler");
	idx_t index = (idx_t)callNotepad(NPPM_GETPOSFROMBUFFERID, static_cast<WPARAM>(bufferID));
	UINT view = (index & 0xC0000000) >> 30;
	index = index & 0x3FFFFFFF;

	callNotepad(NPPM_ACTIVATEDOC, view, (LPARAM)index);

}

boost::python::str NotepadPlusWrapper::getBufferFilename(intptr_t bufferID)
{
	TCHAR buffer[MAX_PATH];
	callNotepad(NPPM_GETFULLPATHFROMBUFFERID, static_cast<WPARAM>(bufferID), reinterpret_cast<LPARAM>(buffer));
	std::shared_ptr<char> filename = WcharMbcsConverter::tchar2char(buffer);
	return boost::python::str(const_cast<const char *>(filename.get()));
}

boost::python::str NotepadPlusWrapper::getCurrentFilename()
{
	idx_t bufferID = callNotepad(NPPM_GETCURRENTBUFFERID);
	TCHAR buffer[MAX_PATH];
	callNotepad(NPPM_GETFULLPATHFROMBUFFERID, bufferID, reinterpret_cast<LPARAM>(buffer));
	std::shared_ptr<char> filename = WcharMbcsConverter::tchar2char(buffer);
	return boost::python::str(const_cast<const char *>(filename.get()));
}

bool NotepadPlusWrapper::runPluginCommand(boost::python::str pluginName, boost::python::str menuOption, bool refreshCache)
{
	bool retVal = false;

	MenuManager *menuManager = MenuManager::getInstance();
	if (!pluginName.is_none() && !menuOption.is_none())
	{
		std::shared_ptr<TCHAR> tpluginName = WcharMbcsConverter::char2tchar(boost::python::extract<const char *>(pluginName));
		std::shared_ptr<TCHAR> tmenuOption = WcharMbcsConverter::char2tchar(boost::python::extract<const char *>(menuOption));
		GILRelease release;
		idx_t commandID = menuManager->findPluginCommand(tpluginName.get(), tmenuOption.get(), refreshCache);
		if (commandID)
		{
			::SendMessage(m_nppHandle, WM_COMMAND, commandID, 0);
			retVal = true;
		}

	}
	return retVal;

}

bool NotepadPlusWrapper::runMenuCommand(boost::python::str menuName, boost::python::str menuOption, bool refreshCache)
{
	bool retVal = false;
	MenuManager *menuManager = MenuManager::getInstance();
	if (!menuName.is_none() && !menuOption.is_none())
	{
		std::shared_ptr<TCHAR> tmenuName = WcharMbcsConverter::char2tchar(boost::python::extract<const char *>(menuName));
		std::shared_ptr<TCHAR> tmenuOption = WcharMbcsConverter::char2tchar(boost::python::extract<const char *>(menuOption));
		GILRelease release;
		idx_t commandID = menuManager->findMenuCommand(tmenuName.get(), tmenuOption.get(), refreshCache);
		if (commandID)
		{
			::SendMessage(m_nppHandle, WM_COMMAND, commandID, 0);
			retVal = true;
		}

	}
	return retVal;

}

boost::python::str NotepadPlusWrapper::getNppDir()
{
	TCHAR buffer[MAX_PATH];
    callNotepad(NPPM_GETNPPDIRECTORY, MAX_PATH, reinterpret_cast<LPARAM>(buffer));
	return boost::python::str(const_cast<const char *>(WcharMbcsConverter::tchar2char(buffer).get()));
}

boost::python::str NotepadPlusWrapper::getCommandLine()
{
	return boost::python::str(const_cast<const char *>(WcharMbcsConverter::tchar2char(::GetCommandLine()).get()));
}

bool NotepadPlusWrapper::allocateSupported()
{
	return 1 == callNotepad(NPPM_ALLOCATESUPPORTED);
}

boost::python::object NotepadPlusWrapper::allocateCmdID(int quantity)
{
	int startID;
	bool result = 1 == callNotepad(NPPM_ALLOCATECMDID, static_cast<WPARAM>(quantity), reinterpret_cast<LPARAM>(&startID));
	if (result)
	{
		return boost::python::object(startID);
	}
	else
	{
		return boost::python::object();
	}
}

boost::python::object NotepadPlusWrapper::allocateMarker(int quantity)
{
	int startID;
	bool result = 1 == callNotepad(NPPM_ALLOCATEMARKER, static_cast<WPARAM>(quantity), reinterpret_cast<LPARAM>(&startID));
	if (result)
	{
		return boost::python::object(startID);
	}
	else
	{
		return boost::python::object();
	}
}

LRESULT NotepadPlusWrapper::getMenuHandle(int menu = 0)
{
	return callNotepad(NPPM_GETMENUHANDLE, static_cast<WPARAM>(menu), 0);
}

bool NotepadPlusWrapper::isTabBarHidden()
{
	return 1 == callNotepad(NPPM_ISTABBARHIDDEN);
}

bool NotepadPlusWrapper::hideToolBar(bool hideOrNot)
{
	return callNotepad(NPPM_HIDETOOLBAR, 0, hideOrNot);
}

bool NotepadPlusWrapper::isToolBarHidden()
{
	return callNotepad(NPPM_ISTOOLBARHIDDEN);
}

bool NotepadPlusWrapper::hideMenu(bool hideOrNot)
{
	return callNotepad(NPPM_HIDEMENU, 0, hideOrNot);
}

bool NotepadPlusWrapper::isMenuHidden()
{
	return callNotepad(NPPM_ISMENUHIDDEN);
}

bool NotepadPlusWrapper::hideStatusBar(bool hideOrNot)
{
	return callNotepad(NPPM_HIDESTATUSBAR, 0, hideOrNot);
}

bool NotepadPlusWrapper::isStatusBarHidden()
{
	return callNotepad(NPPM_ISSTATUSBARHIDDEN);
}

void NotepadPlusWrapper::saveFile(boost::python::str filename)
{
	std::shared_ptr<TCHAR> s = WcharMbcsConverter::char2tchar((const char*)boost::python::extract<const char*>(filename));
	callNotepad(NPPM_SAVEFILE, 0, reinterpret_cast<LPARAM>(s.get()));
}

void NotepadPlusWrapper::showDocSwitcher(bool showOrNot)
{
	callNotepad(NPPM_SHOWDOCSWITCHER, 0, showOrNot);
}

bool NotepadPlusWrapper::isDocSwitcherShown()
{
	return callNotepad(NPPM_ISDOCSWITCHERSHOWN);
}

void NotepadPlusWrapper::docSwitcherDisableColumn(bool disableOrNot)
{
	callNotepad(NPPM_DOCSWITCHERDISABLECOLUMN, 0, disableOrNot);
}

intptr_t NotepadPlusWrapper::getCurrentNativeLangEncoding()
{
	return callNotepad(NPPM_GETCURRENTNATIVELANGENCODING, 0, 0);
}

boost::python::str NotepadPlusWrapper::getLanguageName(int langType)
{
	int size = callNotepad(NPPM_GETLANGUAGENAME, langType, NULL);
	wchar_t* result(new wchar_t[size]);
	callNotepad(NPPM_GETLANGUAGENAME, langType, reinterpret_cast<LPARAM>(result));
	std::shared_ptr<char> languageName = WcharMbcsConverter::tchar2char(result);
	return boost::python::str(const_cast<const char *>(languageName.get()));
}

boost::python::str NotepadPlusWrapper::getLanguageDesc(int langType)
{
	int size = callNotepad(NPPM_GETLANGUAGEDESC, langType, NULL);
	wchar_t* result(new wchar_t[size]);
	callNotepad(NPPM_GETLANGUAGEDESC, langType, reinterpret_cast<LPARAM>(result));
	std::shared_ptr<char> languageName = WcharMbcsConverter::tchar2char(result);
	return boost::python::str(const_cast<const char *>(languageName.get()));
}

bool NotepadPlusWrapper::getAppdataPluginsAllowed()
{
	return callNotepad(NPPM_GETAPPDATAPLUGINSALLOWED, 0, 0);
}

boost::python::tuple NotepadPlusWrapper::getEditorDefaultForegroundColor()
{
	int retVal = (int)callNotepad(NPPM_GETEDITORDEFAULTFOREGROUNDCOLOR,0,0);
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

boost::python::tuple NotepadPlusWrapper::getEditorDefaultBackgroundColor()
{
	int retVal = (int)callNotepad(NPPM_GETEDITORDEFAULTBACKGROUNDCOLOR, 0, 0);
	return boost::python::make_tuple(COLOUR_RED(retVal), COLOUR_GREEN(retVal), COLOUR_BLUE(retVal));
}

void NotepadPlusWrapper::setSmoothFont(bool setSmoothFontOrNot)
{
	callNotepad(NPPM_SETSMOOTHFONT, 0, setSmoothFontOrNot);
}

void NotepadPlusWrapper::setEditorBorderEdge(bool withEditorBorderEdgeOrNot)
{
	callNotepad(NPPM_SETEDITORBORDEREDGE, 0, withEditorBorderEdgeOrNot);
}

intptr_t NotepadPlusWrapper::getNbUserLang()
{
	return callNotepad(NPPM_GETNBUSERLANG);
}

intptr_t NotepadPlusWrapper::encodeSci(int view)
{
	return callNotepad(NPPM_ENCODESCI, view, 0);
}

intptr_t NotepadPlusWrapper::decodeSci(int view)
{
	return callNotepad(NPPM_DECODESCI, view, 0);
}

void NotepadPlusWrapper::launchFindInFilesDlg(std::wstring dir2Search, std::wstring filter)
{
	callNotepad(NPPM_LAUNCHFINDINFILESDLG, reinterpret_cast<WPARAM>(dir2Search.c_str()), reinterpret_cast<LPARAM>(filter.c_str()));
}

winVer NotepadPlusWrapper::getWindowsVersion()
{
	return static_cast<winVer>(callNotepad(NPPM_GETWINDOWSVERSION, 0, 0));
}

bool NotepadPlusWrapper::makeCurrentBufferDirty()
{
	return callNotepad(NPPM_MAKECURRENTBUFFERDIRTY);
}

bool NotepadPlusWrapper::getEnableThemeTextureFunc()
{
	return callNotepad(NPPM_GETENABLETHEMETEXTUREFUNC);
}

void NotepadPlusWrapper::triggerTabbarContextMenu(int view, int index2Activate)
{
	callNotepad(NPPM_TRIGGERTABBARCONTEXTMENU, view, index2Activate);
}

void NotepadPlusWrapper::disableAutoUpdate()
{
	callNotepad(NPPM_DISABLEAUTOUPDATE, 0, 0);
}

bool NotepadPlusWrapper::isSingleView()
{
	HWND splitter_hwnd = FindWindowEx(m_nppHandle, NULL, L"splitterContainer", NULL);
	return !IsWindowVisible(splitter_hwnd);
}

void NotepadPlusWrapper::flashWindow(UINT count, DWORD milliseconds)
{
	FLASHWINFO flashinfo;
	flashinfo.cbSize = sizeof(flashinfo);
	flashinfo.hwnd = m_nppHandle;
	flashinfo.dwFlags = FLASHW_ALL;
	flashinfo.dwTimeout = milliseconds;
	flashinfo.uCount = count;

	FlashWindowEx(&flashinfo);
}

void NotepadPlusWrapper::notAllowedInScintillaCallback(const char *message)
{
    DWORD currentThreadID = ::GetCurrentThreadId();


    if (currentThreadID == g_mainThreadID && ScintillaCallbackCounter::isInCallback())
	{
        throw NotAllowedInCallbackException(message);
	}
}

bool NotepadPlusWrapper::checkForValidBuffer(intptr_t bufferID)
{
	if(callNotepad(NPPM_GETPOSFROMBUFFERID, static_cast<WPARAM>(bufferID)) == -1)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void NotepadPlusWrapper::invalidValueProvided(const char *message)
{
	throw InvalidValueProvidedException(message);
}

}
