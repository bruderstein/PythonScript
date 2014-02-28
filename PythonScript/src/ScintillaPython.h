#ifndef _SCINTILLAPYTHON_H
#define _SCINTILLAPYTHON_H

namespace NppPythonScript
{
class ScintillaWrapper;


void preinitScintillaModule();
void importScintilla(boost::shared_ptr<ScintillaWrapper> editor, boost::shared_ptr<ScintillaWrapper> editor1, boost::shared_ptr<ScintillaWrapper> editor2);


}
#endif
