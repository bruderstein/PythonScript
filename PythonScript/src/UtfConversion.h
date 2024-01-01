#pragma once
/**
* Functions for UTF-8, UTF-16 and UTF-32.
* Similar to UniConversion.cxx in Scintilla, but with UTF-32, and with a special handling of invalid UTF-8.
* Invalid UTF-8 sequences are translated byte per byte to "UTF-32" in the range invalid_utf8_base + 0 to + 0xFF.  When translating from UTF-32 to UTF-8, they are mapped back to single bytes.  Note that UTF-16 cannot handle these codes, so any conversion to UTF-16 will lose these invalid UTF-8 sequences.
* @author    Francois-R.Boyer@PolyMtl.ca
* @date      March, 2013
* @copyright GPL; see license.txt in Notepad++ source for complete license text.
* @file
*/

#include <string>
#include "ConstString.h"
typedef unsigned char  UCHAR;
#include "Utf8.h"

// Base element of UTF-8/16/32:
typedef char           U8;
typedef wchar_t        U16;
typedef int            U32; // UTF-32 must be signed, otherwise its max value will overflow in (i > (int)(std::numeric_limits<charT>::max)()) comparison in basic_regex_parser.hpp.

// Strings in UTF-8/16/32:
typedef std::basic_string<U8>  u8string;
typedef std::basic_string<U16> u16string;
typedef std::basic_string<U32> u32string;

/// Complete UTF-16 character, with possible surrogate pair.
class U16_char {
public:
	U16_char() : _length(0) { _char[0] = _char[1] = 0; }
	void writeTo(U16*& utf16Out) const {
		for (unsigned int i = 0; i < _length; ++i)
			*utf16Out++ = _char[i];
	}
	operator const U16*() const { return _char; }
	operator U16*() { return _char; }
	unsigned int length() const { return _length; }
	unsigned int length(unsigned int newLength) { return _length = newLength; }
private:
	unsigned int _length;
	U16 _char[2];
};

namespace UtfConversion {
static const U32 INVALID = 0x7FFFFFFF;
static const U32 maximum_unicode_point = 0x10FFFF;
static const U32 maximum_utf8_length = 4;
static const U32 invalid_utf8_base = 0x40000000;

static const U16 surrogate_mask = 0xFC00;
static const U16 lead_surrogate_base = 0xD800;
static const U16 tail_surrogate_base = 0xDC00;

extern const unsigned char _firstByteMaskForLength[7];
extern const U32 _minimumCodePointForLength[7];

/// Test for UTF-16 surrogates.
inline bool isSurrogate(U32 b)     { return (b & (surrogate_mask ^ lead_surrogate_base ^ tail_surrogate_base)) == lead_surrogate_base; }
inline bool isLeadSurrogate(U32 b) { return (b & surrogate_mask) == lead_surrogate_base; }
inline bool isTailSurrogate(U32 b) { return (b & surrogate_mask) == tail_surrogate_base; }

inline bool isInvalidUtf8(U32 b) {
	return b >= invalid_utf8_base && b <= invalid_utf8_base+0xFF;
}
inline bool isValidCodepoint(U32 b) {
	if (b < 0
	    || b > UtfConversion::maximum_unicode_point
	    || isSurrogate(b)
	    || (b & 0xFFFF) >= 0xFFFE)
		return false;
	return true;
}

/// Permits direct access to string data as an array pointer.
/// Array should not be considered zero terminated; pointer to the array of an empty string should not be dereferenced.
template <class CharT>
inline CharT* stringData(std::basic_string<CharT>& str) {
	if (str.empty())
		return 0;
	return &*str.begin();
}

/// Converts single UTF-32 character to UTF-16.
inline U16_char toUtf16(U32 utf32)
{
	U16_char utf16Out;
	if (utf32 < 0x10000u) {
		utf16Out[0] = U16(utf32);
		utf16Out.length(1);
	}
	else if (utf32 <= maximum_unicode_point) {
		utf16Out[0] = U16(utf32 >> 10) + lead_surrogate_base;
		utf16Out[1] = U16(utf32 & 0x3F) + tail_surrogate_base;
		utf16Out.length(2);
	}
	else utf16Out.length(0);
	return utf16Out;
}

/// Converts single UTF-16 character to UTF-32.
inline U32 toUtf32(U16_char utf16)
{
	if (utf16.length() == 1)
		return utf16[0];
	if (utf16.length() == 2)
		return ((utf16[0]&0x3F)<<10) | (utf16[1]&0x3F);
	return INVALID;
}

/// Converts UTF-8 text to UTF-32.
u32string toUtf32(const ConstString<U8> utf8);

/// Converts UTF-32 text to UTF-8.
u8string toUtf8(const ConstString<U32> utf32source);

/// Converts UTF-32 text to UTF-16.
u16string toUtf16(const ConstString<U32> utf32source);

/// Template function that calls translation according to destination type.
template <class destination_string_type, class source_string_chartype>
destination_string_type toStringType(const ConstString<source_string_chartype>& source);
template <class destination_string_type, class source_string_chartype>
inline destination_string_type toStringType(const std::basic_string<source_string_chartype>& source) {
	return toStringType<destination_string_type>(ConstString<source_string_chartype>(source));
}
template <> inline u8string toStringType(const ConstString<U8>& source) {
	return source.toString();
}
template <> inline u8string toStringType(const ConstString<U32>& source) {
	return toUtf8(source);
}
template <> inline u32string toStringType(const ConstString<U8>& source) {
	return toUtf32(source);
}

/// Converts UTF-8 text to UTF-32 characters to be read one by one.
class Utf8Decoder {
public:
	Utf8Decoder(const U8* bytes, size_t bytesLeft, unsigned int byteIndex = 0) :
	            _bytes(bytes), _bytesLeft(bytesLeft), _byteIndex(byteIndex) {
		if (!isEnd())
			decode();
	}
	U32 decodedChar() const {   ///< Returns the character codepoint; an invalid UTF-32 code if UTF-8 is invalid.
		return _decodedChar;
	}
	U32 decodedLength() const { ///< Returns how many bytes were read to decode that character.
		return _decodedLength;
	}
	void advanceToNext() {
		if (!isEnd()) {
			_byteIndex += _decodedLength;
			_bytesLeft -= _decodedLength;
			decode();
		}
	}
	bool isEnd() const {
		return _bytesLeft == 0;
	}
	unsigned int lengthInCharacters(); ///< Returns the number of characters (number of UTF-32 codes that can be read, including invalid byte sequences) from current position to the end of the buffer.
private:
	void decode() {
		UCHAR current_byte = getByte(0);
		if (Utf8::isSingleByte(current_byte)) {
			_decodedChar = current_byte;
			_decodedLength = 1;
			return;
		}
		decodeMultiByte();
	}
	void invalidUtf8(UCHAR utf8Byte) {
		_decodedChar = invalid_utf8_base + utf8Byte;
		_decodedLength = 1;
	}
	UCHAR getByte(unsigned int i) const {
		return UCHAR(_bytes[_byteIndex + i]);
	}
	void decodeMultiByte();

	const U8* _bytes;
	size_t _bytesLeft;
	unsigned int _byteIndex;
	
	U32 _decodedChar;
	unsigned int _decodedLength;
};

}