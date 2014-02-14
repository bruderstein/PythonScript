#ifndef NOTSUPPORTEDEXCEPTION_20140214_H
#define NOTSUPPORTEDEXCEPTION_20140214_H

namespace NppPythonScript
{

class NotSupportedException
{
public:
	explicit NotSupportedException(const char *desc)
		: m_desc(desc) 
		{};
	
	const char *what() const
	{ return m_desc.c_str();
	}
	
private:
	NotSupportedException(); // default constructor disabled

	std::string m_desc;
};


boost::python::object py_not_supported(boost::python::tuple args, boost::python::dict kwargs);
void translateNotSupportedException(const NotSupportedException &e);

}

#endif // NOTSUPPORTEDEXCEPTION_20140214_H