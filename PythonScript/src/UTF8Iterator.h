#ifndef UTF8ITERATOR_H_3452843291318441149
#define UTF8ITERATOR_H_3452843291318441149


#include "UtfConversion.h"


#ifndef ARRAY_LENGTH
#define ARRAY_LENGTH(X) (sizeof(X)/sizeof(0[X]))
#endif

class u32_regex_traits
{
	// see http://www.boost.org/doc/libs/1_52_0/libs/regex/doc/html/boost_regex/ref/concepts/traits_concept.html
	
	// Simple character matching: a character from regex and from text is passed in translate(c, ignoreCase), where ignoreCase is from flag regex_constants::icase, and the results are compared.
	// When comparing with a range (e.g. [a-z]) and flag regex_constants::collate is set, result of translate is passed to transform(p1, p2), with a single character.
	// When comparing character equivalence (e.g. [[=a=]]), the text between equals is passed through lookup_collatename, which must return a string of either 1 or 2 characters, that string is passed to transform_primary(p1, p2).
	// The result is the "character" which is compared.
	// For optimization purpose, boost computes and caches translate and transform results for all Latin-1 characters, but still call them again when a potential match is found (why?).
	
public:
	//NOTE: There is a "bug" in Boost basic_regex_parser.hpp (verified in version 1.52.0), where there is a comparison (i > (int)(std::numeric_limits<charT>::max)()).  If charT has a maximum higher than max int, it will overflow. For this reason, we must use signed ints for UTF-32 characters.
	typedef U32                           charT;
	typedef charT                         char_type;
	typedef std::size_t                   size_type;
	typedef std::basic_string<charT>      string_type;
	typedef boost::BOOST_REGEX_DETAIL_NS::lcid_type   locale_type;
	typedef boost::uint_least32_t         char_class_type;
	
	struct boost_extensions_tag{};
	
	u32_regex_traits() { }
	
	static size_type length(const charT* p) {
		return std::char_traits<charT>::length(p);
	}
	static charT translate(charT c) {
		return c;
	}
	static charT translate_nocase(charT c) {
		return tolower(c);
	}
	
	/// Used for character ranges, e.g. [a-z], when flag regex_constants::collate is set.
	/// Accented characters are considered different from each other but are usually between non-accented characters in search order, e.g. transform('a') < transform('à') < transform('b'); that order is locale dependent.
	/// Called on a single character or a digraph, each character being the result of translate.
	static string_type transform(const charT* p1, const charT* p2) { 
		return genSortkey(p1, p2);
	}
	/// Used for equivalence classes, e.g. [[=a=]].
	/// Should be independant of accents and case, e.g. transform_primary('a') = transform_primary('à') = transform_primary('A').
	/// Called on a single character or a digraph, resulting from lookup_collatename.
	static string_type transform_primary(const charT* p1, const charT* p2) {
		return genSortkey(p1, p2, true);
	}
	
	/// Used for character classes, e.g. \w, \d, [[:alpha:]], \pL, \p{L}.
	/// Resulting class must support OR operations as union of classes (used in basic_char_set::add_class).
	static char_class_type lookup_classname(const charT* p1, const charT* p2);
	
	/// Used for collating elements and equivalence classes, e.g. \N{tab}, [[.tab.]], [[.ae.]], [[=ae=]], [[=a=]].
	/// Note that [[=ae=]] should match "ae" and "æ"; for this, lookup_collatename must return a digraph.
	/// Must return a single character for \N{ }, and either a single character or digraph for [[. .]] and [[= =]].
	static string_type lookup_collatename(const charT* p1, const charT* p2);
	
	/// Used to test if a character is in a character class.
	/// class_mask comes from results of lookup_classname, possibly ORed together.
	static bool isctype(charT c, char_class_type class_mask) {
		if (!UtfConversion::isValidCodepoint(c))
			return (class_mask & mask_invalid) != 0;
		
		U16_char u16c = UtfConversion::toUtf16(c);

		WORD char_class[2];
		if (::GetStringTypeExW(LOCALE_USER_DEFAULT, CT_CTYPE1, u16c, u16c.length(), &char_class[0])
			&& (char_class[0] & class_mask & mask_CT_CTYPE1))
			return true;
		return
			(class_mask & mask_word) && (c == '_')
			|| (class_mask & mask_unicode) && (c > 0xFF)
			|| (class_mask & mask_horizontal) &&  (char_class[0] & C1_SPACE) && !(is_separator(c) || c == '\v')
			|| (class_mask & mask_vertical) && (is_separator(c) || c == '\v')
			;
	}
	static int value(charT c, int radix) {
		int result = boost::BOOST_REGEX_DETAIL_NS::global_value(c);
		return result < radix ? result : -1;
	}
	static locale_type imbue(locale_type l) {
		return l; //NOTE: should we support changing locale?
	}
	static locale_type getloc() {
		return locale_type();
	}
	
	// Methods required with boost_extensions_tag : ----------
	
