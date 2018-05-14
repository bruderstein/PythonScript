#ifndef INVALIDVALUEPROVIDEDEXCEPTION_20180413_H
#define INVALIDVALUEPROVIDEDEXCEPTION_20180413_H

namespace NppPythonScript
{

	class InvalidValueProvidedException
	{
	public:
		explicit InvalidValueProvidedException(const char *desc)
			: m_desc(desc)
			{};

		const char *what() const
		{
			return m_desc.c_str();
		}

	private:
		InvalidValueProvidedException(); // default constructor disabled

		std::string m_desc;
	};



	void translateInvalidValueProvidedException(const InvalidValueProvidedException &e);

}

#endif // INVALIDVALUEPROVIDEDEXCEPTION_20140214_H