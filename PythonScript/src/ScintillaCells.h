#ifndef _SCINTILLACELL_H
#define _SCINTILLACELL_H

#include <boost/python.hpp>
#include <memory>

class ScintillaCells 
{
public:
	ScintillaCells(boost::python::str characters, boost::python::list styles);
	~ScintillaCells();

	unsigned char* cells() { return m_cells.get();      };
	int length()           { return m_length * 2; };

private:
	std::shared_ptr<unsigned char> m_cells;
	int m_length;
};

#endif