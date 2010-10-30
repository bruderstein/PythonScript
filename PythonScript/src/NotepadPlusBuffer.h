#ifndef _NOTEPADPLUSBUFFER_H
#define _NOTEPADPLUSBUFFER_H

enum DOCUMENTSTATUS
{
	DOCSTA_SAVED,
	DOCSTA_UNSAVED,
	DOCSTA_READONLY
};

class NotepadPlusBuffer
{
public:
	NotepadPlusBuffer();
	NotepadPlusBuffer(const TCHAR *filename);
	NotepadPlusBuffer(int bufferID);

	int getBufferID();
	void setBufferID(int bufferID);
	
	tstring& getFilename();
	void setFilename(const TCHAR *filename);

	DOCUMENTSTATUS getStatus();
	void setStatus(DOCUMENTSTATUS status);

	int getView();
	void setView(int view);

private:
	int m_view;
	int m_bufferID;
	tstring m_filename;
	DOCUMENTSTATUS m_status;
};

#endif