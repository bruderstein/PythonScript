#include "stdafx.h"

#include "NotAllowedInCallbackException.h"


namespace NppPythonScript
{

void translateNotAllowedInCallbackException(const NotAllowedInCallbackException &e)
{
    PyErr_SetString(PyExc_RuntimeError, e.what());
}

}