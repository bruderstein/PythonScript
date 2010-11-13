#ifndef _NPPALLOCATOR_H
#define _NPPALLOCATOR_H

#ifndef _IDALLOCATOR_H
#include "IDAllocator.h"
#endif

class NppAllocator : public IDAllocator
{
public:
	NppAllocator(HWND hNotepad)
		: m_hNotepad(hNotepad)  {};

	bool allocate(int quantity, int *start);
	
private:
	HWND m_hNotepad;
};

#endif
