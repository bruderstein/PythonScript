#ifndef _STATICIDALLOCATOR_H
#define _STATICIDALLOCATOR_H

#ifndef _IDALLOCATOR_H
#include "IDAllocator.h"
#endif

class StaticIDAllocator : public IDAllocator
{
public:
	StaticIDAllocator(int start, int maximum)
		: m_start(start),
		  m_maximum(maximum),
		  m_current(start)   {};

	bool allocate(int quantity, int *start);
	
private:
	int m_start;
	int m_maximum;
	int m_current;
};

#endif
