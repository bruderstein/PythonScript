
#include "stdafx.h"
#include "MatchPython.h"
#include "Match.h"



void export_match()
{
	//lint -e1793 While calling ’Symbol’: Initializing the implicit object parameter ’Type’ (a non-const reference) with a non-lvalue
	// The class and enum declarations are used as designed, but they mess up Lint.
	boost::python::class_<NppPythonScript::Match, boost::noncopyable>("NppReMatch", boost::python::no_init)
		.def("group", &NppPythonScript::Match::py_group_number, "Gets the text of the group number")
		.def("group", &NppPythonScript::Match::py_group_name, "Gets the text of the group number")
		.def("group", &NppPythonScript::Match::py_group_tuple2, "Gets the text of the groups in the given list as a tuple")
		.def("group", &NppPythonScript::Match::py_group_tuple3, "Gets the text of the groups in the given list as a tuple")
		.def("group", &NppPythonScript::Match::py_group_tuple4, "Gets the text of the groups in the given list as a tuple")
		.def("expand", &NppPythonScript::Match::py_expand, "Expands the replacement string")
		.def("groups", &NppPythonScript::Match::py_groups, "Returns a tuple containing all the subgroups of the match")
		.def("start", &NppPythonScript::Match::py_start, boost::python::args("group"), "Returns the start position of the given group.")
		.def("start", &NppPythonScript::Match::py_start_name, boost::python::args("group"), "Returns the start position of the given group.")
		.def("start", &NppPythonScript::Match::py_start_group_0, "Returns the start position of group 0 (the whole match).")
		.def("end", &NppPythonScript::Match::py_end, boost::python::args("group"), "Returns the start position of the given group.")
		.def("end", &NppPythonScript::Match::py_end_name, boost::python::args("group"), "Returns the start position of the given group.")
		.def("end", &NppPythonScript::Match::py_end_group_0, "Returns the start position of group 0 (the whole match).")
		.def("span", &NppPythonScript::Match::py_span_group_0, "Returns a tuple (m.start(group), m.end(group)). Group defaults to 0 (the whole match)")
		.def("span", &NppPythonScript::Match::py_span, boost::python::args("group"), "Returns a tuple (m.start(group), m.end(group)). Group defaults to 0 (the whole match)")
		.def("span", &NppPythonScript::Match::py_span_name, boost::python::args("group"), "Returns a tuple (m.start(group), m.end(group)). Group defaults to 0 (the whole match)")
        // TODO: groupdict, span, pos, endpos, lastindex, lastgroup, re (throw exception?), string (throw exception?)
        // See: http://docs.python.org/2/library/re.html#match-objects
		;
	//lint +e1793
}
