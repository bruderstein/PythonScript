#include "stdafx.h"
#include "Match.h"

namespace NppPythonScript
{


boost::python::str Match::py_group_number(int groupNumber)
{
    return boost::python::str(getTextForGroup(group(groupNumber)));
}


boost::python::str Match::py_group_name(boost::python::str pyGroupName)
{
    std::string stringGroupName(boost::python::extract<const char *>(pyGroupName.attr("__str__")()));

    return boost::python::str(getTextForGroup(groupName(stringGroupName.c_str())));
}

boost::python::str Match::getGroup(boost::python::object groupIdentifier)
{
    if (PyInt_Check(groupIdentifier.ptr()))
	{
        return py_group_number(boost::python::extract<int>(groupIdentifier));
	}
	else if (PyString_Check(groupIdentifier.ptr()))
	{
        return py_group_name(boost::python::extract<boost::python::str>(groupIdentifier));
	} 
	else 
	{
        return py_group_name(boost::python::extract<boost::python::str>(groupIdentifier.attr("__str__")));
	}
}



boost::python::tuple Match::py_group_tuple2(boost::python::object group1, boost::python::object group2)
{
    return boost::python::make_tuple(getGroup(group1), getGroup(group2));
}

boost::python::tuple Match::py_group_tuple3(boost::python::object group1, boost::python::object group2, boost::python::object group3)
{
    return boost::python::make_tuple(getGroup(group1), getGroup(group2), getGroup(group3));
}

boost::python::tuple Match::py_group_tuple4(boost::python::object group1, boost::python::object group2, boost::python::object group3, boost::python::object group4)
{
    return boost::python::make_tuple(getGroup(group1), getGroup(group2), getGroup(group3), getGroup(group4));
}


boost::python::str Match::py_expand(boost::python::object replaceFormat)
{
	char *result;
    int resultLength;
	expand(boost::python::extract<const char *>(replaceFormat.attr("__str__")()), &result, &resultLength);
    boost::python::str pyResult(const_cast<const char *>(result));
    delete [] result;
    return pyResult;
}


int Match::py_start(int groupIndex)
{
    GroupDetail *groupDetail = group(groupIndex);
    int result = -1;
	if (groupDetail && groupDetail->matched()) 
	{
        result = groupDetail->start();
	}

    return result;   
}

int Match::py_start_name(boost::python::str groupName)
{
    GroupDetail *groupDetail = this->groupName(boost::python::extract<const char *>(groupName));
    int result = -1;
	if (groupDetail && groupDetail->matched()) 
	{
        result = groupDetail->start();
	}
    return result;
}

int Match::py_end(int groupIndex)
{
    GroupDetail *groupDetail = group(groupIndex);
    int result = -1;
	if (groupDetail && groupDetail->matched()) 
	{
        result = groupDetail->end();
	}

    return result;   
}

int Match::py_end_name(boost::python::str groupName)
{
    GroupDetail *groupDetail = this->groupName(boost::python::extract<const char *>(groupName));
    int result = -1;
	if (groupDetail && groupDetail->matched()) 
	{
        result = groupDetail->end();
	}
    return result;
}

        
boost::python::tuple Match::py_groups()
{
    int size = groupCount();
    PyObject* groupsTuple = PyTuple_New(size - 1);
    for(int index = 1; index != size; ++index)
	{
        boost::python::str groupContent = py_group_number(index);
        // PyTuple_SetItem steals a reference, but because it's a boost::python::object, it'll be Py_DECREF'd by the next iteration
        Py_INCREF(groupContent.ptr());
        PyTuple_SetItem(groupsTuple, index - 1, groupContent.ptr());
	}

    return boost::python::tuple(boost::python::handle<PyObject>(groupsTuple));
}


}