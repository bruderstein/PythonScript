#ifndef ARGUMENTEXCEPTION_20140214_H
#define ARGUMENTEXCEPTION_20140214_H

namespace NppPythonScript
{

class ArgumentException
{
public:
	explicit ArgumentException(const char *desc)
		: m_desc(desc) 
		{};
	
	const char *what() const
	{ return m_desc.c_str();
	}
	
private:
	ArgumentException(); // default constructor disabled

	std::string m_desc;
};


void translateArgumentException(const ArgumentException &e);

}

#endif // ARGUMENTEXCEPTION_20140214_H