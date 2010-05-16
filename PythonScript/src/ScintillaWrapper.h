#include "stdafx.h"

class ScintillaWrapper 
{
public:
	ScintillaWrapper();
	ScintillaWrapper(HWND handle);
	virtual ~ScintillaWrapper();

	void setHandle(const HWND handle) { m_handle = handle; };
	
	void addText(int length, const char *s);

protected:
	// Call wrapper
	LRESULT ScintillaWrapper::call(UINT message, WPARAM wParam = 0, LPARAM lParam = 0)
	{
		return SendMessage(m_handle, message, wParam, lParam);
	};
	
	
private:
	// Active Scintilla handle
	HWND m_handle;

	
};

