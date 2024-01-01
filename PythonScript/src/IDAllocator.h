
#ifndef _IDALLOCATOR_H
#define _IDALLOCATOR_H

class IDAllocator
{
public:
	virtual ~IDAllocator() = default;
	virtual bool allocate(size_t quantity, idx_t *start) = 0;
};

#endif

