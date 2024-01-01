#pragma once
/**
* @author    Francois-R.Boyer@PolyMtl.ca
* @date      March, 2013
* @copyright GPL; see license.txt in Notepad++ source for complete license text.
* @file
*/

#include <string>

/// Simple string class with length and pointer to const array of some type, without allocation.
template <class CharT>
class ConstString {
public:
	typedef size_t size_type;
	ConstString() : _length(0), _str(0) { }
	ConstString(const CharT* str, size_type length) : _length(length), _str(str) { }
	ConstString(const ConstString<CharT>& source) {
		*this = source;
	}
	ConstString(const std::basic_string<CharT>& source) {
		*this = source;
	}
	
	template <class string_type> // string_type can by a ConstString or std::basic_string; anything that has length() and c_str() of same type.
	ConstString<CharT>& operator=(const string_type& source) {
		_length = source.length();
		_str = source.c_str();
		return *this;
	}
	
	size_type length() const {
		return _length;
	}
	const CharT* c_str() const {
		return _str;
	}
	const CharT& operator[](size_type i) const {
		return _str[i];
	}
	std::basic_string<CharT> toString() const {
		return std::basic_string<CharT>(_str, _length);
	}
	
private:
	size_type _length;
	const CharT* _str;
};

/// Generic comparison of strings of different character sizes.
/// @note Characters are not encoded/decoded, the comparison is value by value.  Thus it can compare latin-1, UCS-2, and UTF-32 strings, but not UTF-8, UTF-16 and UTF-32.
template <class CharT1, class CharT2>
bool operator==(const ConstString<CharT1>& a, const ConstString<CharT2>& b)
{
	size_t a_length = a.length();
	if (a_length != b.length())
		return false;
	for (size_t i = 0; i < a_length; i++)
		if (a[i] != b[i])
			return false;
	return true;
}
template <class CharT1, class CharT2>
bool operator!=(const ConstString<CharT1>& a, const ConstString<CharT2>& b)
{
	return !(a == b);
}

/// Specialized comparison when both strings have same character type.
template <class CharT>
bool operator==(const ConstString<CharT>& a, const ConstString<CharT>& b)
{
	return a.length() == b.length() && memcmp(a.c_str(), b.c_str(), a.length() * sizeof(CharT)) == 0;
}

// Comparison with other string types:
template <class CharT1, class CharT2>
bool operator==(const ConstString<CharT1>& a, const std::basic_string<CharT2>& b)
{
	return a == ConstString<CharT2>(b.c_str(), b.length());
}
template <class CharT1, class CharT2>
bool operator==(const ConstString<CharT1>& a, const CharT2* b)
{
	return a == ConstString<CharT2>(b, std::char_traits<CharT2>::length(b));
}

template <class CharT1, class CharT2>
bool operator!=(const ConstString<CharT1>& a, const std::basic_string<CharT2>& b)
{
	return !(a == b);
}
template <class CharT1, class CharT2>
bool operator!=(const ConstString<CharT1>& a, const CharT2* b)
{
	return !(a == b);
}
