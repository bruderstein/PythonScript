#ifndef _HELPCONTROLLER_H
#define _HELPCONTROLLER_H

class HelpController
{
public:
	HelpController(HWND hNotepad, HWND hScintilla);

	void callHelp();
private:
	HelpController(); // default constructor disabled

	HWND m_hNotepad;
	HWND m_hScintilla;


	std::string getFilename();

	std::string getTopicUrl();
};

#endif
