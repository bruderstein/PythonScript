#include "stdafx.h"

#include <boost/python.hpp>

struct ScintillaCell;

class ScintillaWrapper 
{
public:
	ScintillaWrapper(HWND handle);
	virtual ~ScintillaWrapper();

	void setHandle(const HWND handle) { m_handle = handle; };
	
	void addText(boost::python::str s);
	boost::python::str getLine(int lineNumber);
	void AddStyledText(boost::python::list s);

protected:
	// Call wrapper
	LRESULT ScintillaWrapper::call(UINT message, WPARAM wParam = 0, LPARAM lParam = 0)
	{
		return SendMessage(m_handle, message, wParam, lParam);
	};
	
	
private:
	// Active Scintilla handle
	HWND m_handle;
	
	// Temporary buffer for returned strings
	int m_tempStringLength;
	char * m_tempString;

	// Private methods
	char *allocateString(int length);

	
	
};

