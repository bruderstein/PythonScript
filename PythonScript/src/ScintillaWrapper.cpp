#include "stdafx.h"

#include "ScintillaWrapper.h"
#include "Scintilla.h"

#include <boost/python.hpp>

using namespace boost::python;
#include "ScintillaCells.h"

ScintillaWrapper::ScintillaWrapper(const HWND handle)
	: m_handle(handle),
	  m_tempString(NULL),
	  m_tempStringLength(0)
{
}

ScintillaWrapper::~ScintillaWrapper()
{
}

void ScintillaWrapper::addText(str s)
{
	const char *raw = extract<const char*>(s);
	call(SCI_ADDTEXT, len(s), reinterpret_cast<LPARAM>(raw));
}
/*
void ScintillaWrapper::AddStyledText(ScintillaCells s)
{
	std::size_t n = len(s);
	ScintillaCell* tmp = new ScintillaCell[n];
    for (int i = 0; i < n; i++)
	{
        tmp[i] = extract<ScintillaCell>(s[i]);
    }
	call(SCI_ADDSTYLEDTEXT, n * 2, reinterpret_cast<LPARAM>(tmp));
	delete [] tmp;
}
*/
str ScintillaWrapper::getLine(int lineNumber)
{
	int resultLength = call(SCI_GETLINE, lineNumber, NULL);
	char * result = (char *)malloc(resultLength + 1);
	call(SCI_GETLINE, lineNumber, reinterpret_cast<LPARAM>(result));
	result[resultLength] = '\0';
	str o = str((const char *)result);
	free(result);
	return o;
}

char *ScintillaWrapper::allocateString(int length)
{
	if (m_tempStringLength > length)
		return m_tempString;

	if (NULL != m_tempString)
		free(m_tempString);

	m_tempString = (char *) malloc(length);
	m_tempStringLength = length;
	return m_tempString;
}

