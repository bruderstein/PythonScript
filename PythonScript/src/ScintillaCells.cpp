#include "stdafx.h"

#include "ScintillaCells.h"

//lint -e1732 (Info -- new in constructor for class 'ScintillaCells' which has no assignment operator -- Effective C++ #11)
//lint -e1733 (Info -- new in constructor for class 'ScintillaCells' which has no copy constructor -- Effective C++ #11)
// The allocated memory is placed inside a shared_ptr. The default copy constructor and assignment operator will do just fine.
ScintillaCells::ScintillaCells(boost::python::str characters, boost::python::list styles) :
	m_length(_len(characters)),
	m_cells(new unsigned char[m_length*2])
{
	const char *chrs = boost::python::extract<const char*>(characters);
	size_t styleLength = _len(styles);
	int style = 0;

	for(idx_t pos = 0; pos < m_length; pos++)
	{
		if (pos < styleLength)
		{
			style = boost::python::extract<int>(styles[pos]);
		}

		m_cells.get()[pos*2] = (unsigned char)chrs[pos];
		m_cells.get()[(pos*2) + 1] = static_cast<unsigned char>(style);
	}
}
//lint +e1732
//lint +e1733

ScintillaCells::~ScintillaCells()
{
	
}


