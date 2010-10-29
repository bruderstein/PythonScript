#include "stdafx.h"

#include "StaticIDAllocator.h"

bool StaticIDAllocator::allocate(int quantity, int *start)
{
	bool retVal = false;

	if (m_current + quantity <= m_maximum)
	{
		*start = m_current;
		m_current += quantity;
		retVal = true;
	}

	return retVal;
}
