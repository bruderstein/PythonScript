
#include "stdafx.h"
#include "MatchPython.h"
#include "Match.h"



void export_match()
{
	//lint -e1793 While calling ’Symbol’: Initializing the implicit object parameter ’Type’ (a non-const reference) with a non-lvalue
	// The class and enum declarations are used as designed, but they mess up Lint.
	boost::python::class_<NppPythonScript::Match>("NppReMatch", boost::python::no_init)
		.def("group", &NppPythonScript::Match::py_group_list, "Gets the text of the groups in the given list as a tuple")
		.def("group", &NppPythonScript::Match::py_group_number, "Gets the text of the group number")
		.def("group", &NppPythonScript::Match::py_group_name, "Gets the text of the group number")
		.def("expand", &NppPythonScript::Match::py_expand, "Expands the replacement string")
		.def("groups", &NppPythonScript::Match::py_groups, "Returns a tuple containing all the subgroups of the match")
        // TODO: groupdict, start, end, span, pos, endpos, lastindex, lastgroup, re (throw exception?), string (throw exception?)
        // See: http://docs.python.org/2/library/re.html#match-objects
		;
	//lint +e1793
}
