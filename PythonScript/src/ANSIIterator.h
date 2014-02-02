#ifndef ANSIITERATOR_H_1245813891234891238
#define ANSIITERATOR_H_1245813891234891238





class ANSIIterator : public std::iterator<std::bidirectional_iterator_tag, char>
{
public:
	ANSIIterator(const char* doc = NULL, int pos = 0, int end = 0) : 
		_doc(doc),
		_pos(pos),
		_end(end)
	{
		// Check for debug builds
		assert(_pos <= _end);
		// Ensure for release.
		if (_pos > _end)
			_pos = _end;
		
	}

	ANSIIterator(const ANSIIterator& copy) :
		_doc(copy._doc),
		_pos(copy._pos),
		_end(copy._end)
	{
	}

	bool operator == (const ANSIIterator& other) const
	{
		return (ended() == other.ended()) && (_doc == other._doc) && (_pos == other._pos);
	}

	bool operator != (const ANSIIterator& other) const
	{
		return !(*this == other);
	}

	char operator * () const
	{
		return _doc[_pos];
	}

	ANSIIterator& operator = (int pos)
	{
		_pos = pos;
		return *this;
	}

	ANSIIterator& operator ++ ()
	{
		assert(_pos < _end);
		++_pos;
		if (ended())
			_pos = _end;
		return *this;
	}

	ANSIIterator& operator -- ()
	{
		assert(_pos > 0);
		--_pos;
        if (_pos < 0) {
            _pos = 0;
		}
		return *this;
	}

	int pos() const
	{
		return _pos;
	}

private:

	bool ended() const
	{
		return bytesLeft() <= 0;
	}
	
	int bytesLeft() const
	{
		return _end - _pos;
	}

	const char* _doc;
	int _pos;
	int _end;
};

#endif // ANSIITERATOR_H_1245813891234891238