	static boost::regex_constants::syntax_type syntax_type(charT c) {
		return (c < 0x7F && c > 0)
			? boost::BOOST_REGEX_DETAIL_NS::get_default_syntax_type((char)c)
			: boost::regex_constants::syntax_char;
	}
	static boost::regex_constants::escape_syntax_type escape_syntax_type(charT c) {
		if (c < 0x7F && c > 0)
			return boost::BOOST_REGEX_DETAIL_NS::get_default_escape_syntax_type((char)c);
		if (isctype(c, C1_LOWER))
			return boost::regex_constants::escape_type_class;
		if (isctype(c, C1_UPPER))
			return boost::regex_constants::escape_type_not_class;
		return 0;
	}
	static charT translate(charT c, bool ignoreCase) {
		return ignoreCase ? translate_nocase(c) : translate(c);
	}
	boost::intmax_t toi(const charT*& p1, const charT* p2, int radix) const {
		return boost::BOOST_REGEX_DETAIL_NS::global_toi(p1, p2, radix, *this);
	}
	static std::string error_string(boost::regex_constants::error_type n) {
		return boost::BOOST_REGEX_DETAIL_NS::get_default_error_string(n);
	}
	static charT tolower(charT c) {
		if (UtfConversion::isValidCodepoint(c))
			c = mapChar(c, LCMAP_LOWERCASE);
		return c;
	}
	static charT toupper(charT c) {
		if (UtfConversion::isValidCodepoint(c))
			c = mapChar(c, LCMAP_UPPERCASE);
		return c;
	}
private:
	// Same mask values as in Boost regex w32_regex_traits.hpp:
	static const char_class_type mask_CT_CTYPE1 = 0x3FF;
	static const char_class_type mask_word = 0x0400;
	static const char_class_type mask_unicode = 0x0800;
	static const char_class_type mask_horizontal = 0x1000;
	static const char_class_type mask_vertical = 0x2000;
	static const char_class_type mask_invalid = 0x4000;
	
	// Our version of is_separator (similar to the one in regex_traits_defaults.hpp, but not truncating c to 16bits)
	static bool is_separator(charT c) {
		return 
			(c == '\n') 
			|| (c == '\r') 
			|| (c == '\f') 
			|| (c == 0x2028) 
			|| (c == 0x2029) 
			|| (c == 0x85);
	}
	static charT mapChar(charT c, DWORD mapFlags);
	
	static string_type genSortkey(const charT* p1, const charT* p2, bool onlyPrimaryLevel = false);
	static std::basic_string<UCHAR> genSortkey(const std::wstring& wstr, bool onlyPrimaryLevel);
	static string_type convertSortkeyToInts(const std::basic_string<UCHAR>& sortkey);
};

class UTF8Iterator
{
public:

	//std::iterator traits
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = U32;
	using difference_type = std::ptrdiff_t;
	using pointer = U32*;
	using reference = U32&;

	UTF8Iterator(const char* doc = NULL, int pos = 0, int end = 0) : 
		_doc(doc),
		_pos(pos),
		_end(end),
		_character(0),
		_utf8Length(0)
	{
		// Check for debug builds
		assert(_pos <= _end);
		// Ensure for release.
		if (_pos > _end)
			_pos = _end;
		
		if (!ended())
			readCharacter();
	}

	UTF8Iterator(const UTF8Iterator& copy) :
		_doc(copy._doc),
		_pos(copy._pos),
		_end(copy._end),
		_character(copy._character),
		_utf8Length(copy._utf8Length)
	{
	}

	bool operator == (const UTF8Iterator& other) const
	{
		return (ended() == other.ended()) && (_doc == other._doc) && (_pos == other._pos);
	}

	bool operator != (const UTF8Iterator& other) const
	{
		return !(*this == other);
	}

	U32 operator * () const
	{
		return _character;
	}

	UTF8Iterator& operator = (int pos)
	{
		_pos = pos;
		return *this;
	}

	UTF8Iterator& operator ++ ()
	{
		assert(_pos < _end);
		_pos += _utf8Length;
		if (!ended())
			readCharacter();
		else
			_pos = _end;
		return *this;
	}

	UTF8Iterator& operator -- ()
	{
		assert(_pos > 0);
		const int original_pos = _pos;
		--_pos;
		UCHAR current_byte = _doc[_pos];
		_utf8Length = 1;
		_character = UtfConversion::INVALID;
		if (Utf8::isSingleByte(current_byte))
		{
			_character = current_byte;
			return *this;
		}
		const int mininum_character_start_position = std::max(original_pos-4, 0);
		while (_pos > mininum_character_start_position
			&& Utf8::isContinuation(_doc[_pos]))
			--_pos;
		readCharacter();
		if (_pos + _utf8Length != original_pos)
		{
			_pos = original_pos - 1;
			_character = UtfConversion::INVALID;
			_utf8Length = 1;
			return *this;
		}
		return *this;
	}

	int pos() const
	{
		return _pos;
	}

private:
	void readCharacter();

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
	U32 _character;
	int _utf8Length;
};

#endif // UTF8ITERATOR_H_3452843291318441149
