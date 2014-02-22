#ifndef CALLBACKEXECARGS_20140217_H
#define CALLBACKEXECARGS_20140217_H


namespace NppPythonScript
{

class CallbackExecArgs
{
public:
    CallbackExecArgs() 
        : m_callbacks(new std::list<boost::python::object>()),
          m_params(NULL)
	{}

    virtual ~CallbackExecArgs();
	
	void addCallback(boost::python::object callback) { m_callbacks->push_back(callback); }
    void setParams(boost::python::dict params);

	std::list<boost::python::object> *getCallbacks() { return m_callbacks; }
	boost::python::dict *getParams() { return m_params; }
private:
	std::list<boost::python::object> *m_callbacks;
	boost::python::dict *m_params;
};

}

#endif // CALLBACKEXECARGS_20140217_H