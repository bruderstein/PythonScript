#include "stdafx.h"
#include "Match.h"
#include "GroupNotFoundException.h"

namespace NppPythonScript
{


boost::python::str Match::py_group_number(int groupNumber)
{
	GroupDetail *groupDetail = group(groupNumber);
	if (NULL == groupDetail)
	{
		throw GroupNotFoundException("no such group");
	}

	return boost::python::str(getTextForGroup(group(groupNumber)));
}


boost::python::str Match::py_group_name(boost::python::str pyGroupName)
{
	std::string stringGroupName(boost::python::extract<const char *>(pyGroupName.attr("__str__")()));

	GroupDetail *groupDetail = groupName(stringGroupName.c_str());
	if (NULL == groupDetail)
	{
		throw GroupNotFoundException("no such group");
	}

	return boost::python::str(getTextForGroup(groupDetail));
}

boost::python::str Match::getGroup(boost::python::object groupIdentifier)
{
	if (PyLong_Check(groupIdentifier.ptr()))
	{
		return py_group_number(boost::python::extract<int>(groupIdentifier));
	}
	else if (PyUnicode_Check(groupIdentifier.ptr()))
	{
		return py_group_name(boost::python::extract<boost::python::str>(groupIdentifier));
	}
	else
	{
		return py_group_name(boost::python::extract<boost::python::str>(groupIdentifier.attr("__str__")));
	}
}



boost::python::str Match::py_expand(boost::python::object replaceFormat)
{
	char *result;
	size_t resultLength;
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

boost::python::tuple Match::py_span(int groupIndex)
{
	return boost::python::make_tuple(py_start(groupIndex), py_end(groupIndex));
}

boost::python::tuple Match::py_span_name(boost::python::str groupName)
{
	return boost::python::make_tuple(py_start_name(groupName), py_end_name(groupName));
}

int Match::py_lastindex()
{
	int lastGroup = static_cast<int>(groupCount() - 1);
	while(lastGroup > 0 && !group(lastGroup)->matched())
		--lastGroup;

	return lastGroup;
}

boost::python::tuple Match::py_groups()
{
	size_t size = groupCount();
	PyObject* groupsTuple = PyTuple_New(size - 1);
	for(size_t index = 1; index < size; ++index)
	{
		boost::python::str groupContent = py_group_number(static_cast<int>(index));
		// PyTuple_SetItem steals a reference, but because it's a boost::python::object, it'll be Py_DECREF'd by the next iteration
		Py_INCREF(groupContent.ptr());
		PyTuple_SetItem(groupsTuple, index - 1, groupContent.ptr());
	}

	return boost::python::tuple(boost::python::handle<PyObject>(groupsTuple));
}

boost::python::object py_group_variable(boost::python::tuple args, boost::python::dict kwargs)
{
	Match *match = boost::python::extract<Match*>(args[0]);
	return match->py_group_variable(boost::python::tuple(args.slice(1, boost::python::len(args))), kwargs);
}

boost::python::object Match::py_group_variable(boost::python::tuple args, boost::python::dict kwargs)
{
	size_t size = boost::python::len(args);

	// For the default case, no arguments, return the whole match
	if (size == 0)
	{
		return py_group_number(0);
	}

	// If only one argument is specified, then we return the content of that group
	if (size == 1)
	{
		if (PyNumber_Check(boost::python::object(args[0]).ptr()))
		{
			return py_group_number(boost::python::extract<int>(args[0]));
		}
		else
		{
			return py_group_name(boost::python::str(args[0]));
		}
	}

	// If more than one argument is specified, then we return a tuple with group contents in order of the arguments specified
	PyObject* groupsTuple = PyTuple_New(size);
	for(size_t index = 0; index < size; ++index)
	{
		boost::python::str groupContent;
		if (PyNumber_Check(boost::python::object(args[index]).ptr()))
		{
			groupContent = py_group_number(boost::python::extract<int>(args[index]));
		}
		else
		{
			groupContent = py_group_name(boost::python::str(args[index]));
		}

		// PyTuple_SetItem steals a reference, but because it's a boost::python::object, it'll be Py_DECREF'd by the next iteration
		Py_INCREF(groupContent.ptr());
		PyTuple_SetItem(groupsTuple, index, groupContent.ptr());
	}
	return boost::python::tuple(boost::python::handle<PyObject>(groupsTuple));
}

}