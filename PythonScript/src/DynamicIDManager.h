#ifndef _DYNAMICIDMANAGER_H 
#define _DYNAMICIDMANAGER_H

class IDAllocator;

class DynamicIDManager
{
public:

	DynamicIDManager(IDAllocator *allocator)
		: m_allocator (allocator),
		  m_nextID(0),
		  m_capacity(0)	{
		m_current = m_idList.begin();
	}

	DynamicIDManager(IDAllocator *allocator, int initialStart, int quantity)
		: m_allocator (allocator),
		 m_nextID(initialStart + quantity),
		 m_capacity(quantity)
	{
		m_idList.push_back(std::pair<int, int>(initialStart, quantity));
		m_current = m_idList.begin();
	}

	void reserve(int quantity);

	void reserveAdditional(int quantity);

	int begin();

	int currentID();

	void addBlock(int start, int quantity);

	// Post-increment operator
	DynamicIDManager& operator++(int);


	int capacity()   { return m_capacity; }

	bool inRange(int id);

private:
	DynamicIDManager(); // default constructor disabled

	// Methods
	bool allocateIDs(int quantity, int *start);


	IDAllocator* m_allocator;

	// Data
	typedef std::list< std::pair<int, int> >  t_idList;
	t_idList           m_idList;
	t_idList::iterator m_current;

	int m_nextID;

	int m_capacity;
};

#endif
