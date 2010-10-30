
#include "stdafx.h"

#include "DynamicIDManager.h"
#include "IDAllocator.h"

using namespace std;


void DynamicIDManager::reserve(int quantity)
{
	if (quantity > m_capacity)
	{
		reserveAdditional(quantity - m_capacity);
	}
}

void DynamicIDManager::addBlock(int start, int quantity)
{
	m_idList.push_back(pair<int, int>(start, quantity));

	// Assume no overlaps (should really fix this, but we just need to use this carefully)
	m_capacity += quantity;
}

void DynamicIDManager::reserveAdditional(int quantity)
{
	
	int start;
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
			m_idList.push_back(pair<int, int>(start, quantity));	
		}

		m_capacity += quantity;
	}
}


int DynamicIDManager::begin()
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

int DynamicIDManager::currentID()
{
	return m_nextID;
}

DynamicIDManager& DynamicIDManager::operator++(int)
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
					throw exception("Out of IDs");
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


bool DynamicIDManager::allocateIDs(int quantity, int *start)
{
	return m_allocator->allocate(quantity, start);
}


bool DynamicIDManager::inRange(int id)
{
	bool retVal = false;
	list< pair<int, int> >::iterator it = m_idList.begin();
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
