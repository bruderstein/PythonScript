#include "stdafx.h"
#include "NppAllocator.h"
#include "Notepad_plus_msgs.h"

bool NppAllocator::allocate(int quantity, int *start)
{
	return TRUE == ::SendMessage(m_hNotepad, NPPM_ALLOCATECMDID, quantity, reinterpret_cast<LPARAM>(start));
}
