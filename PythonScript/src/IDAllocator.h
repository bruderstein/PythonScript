
#ifndef _IDALLOCATOR_H
#define _IDALLOCATOR_H

class IDAllocator
{
public:
	virtual bool allocate(int quantity, int *start) = 0;
};

#endif

