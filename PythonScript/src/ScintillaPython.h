#ifndef _SCINTILLAPYTHON_H
#define _SCINTILLAPYTHON_H

namespace NppPythonScript
{
class ScintillaWrapper;
}


void preinitScintillaModule();
void importScintilla(NppPythonScript::ScintillaWrapper* editor, NppPythonScript::ScintillaWrapper* editor1, NppPythonScript::ScintillaWrapper* editor2);

#endif
