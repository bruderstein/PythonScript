#include "stdafx.h"

#include "ScintillaWrapper.h"
#include "Scintilla.h"

#include <boost/python.hpp>

using namespace boost::python;

ScintillaWrapper::ScintillaWrapper()
	: m_handle(NULL)
{
}


ScintillaWrapper::ScintillaWrapper(const HWND handle)
	: m_handle(handle)
{
}

ScintillaWrapper::~ScintillaWrapper()
{
}

void ScintillaWrapper::addText(int length, const char *s)
{
	call(SCI_ADDTEXT, length, reinterpret_cast<LPARAM>(s));
}




