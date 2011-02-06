#ifndef _NPPALLOCATOR_H
#define _NPPALLOCATOR_H

#ifndef _IDALLOCATOR_H
#include "IDAllocator.h"
#endif

class NppAllocator : public IDAllocator
{
public:
	explicit NppAllocator(HWND hNotepad)
		: m_hNotepad(hNotepad)  {};

	bool allocate(size_t quantity, idx_t *start);
	
private:
	NppAllocator(); // default constructor disabled

	HWND m_hNotepad;
};

#endif
