#include "stdafx.h"
#include "CallbackExecArgs.h"
#include "GILManager.h"


namespace NppPythonScript
{

    CallbackExecArgs::~CallbackExecArgs()
	{
        GILLock gilLock = GILManager::getGIL();
        delete m_callbacks;
        if (NULL != m_params)
		{
            delete m_params;
		}
	}


    void CallbackExecArgs::setParams(boost::python::dict params)
	{
        if (NULL != params)
		{
            delete m_params;
		}
        m_params = new boost::python::dict(params);
	}
}
