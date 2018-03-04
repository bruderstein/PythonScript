#include "stdafx.h"

#include "ReplacementContainer.h"
#include "ScintillaWrapper.h"
#include "ReplaceEntry.h"
#include "Scintilla.h"

namespace NppPythonScript
{

void ReplacementContainer::runReplacements()
{
    std::list<ReplaceEntry*>::iterator endIterator = m_replacements->end();
    int offset = 0;
    for(std::list<ReplaceEntry*>::iterator it = m_replacements->begin(); it != endIterator; ++it)
	{
        int start = (*it)->getStart();
        int end = (*it)->getLength();

        m_scintillaWrapper->setTarget(offset + start, offset + end);
        intptr_t replacementLength = m_scintillaWrapper->callScintilla(SCI_REPLACETARGET, (*it)->getReplacementLength(), reinterpret_cast<LPARAM>((*it)->getReplacement()));
        offset = offset + (replacementLength - (end - start));
	}

}

}