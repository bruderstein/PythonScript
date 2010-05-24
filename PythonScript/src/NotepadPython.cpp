
#include "stdafx.h"
#include "NotepadPython.h"

#include "NotepadPlusWrapper.h"

using namespace boost::python;

void export_notepad()
{
	class_<NotepadPlusWrapper>("Editor", no_init)
		.def("save", &NotepadPlusWrapper::save, "Save the current file")
		.def("saveAs", &NotepadPlusWrapper::saveAs, "Save the current file as the specified filename")
		.def("saveAsCopy", &NotepadPlusWrapper::saveAsCopy, "Save the current file as the specified filename, but don't change the filename")
		.def("getCurrentView", &NotepadPlusWrapper::getCurrentView, "Get the currently active view (0 or 1)")
		.def("getCurrentLang", &NotepadPlusWrapper::getCurrentLangType, "Get the current language type (returns the LANGTYPE.xxx constants)")
		.def("setCurrentLang", &NotepadPlusWrapper::SetCurrentLangType, "Set the current language type (use LANGTYPE.xxx constants)");

	enum_<LangType>("LANGTYPE")
			.value("TXT", L_TXT)
			.value("PHP", L_PHP)
			.value("C", L_C)
			.value("CPP", L_CPP)
			.value("CS", L_CS)
			.value("OBJC", L_OBJC)
			.value("JAVA", L_JAVA)
			.value("RC", L_RC)
			.value("HTML", L_HTML)
			.value("XML", L_XML)
			.value("MAKEFILE", L_MAKEFILE)
			.value("PASCAL", L_PASCAL)
			.value("BATCH", L_BATCH)
			.value("INI", L_INI)
			.value("NFO", L_NFO)
			.value("USER", L_USER)
			.value("ASP", L_ASP)
			.value("SQL", L_SQL)
			.value("VB", L_VB)
			.value("JS", L_JS)
			.value("CSS", L_CSS)
			.value("PERL", L_PERL)
			.value("PYTHON", L_PYTHON)
			.value("LUA", L_LUA)
			.value("TEX", L_TEX)
			.value("FORTRAN", L_FORTRAN)
			.value("BASH", L_BASH)
			.value("FLASH", L_FLASH)
			.value("NSIS", L_NSIS)
			.value("TCL", L_TCL)
			.value("LISP", L_LISP)
			.value("SCHEME", L_SCHEME)
			.value("ASM", L_ASM)
			.value("DIFF", L_DIFF)
			.value("PROPS", L_PROPS)
			.value("PS", L_PS)
			.value("RUBY", L_RUBY)
			.value("SMALLTALK", L_SMALLTALK)
			.value("VHDL", L_VHDL)
			.value("KIX", L_KIX)
			.value("AU3", L_AU3)
			.value("CAML", L_CAML)
			.value("ADA", L_ADA)
			.value("VERILOG", L_VERILOG)
			.value("MATLAB", L_MATLAB)
			.value("HASKELL", L_HASKELL)
			.value("INNO", L_INNO)
			.value("SEARCHRESULT", L_SEARCHRESULT)
			.value("CMAKE", L_CMAKE)
			.value("YAML", L_YAML);

}

