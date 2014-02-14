#ifndef GROUPNOTFOUNDEXCEPTION_20140214_H
#define GROUPNOTFOUNDEXCEPTION_20140214_H

namespace NppPythonScript
{

class GroupNotFoundException
{
public:
	explicit GroupNotFoundException(const char *desc)
		: m_desc(desc) 
		{};
	
	const char *what() const
	{ return m_desc.c_str();
	}
	
private:
	GroupNotFoundException(); // default constructor disabled

	std::string m_desc;
};


void translateGroupNotFoundException(const GroupNotFoundException &e);

}

#endif // GROUPNOTFOUNDEXCEPTION_20140214_H