#include "stdafx.h"

#include "WcharMbcsConverter.h"
#include "NotepadPlusWrapper.h"
#include "menuCmdID.h"
#include "PromptDialog.h"

using namespace std;
using namespace boost::python;

NotepadPlusWrapper::NotepadPlusWrapper(HINSTANCE hInst, HWND nppHandle)
	: m_hInst(hInst),
	  m_nppHandle(nppHandle),
	  m_notificationsEnabled(false)
{
	
}
	
NotepadPlusWrapper::~NotepadPlusWrapper()
{
	callbackT::iterator iter = m_callbacks.begin();
	while (iter != m_callbacks.end())
	{
		Py_XDECREF(iter->second);
	}

	m_callbacks.clear();
	m_notificationsEnabled = false;
}


void NotepadPlusWrapper::notify(SCNotification *notifyCode)
{
	if (!m_notificationsEnabled)
		return;

	pair<callbackT::iterator, callbackT::iterator> callbackIter 
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
			params["bufferID"] = notifyCode->nmhdr.idFrom;
			break;

		case NPPN_FILEBEFORELOAD:
		case NPPN_READY:
		case NPPN_SHUTDOWN:
		case NPPN_TBMODIFICATION:  // I hope no-one ever has to use this!
			// No additional parameters
			break;

		case NPPN_READONLYCHANGED:
			params["bufferID"] = notifyCode->nmhdr.hwndFrom;
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

		
		default:
			// Unknown notification, so just fill in the parameters as integers.
			params["idFrom"] = notifyCode->nmhdr.idFrom;
			params["hwndFrom"] = notifyCode->nmhdr.hwndFrom;
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


bool NotepadPlusWrapper::callback(PyObject* callback, boost::python::list events)
{
	if (PyCallable_Check(callback))
	{
		int eventCount = len(events);
		for(int i = 0; i < eventCount; i++)
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

void NotepadPlusWrapper::save()
{
	callNotepad(NPPM_SAVECURRENTFILE);
}


void NotepadPlusWrapper::newDocument()
{
	callNotepad(NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);
}

void NotepadPlusWrapper::newDocumentWithFilename(const char *filename)
{
	callNotepad(NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);
	shared_ptr<TCHAR> tFilename = WcharMbcsConverter::char2tchar(filename);
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
	callNotepad(NPPM_DOOPEN, 0, reinterpret_cast<LPARAM>(WcharMbcsConverter::char2tchar(filename).get()));
}

bool NotepadPlusWrapper::activateFile(const char *filename)
{
	return (bool)callNotepad(NPPM_SWITCHTOFILE, 0, reinterpret_cast<LPARAM>(WcharMbcsConverter::char2tchar(filename).get()));
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

boost::python::list NotepadPlusWrapper::getFiles()
{
	int count;
	int bufferID;

	boost::python::list files;

	for(int view = 0; view <= 1; view++)
	{
		count = callNotepad(NPPM_GETNBOPENFILES, 0, view);
	
		TCHAR **fileNames = (TCHAR **)new TCHAR*[count];
		for (int i = 0 ; i < count ; i++)
		{
			fileNames[i] = new TCHAR[MAX_PATH];
		}

		if (callNotepad(view ? NPPM_GETOPENFILENAMESSECOND : NPPM_GETOPENFILENAMESPRIMARY, 
							reinterpret_cast<WPARAM>(fileNames), static_cast<LPARAM>(count)))
		{
			for(int pos = 0; pos < count; pos++)
			{
				bufferID = callNotepad(NPPM_GETBUFFERIDFROMPOS, pos, view);
				if (bufferID)
				{
#ifdef UNICODE
					shared_ptr<char> mbFilename = WcharMbcsConverter::tchar2char(fileNames[pos]);
					files.append(make_tuple(const_cast<const char*>(mbFilename.get()), bufferID, pos, view));
#else
					files.append(make_tuple(const_cast<const char*>(fileNames[pos]), bufferID, pos, view));
#endif
				}
			}
		}

		for (int i = 0 ; i < count ; i++)
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

	int count = callNotepad(NPPM_GETNBSESSIONFILES, 0, reinterpret_cast<LPARAM>(sessionFilename));
	if (count > 0)
	{
		TCHAR **fileNames = (TCHAR **)new TCHAR*[count];
		for (int pos = 0 ; pos < count ; pos++)
		{
			fileNames[pos] = new TCHAR[MAX_PATH];
		}

		if (callNotepad(NPPM_GETSESSIONFILES, 0, reinterpret_cast<LPARAM>(fileNames)))
		{

			for(int pos = 0; pos < count; pos++)
			{
#ifdef UNICODE
					shared_ptr<char> mbFilename = WcharMbcsConverter::tchar2char(fileNames[pos]);
					result.append(const_cast<const char*>(mbFilename.get()));
#else
					result.append(const_cast<const char*>(fileNames[pos]));
#endif
			}

		}
	}

	return result;

}


void NotepadPlusWrapper::saveSession(const char *sessionFilename, boost::python::list files)
{
	sessionInfo si;
	
	si.sessionFilePathName = WcharMbcsConverter::char2tchar(sessionFilename).get();
	
	int filesCount = len(files);
	si.files = (TCHAR **)new TCHAR*[filesCount];

	// Vector to store temporary list of shared_ptr 
	vector< shared_ptr<TCHAR> > filesList(filesCount);

	for(int pos = 0; pos < filesCount; pos++)
	{
		filesList[pos] = WcharMbcsConverter::char2tchar(static_cast<const char*>(extract<const char *>(files[0])));
		si.files[pos] = filesList[pos].get();
	}
	
	si.nbFile = filesCount;

	callNotepad(NPPM_SAVESESSION, 0, reinterpret_cast<LPARAM>(&si));

	for(int pos = 0; pos < filesCount; pos++)
	{
		filesList[pos].reset();
	}
}


void NotepadPlusWrapper::saveCurrentSession(const char *filename)
{
	callNotepad(NPPM_SAVECURRENTSESSION, 0, reinterpret_cast<LPARAM>(WcharMbcsConverter::char2tchar(filename).get()));
}

ScintillaWrapper NotepadPlusWrapper::createScintilla()
{
	LRESULT handle = callNotepad(NPPM_CREATESCINTILLAHANDLE, 0, NULL);
	
	// return copy
	return ScintillaWrapper((HWND)handle);
}

void NotepadPlusWrapper::destroyScintilla(ScintillaWrapper& buffer)
{
	callNotepad(NPPM_DESTROYSCINTILLAHANDLE, 0, reinterpret_cast<LPARAM>(buffer.getHandle()));
	buffer.invalidateHandle();
}

int NotepadPlusWrapper::getCurrentDocIndex(int view)
{
	return callNotepad(NPPM_GETCURRENTDOCINDEX, view);
}


void NotepadPlusWrapper::setStatusBar(StatusBarSection section, const char *text)
{
	callNotepad(NPPM_SETSTATUSBAR, static_cast<WPARAM>(section), reinterpret_cast<LPARAM>(text));
}


long NotepadPlusWrapper::getPluginMenuHandle()
{
	return callNotepad(NPPM_GETMENUHANDLE, 0, 0);
}

void NotepadPlusWrapper::activateIndex(int view, int index)
{
	callNotepad(NPPM_ACTIVATEDOC, view, index);
}

void NotepadPlusWrapper::loadSession(str filename)
{

#ifdef UNICODE
	shared_ptr<TCHAR> s = WcharMbcsConverter::char2tchar((const char*)extract<const char*>(filename));
	callNotepad(NPPM_LOADSESSION, 0, reinterpret_cast<LPARAM>(s.get()));
#else
	callNotepad(NPPM_LOADSESSION, 0, reinterpret_cast<LPARAM>((const char*)extract<const char*>(filename)));
#endif

}

void NotepadPlusWrapper::activateFileString(str filename)
{
	#ifdef UNICODE
	shared_ptr<TCHAR> s = WcharMbcsConverter::char2tchar((const char*)extract<const char*>(filename));
	callNotepad(NPPM_SWITCHTOFILE, 0, reinterpret_cast<LPARAM>(s.get()));
#else
	callNotepad(NPPM_SWITCHTOFILE, 0, reinterpret_cast<LPARAM>((const char*)extract<const char*>(filename)));
#endif
}


void NotepadPlusWrapper::reloadFile(str filename, bool alert)
{
#ifdef UNICODE
	callNotepad(NPPM_RELOADFILE, alert ? 1 : 0, reinterpret_cast<LPARAM>(static_cast<const TCHAR *>(WcharMbcsConverter::char2tchar(extract<const char *>(filename)).get())));
#else
	callNotepad(NPPM_RELOADFILE, alert ? 1 : 0, reinterpret_cast<LPARAM>(static_cast<const char *>(extract<const char *>(filename))));
#endif
}


void NotepadPlusWrapper::saveAllFiles()
{
	callNotepad(NPPM_SAVEALLFILES);
}

str NotepadPlusWrapper::getPluginConfigDir()
{
	TCHAR temp[MAX_PATH];
	callNotepad(NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(temp));
	return str(WcharMbcsConverter::tchar2char(temp).get());
}

void NotepadPlusWrapper::menuCommand(int commandID)
{
	callNotepad(NPPM_MENUCOMMAND, 0, commandID);
}

tuple NotepadPlusWrapper::getVersion()
{
	LPARAM nppVersion = callNotepad(NPPM_GETNPPVERSION);
	
	int majorVersion = HIWORD(nppVersion);
	int minorVersion = LOWORD(nppVersion);

	vector<int> version(4);
	
	version[0] = majorVersion;

	char tmp[10];
	_itoa_s(minorVersion, tmp, 10, 10);
	
	int i;
	for(i = 0; tmp[i] && i < 4; i++)
	{
		version[i+1] = tmp[i] - 48;
	}

	switch(i)
	{
		case 3:
			return make_tuple<int, int, int, int>(version[0], version[1], version[2], version[3]);

		case 2:
			return make_tuple<int, int, int>(version[0], version[1], version[2]);

		case 1:
			return make_tuple<int, int>(version[0], version[1]);
		
		case 0:
		default:
			return make_tuple<int>(version[0]);
	}
}


void NotepadPlusWrapper::hideTabBar()
{
	callNotepad(NPPM_HIDETABBAR, 0, TRUE);
}

void NotepadPlusWrapper::showTabBar()
{
	callNotepad(NPPM_HIDETABBAR, 0, FALSE);
}

int NotepadPlusWrapper::getCurrentBufferID()
{
	return callNotepad(NPPM_GETCURRENTBUFFERID);
}

void NotepadPlusWrapper::reloadBuffer(int bufferID, bool withAlert)
{
	callNotepad(NPPM_RELOADBUFFERID, bufferID, static_cast<LPARAM>(withAlert));
}

LangType NotepadPlusWrapper::getLangType()
{
	return getBufferLangType(callNotepad(NPPM_GETCURRENTBUFFERID));
}

LangType NotepadPlusWrapper::getBufferLangType(int bufferID)
{
	return static_cast<LangType>(callNotepad(NPPM_GETBUFFERLANGTYPE, bufferID));
}

void NotepadPlusWrapper::setLangType(LangType language)
{
	setBufferLangType(language, callNotepad(NPPM_GETCURRENTBUFFERID));
}

void NotepadPlusWrapper::setBufferLangType(LangType language, int bufferID)
{
	callNotepad(NPPM_SETBUFFERLANGTYPE, bufferID, static_cast<LPARAM>(language));
}

BufferEncoding NotepadPlusWrapper::getEncoding()
{
	return getBufferEncoding(callNotepad(NPPM_GETCURRENTBUFFERID));
}

BufferEncoding NotepadPlusWrapper::getBufferEncoding(int bufferID)
{
	return static_cast<BufferEncoding>(callNotepad(NPPM_GETBUFFERENCODING, bufferID));
}

void NotepadPlusWrapper::setEncoding(BufferEncoding encoding)
{
	setBufferEncoding(encoding, callNotepad(NPPM_GETCURRENTBUFFERID));
}

void NotepadPlusWrapper::setBufferEncoding(BufferEncoding encoding, int bufferID)
{
	callNotepad(NPPM_SETBUFFERENCODING, bufferID, static_cast<LPARAM>(encoding));
}

FormatType NotepadPlusWrapper::getFormatType()
{
	return getBufferFormatType(callNotepad(NPPM_GETCURRENTBUFFERID));
}


FormatType NotepadPlusWrapper::getBufferFormatType(int bufferID)
{
	return static_cast<FormatType>(callNotepad(NPPM_GETBUFFERFORMAT, bufferID));
}

void NotepadPlusWrapper::setFormatType(FormatType format)
{
	setBufferFormatType(format, callNotepad(NPPM_GETCURRENTBUFFERID));
}

void NotepadPlusWrapper::setBufferFormatType(FormatType format, int bufferID)
{
	callNotepad(NPPM_SETBUFFERFORMAT, bufferID, static_cast<LPARAM>(format));
}

void NotepadPlusWrapper::closeDocument()
{
	callNotepad(NPPM_MENUCOMMAND, 0, IDM_FILE_CLOSE);
}

void NotepadPlusWrapper::closeAllDocuments()
{
	callNotepad(NPPM_MENUCOMMAND, 0, IDM_FILE_CLOSEALL);
}

void NotepadPlusWrapper::closeAllButCurrentDocument()
{
	callNotepad(NPPM_MENUCOMMAND, 0, IDM_FILE_CLOSEALL_BUT_CURRENT);
}

void NotepadPlusWrapper::reloadCurrentDocument()
{
	callNotepad(NPPM_MENUCOMMAND, 0, IDM_FILE_RELOAD);
}


int NotepadPlusWrapper::messageBox(const char *message, const char *title, int flags)
{
	return ::MessageBoxA(m_nppHandle, message, title, flags);
}



boost::python::object NotepadPlusWrapper::prompt(boost::python::object promptObj, boost::python::object title, boost::python::object initial)
{
	PromptDialog promptDlg(m_hInst, m_nppHandle);
	const char *cPrompt = NULL;
	const char *cTitle = NULL;
	const char *cInitial = NULL;
	if (!promptObj.is_none())
		cPrompt = (const char *)extract<const char *>(promptObj.attr("__str__")());
	if (!title.is_none())
		cTitle= (const char *)extract<const char *>(title.attr("__str__")());
	
	if (!initial.is_none())
		cInitial = (const char *)extract<const char *>(initial.attr("__str__")());

	if (PromptDialog::RESULT_OK == promptDlg.prompt(cPrompt, cTitle, cInitial))
	{
		return str(promptDlg.getText());
	}
	else
	{
		return object();
	}

}



void NotepadPlusWrapper::clearCallbackFunction(PyObject* callback)
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
	
void NotepadPlusWrapper::clearCallbackEvents(boost::python::list events)
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
	

void NotepadPlusWrapper::clearCallback(PyObject* callback, boost::python::list events)
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

void NotepadPlusWrapper::clearAllCallbacks()
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


void NotepadPlusWrapper::activateBufferID(int bufferID)
{
	int index = callNotepad(NPPM_GETPOSFROMBUFFERID, bufferID);
	int view = (index & 0xC0000000) >> 30;
	index = index & 0x3FFFFFFF;
	callNotepad(NPPM_ACTIVATEDOC, view, index);
}


boost::python::str NotepadPlusWrapper::getCurrentFilename()
{
	int bufferID = callNotepad(NPPM_GETCURRENTBUFFERID);
	TCHAR buffer[MAX_PATH];
	callNotepad(NPPM_GETFULLPATHFROMBUFFERID, bufferID, reinterpret_cast<LPARAM>(buffer));
	shared_ptr<char> filename = WcharMbcsConverter::tchar2char(buffer);
	return str(const_cast<const char *>(filename.get()));
}


