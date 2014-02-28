#ifndef _NOTEPADPYTHON_H
#define _NOTEPADPYTHON_H

namespace NppPythonScript
{
class NotepadPlusWrapper;

void export_notepad();

void importNotepad(boost::shared_ptr<NotepadPlusWrapper> instance);

}

#endif
