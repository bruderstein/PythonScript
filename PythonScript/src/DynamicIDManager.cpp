
#include "stdafx.h"

#include "DynamicIDManager.h"
#include "IDAllocator.h"

void DynamicIDManager::reserve( size_t quantity )
{
	if (quantity > m_capacity)
	{
		reserveAdditional(quantity - m_capacity);
	}
}

void DynamicIDManager::addBlock( idx_t start, size_t quantity )
{
	m_idList.push_back(t_id(start, quantity));

	// Assume no overlaps (should really fix this, but we just need to use this carefully)
	m_capacity += quantity;
}

void DynamicIDManager::reserveAdditional( size_t quantity )
{
	idx_t start;
	if (allocateIDs(quantity, &start))
	{
		t_idList::reverse_iterator iter = m_idList.rbegin();
			
		// If this is a continuation of the last block,
		// just increase the quantity of the last block
			
		if (iter != m_idList.rend() 
			&& start == (iter->first + iter->second))
		{
			iter->second += quantity;
		}

		else // Otherwise just add a new block
		{
			m_idList.push_back(t_id(start, quantity));	
		}

		m_capacity += quantity;
	}
}


idx_t DynamicIDManager::begin()
{
	m_current = m_idList.begin();
	if (m_current == m_idList.end())
	{
		reserveAdditional(10);
		m_current = m_idList.begin();
		m_nextID = m_current->first;
	}
	else
	{
		m_nextID = m_current->first;
	}
	
	return m_nextID;
}

idx_t DynamicIDManager::currentID()
{
	return m_nextID;
}

DynamicIDManager& DynamicIDManager::operator++()
{
	// If nothing has ever been allocated
	if (m_current == m_idList.end())
	{
		reserveAdditional(10);
		m_current = m_idList.begin();
		m_nextID = m_current->first;
	}
	else
	{
		++m_nextID;
		if (m_nextID >= (m_current->first + m_current->second))
		{
			++m_current;
			if (m_current == m_idList.end())
			{
				reserveAdditional(10);
				m_current = m_idList.end();
				--m_current;
				if (m_nextID >= (m_current->first + m_current->second))
				{
					throw std::exception("Out of IDs");
				}
				else
				{
					// If the current ID is now within the limits of the last block
					// then we can just increment m_nextID
					if (m_nextID > m_current->first && m_nextID < m_current->first + m_current->second)
					{
						++m_nextID;
					}
					else
					{
						// Otherwise, we can just set the next ID to the first in the last block
						m_nextID = m_current->first;
					}
				}
			}
			else
			{
				m_nextID = m_current->first;
			}
		}
	}

	return *this;
}


bool DynamicIDManager::allocateIDs(size_t quantity, idx_t *start)
{
	return m_allocator->allocate(quantity, start);
}


bool DynamicIDManager::inRange(idx_t id)
{
	bool retVal = false;
	t_idList::iterator it = m_idList.begin();
	for(;it != m_idList.end(); ++it)
	{
		if (it->first > id)
			break;

		if (id >= it->first && (id < (it->first + it->second)))
		{
			retVal = true;
			break;
		}
	}

	return retVal;
}
