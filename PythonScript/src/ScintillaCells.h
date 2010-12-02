#ifndef _SCINTILLACELL_H
#define _SCINTILLACELL_H

class ScintillaCells 
{
public:
	ScintillaCells(boost::python::str characters, boost::python::list styles);
	~ScintillaCells();

	unsigned char* cells() const { return m_cells.get(); }
	size_t length() const        { return m_length * 2; }

private:
	ScintillaCells(); // default constructor disabled

	std::shared_ptr<unsigned char> m_cells;
	size_t m_length;
};

#endif
