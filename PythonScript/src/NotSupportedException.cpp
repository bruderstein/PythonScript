#include "stdafx.h"

#include "NotSupportedException.h"


namespace NppPythonScript
{

boost::python::object py_not_supported(boost::python::tuple /*args*/, boost::python::dict /*kwargs*/)
{
    throw NotSupportedException("This method or property is not supported under Notepad++ PythonScript");
}

void translateNotSupportedException(const NotSupportedException &e)
{
    PyErr_SetString(PyExc_RuntimeError, e.what());
}

}