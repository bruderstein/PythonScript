#ifndef NOTALLOWEDINCALLBACKEXCEPTION_20140311_H
#define NOTSUPPORTEDEXCEPTION_20140214_H

namespace NppPythonScript
{

class NotAllowedInCallbackException
{
public:
	explicit NotAllowedInCallbackException(const char *desc)
		: m_desc(desc) 
		{};
	
	const char *what() const
	{ return m_desc.c_str();
	}
	
private:
	NotAllowedInCallbackException(); // default constructor disabled

	std::string m_desc;
};



void translateNotAllowedInCallbackException(const NotAllowedInCallbackException &e);

}

#endif // NOTALLOWEDINCALLBACKEXCEPTION_20140311_H