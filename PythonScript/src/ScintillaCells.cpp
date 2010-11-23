#include "stdafx.h"

#include "ScintillaCells.h"

ScintillaCells::ScintillaCells(boost::python::str characters, boost::python::list styles)
{
	m_length = len(characters);
	m_cells.reset(new unsigned char[(m_length)*2]);
	const char *chrs = boost::python::extract<const char*>(characters);
	int styleLength = len(styles);
	int style = 0;

	for(int pos = 0; pos < m_length; pos++)
	{
		if (pos < styleLength)
			style = boost::python::extract<int>(styles[pos]);

		m_cells.get()[pos*2] = chrs[pos];
		m_cells.get()[(pos*2) + 1] = static_cast<unsigned char>(style);
	}
}

ScintillaCells::~ScintillaCells()
{
	
}


