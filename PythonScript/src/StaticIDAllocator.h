#ifndef _STATICIDALLOCATOR_H
#define _STATICIDALLOCATOR_H

#ifndef _IDALLOCATOR_H
#include "IDAllocator.h"
#endif

class StaticIDAllocator : public IDAllocator
{
public:
	StaticIDAllocator(idx_t start, idx_t maximum)
		: m_start(start),
		  m_maximum(maximum),
		  m_current(start)   {};

	bool allocate(size_t quantity, idx_t *start);
	
private:
	StaticIDAllocator(); // default constructor disabled

	idx_t m_start;
	idx_t m_maximum;
	idx_t m_current;
};

#endif
