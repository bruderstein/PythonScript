#include "stdafx.h"

#include "Python.h"
#include <boost/python.hpp>


#include "ScintillaCells.h"

using namespace boost::python;

ScintillaCells::ScintillaCells(str characters, list styles)
{
	m_length = len(characters);
	m_cells.reset(new unsigned char[(m_length)*2]);
	const char *chrs = extract<const char*>(characters);
	int styleLength = len(styles);
	int style = 0;

	for(int pos = 0; pos < m_length; pos++)
	{
		if (pos < styleLength)
			style = extract<int>(styles[pos]);

		m_cells.get()[pos*2] = chrs[pos];
		m_cells.get()[(pos*2) + 1] = static_cast<unsigned char>(style);
	}

	
}

ScintillaCells::~ScintillaCells()
{
	
}


