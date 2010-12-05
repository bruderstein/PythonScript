#ifndef _DYNAMICIDMANAGER_H 
#define _DYNAMICIDMANAGER_H

class IDAllocator;

class DynamicIDManager
{
public:

	explicit DynamicIDManager(IDAllocator *allocator)
		: m_allocator (allocator),
		  m_nextID(0),
		  m_capacity(0)	{
		m_current = m_idList.begin();
	}

	DynamicIDManager(IDAllocator* allocator, idx_t initialStart, size_t quantity)
		: m_allocator (allocator),
		 m_nextID(initialStart + quantity),
		 m_capacity(quantity)
	{
		m_idList.push_back(t_id(initialStart, quantity));
		m_current = m_idList.begin();
	}

	void reserve(size_t quantity);

	void reserveAdditional(size_t quantity);

	idx_t begin();

	idx_t currentID();

	void addBlock(idx_t start, size_t quantity);

	// Post-increment operator
	DynamicIDManager& operator++(int);


	int capacity()   { return m_capacity; }

	bool inRange(idx_t id);

private:
	DynamicIDManager(); // default constructor disabled

	// Methods
	bool allocateIDs(size_t quantity, idx_t *start);


	IDAllocator* m_allocator;

	// Data
	typedef std::pair<idx_t, size_t> t_id;
	typedef std::list<t_id>  t_idList;
	t_idList           m_idList;
	t_idList::iterator m_current;

	idx_t m_nextID;

	size_t m_capacity;
};

#endif
