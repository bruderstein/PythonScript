#include "stdafx.h"

#include "ArgumentException.h"


namespace NppPythonScript
{


void translateArgumentException(const ArgumentException &e)
{
    PyErr_SetString(PyExc_TypeError, e.what());
}

}