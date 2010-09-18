#ifndef _NPPALLOCATOR_H
#define _NPPALLOCATOR_H

#include "IDAllocator.h"

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