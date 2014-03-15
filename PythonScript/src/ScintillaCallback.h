#ifndef SCINTILLACALLBACK_20140303_H
#define SCINTILLACALLBACK_20140303_H

namespace NppPythonScript
{

    class ScintillaCallback
	{
	public:
        ScintillaCallback(boost::python::object callback, bool isAsync)
			: m_callback(callback),
              m_isAsync(isAsync)
		{}


		boost::python::object getCallback() { return m_callback; }
		bool isAsync() { return m_isAsync; }

	private:
		boost::python::object m_callback;
        bool m_isAsync;
	};
}

#endif // SCINTILLACALLBACK_20140303_H 