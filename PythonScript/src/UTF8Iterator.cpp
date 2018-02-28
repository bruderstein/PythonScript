#include "stdafx.h"

#include "UTF8Iterator.h"

void UTF8Iterator::readCharacter()
{
	UtfConversion::Utf8Decoder decoder(_doc + _pos, bytesLeft());
	_character = decoder.decodedChar();
	_utf8Length = decoder.decodedLength();
}

typedef u32_regex_traits::charT charT;
typedef u32_regex_traits::string_type string_type;
typedef u32_regex_traits::char_class_type char_class_type;

char_class_type u32_regex_traits::lookup_classname(const charT* p1, const charT* p2) {
	static const char_class_type masks[] = {
		C1_ALPHA|C1_DIGIT, // alnum
		C1_ALPHA, // alpha
		C1_BLANK, // blank
		C1_CNTRL, // cntrl
		C1_DIGIT, // d
		C1_DIGIT, // digit
		~(C1_CNTRL|C1_SPACE|C1_BLANK) & 0x1ff, // graph
		mask_horizontal, // h
		C1_LOWER, // l
		C1_LOWER, // lower
		~C1_CNTRL & 0x1ff, // print
		C1_PUNCT, // punct
		C1_SPACE, // s
		C1_SPACE, // space
		C1_UPPER, // u
		mask_unicode, // unicode
		C1_UPPER, // upper
		mask_vertical, // v
		C1_ALPHA|C1_DIGIT | mask_word, // w
		C1_ALPHA|C1_DIGIT | mask_word, // word
		C1_XDIGIT // xdigit
	};
	//NOTE: PEARL is case sensitive in class names, so we do case sensitive matches, but standard regex traits from Boost do case insensitive matches.
	std::size_t class_id = boost::BOOST_REGEX_DETAIL_NS::get_default_class_id(p1, p2);
	if (class_id < ARRAY_LENGTH(masks))
		return masks[class_id];
	if (p2 - p1 == 1) { // Support for negative "single character" character classes.
		charT c[1] = { tolower(*p1) };
		class_id = boost::BOOST_REGEX_DETAIL_NS::get_default_class_id(c, c+1);
		if (class_id < ARRAY_LENGTH(masks))
			return masks[class_id]; // The inversion is done by basic_regex_parser, if the character has class regex_constants::escape_type_not_class.
	}
	ConstString<charT> classname(p1, std::min((int)(p2-p1), 5));
	if (classname == "inval")
		return mask_invalid;
	return 0;
}

string_type u32_regex_traits::lookup_collatename(const charT* p1, const charT* p2) {
	u8string name = UtfConversion::toUtf8(ConstString<charT>(p1, p2-p1));
	u8string result = boost::BOOST_REGEX_DETAIL_NS::lookup_default_collate_name(name);
	return UtfConversion::toUtf32(result);
}

charT u32_regex_traits::mapChar(charT c, DWORD mapFlags) {
	U16_char u16 = UtfConversion::toUtf16(c);
	U16_char u16mapped;
	u16mapped.length(::LCMapStringW(LOCALE_USER_DEFAULT, mapFlags, u16, u16.length(), u16mapped, 2));
	return UtfConversion::toUtf32(u16mapped);
}

string_type u32_regex_traits::genSortkey(const charT* p1, const charT* p2, bool onlyPrimaryLevel) {
	std::wstring str = UtfConversion::toUtf16(ConstString<charT>(p1, p2-p1));
	std::basic_string<UCHAR> sortkey = genSortkey(str, onlyPrimaryLevel);
	return convertSortkeyToInts(sortkey);
}

std::basic_string<UCHAR> u32_regex_traits::genSortkey(const std::wstring& wstr, bool onlyPrimaryLevel) {
	int sortkey_byte_length = ::LCMapStringW(LOCALE_USER_DEFAULT, LCMAP_SORTKEY, wstr.c_str(), wstr.length(), 0,0);
	if (sortkey_byte_length <= 0)
		return std::basic_string<UCHAR>();
	std::basic_string<UCHAR> sortkey(sortkey_byte_length, 0);
	::LCMapStringW(LOCALE_USER_DEFAULT, LCMAP_SORTKEY, wstr.c_str(), wstr.length(), reinterpret_cast<LPWSTR>(UtfConversion::stringData(sortkey)), sortkey_byte_length);
	// see http://blogs.msdn.com/b/michkap/archive/2004/12/30/344389.aspx for LCMapString sortkey format.
	if (onlyPrimaryLevel) {
		// Cut sortkey at separator, to keep only primary level.
		static const UCHAR separator = 0x01;
		size_t separator_position = sortkey.find(separator);
		if (separator_position != sortkey.npos)
			sortkey_byte_length = separator_position;
	}
	else {
		// Remove unnecessary trailing null.
		while (sortkey_byte_length > 0 && sortkey[sortkey_byte_length - 1] == 0)
			sortkey_byte_length--;
	}
	sortkey.resize(sortkey_byte_length);
	return sortkey;
}
string_type u32_regex_traits::convertSortkeyToInts(const std::basic_string<UCHAR>& sortkey) {
	int length_as_ints = (sortkey.length()+2) / 3; // We store 3 bytes per int, to be certain we do not generate a negative int.
	string_type sortkey_ints;
	sortkey_ints.reserve(length_as_ints);
	unsigned int i;
	for (i = 3; i < sortkey.length(); i += 3)
		sortkey_ints += (sortkey[i-3]<<16) | (sortkey[i-2]<<8) | sortkey[i-1];
	U32 last_value = 0;
	switch (sortkey.length() % 3) {
		case 2: last_value |= sortkey[i-2]<<8;
		case 1: last_value |= sortkey[i-3]<<16;
			sortkey_ints += last_value;
	}
	return sortkey_ints;
}
