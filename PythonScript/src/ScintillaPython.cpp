
#include "stdafx.h"
#include <boost/python.hpp>
#include "Python.h"
#include "ScintillaWrapper.h"

#include "ScintillaPython.h"



using namespace boost::python;


BOOST_PYTHON_MODULE(scintilla)
{
	class_<ScintillaWrapper>("Buffer")
		.def("addText", &ScintillaWrapper::addText, "Adds some text to current buffer");

}

void preinitScintillaModule()
{
	PyImport_AppendInittab("scintilla", &initscintilla);
}

void importScintilla(ScintillaWrapper* instance)
{
	// Get the __main__ module/namespace
	object main_module(handle<>(borrowed(PyImport_AddModule("__main__"))));
	object main_namespace = main_module.attr("__dict__");
	
	// Import our Scintilla object
	object scintilla_module( (handle<>(PyImport_ImportModule("scintilla"))) );
	
	// Add "Scintilla" to __main__
	main_namespace["scintilla"] = scintilla_module;

	// Create an instance variable buffer in __main__ that points to the ScintillaWrapper instance
	main_namespace["buffer"] = ptr(instance);

}
