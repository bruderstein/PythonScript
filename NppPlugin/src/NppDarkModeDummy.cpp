#include "stdafx.h" //ADDED BY PYTHONSCRIPT
#include "NppDarkMode.h"

namespace NppDarkMode
{
	bool isEnabled()
	{
		return false;
	}

	HBRUSH getDlgBackgroundBrush()
	{
		return 0;
	}

	void setDarkTitleBar(HWND /*hwnd*/)
	{
	}

	bool isWindows10()
	{
		return true;
	}
}
