#ifndef _DYNAMICIDMANAGER_H 
#define _DYNAMICIDMANAGER_H

#include "IDAllocator.h"

class DynamicIDManager
{
public:

	DynamicIDManager(IDAllocator *allocator)
		: m_allocator (allocator)
	{
		m_current = m_idList.begin();
	};

	DynamicIDManager(IDAllocator *allocator, int initialStart, int quantity)
		: m_allocator (allocator)
	{
		m_idList.push_back(std::pair<int, int>(initialStart, quantity));
		m_current = m_idList.begin();
		m_capacity = quantity;
		m_nextID = initialStart + quantity;
		
	};


	void reserve(int quantity);

	void reserveAdditional(int quantity);

	int begin();

	int currentID();

	// Post-increment operator
	DynamicIDManager& operator++(int);


	int capacity()   { return m_capacity; }; 

private:
	// Methods
	bool allocateIDs(int quantity, int *start);


	IDAllocator* m_allocator;

	// Data
	typedef std::list< std::pair<int, int> >  t_idList;
	t_idList           m_idList;
	t_idList::iterator m_current;

	int m_nextID;

	int m_capacity;

	HWND m_nppHandle;
};


#endif
