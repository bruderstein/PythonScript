#include "stdafx.h" //ADDED BY PYTHONSCRIPT
#include "NppDarkMode.h"

namespace NppDarkMode
{
	bool isEnabled()
	{
		return false;
	}

	HBRUSH getDarkerBackgroundBrush()
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
