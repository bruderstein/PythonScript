/**
* Functions for UTF-8, UTF-16 and UTF-32.
* @author    Francois-R.Boyer@PolyMtl.ca
* @date      March, 2013
* @copyright GPL; see license.txt in Notepad++ source for complete license text.
* @file
*/

#include "stdafx.h"
#include "UtfConversion.h"

namespace UtfConversion {

const unsigned char _firstByteMaskForLength[7] = { 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01 };
const U32 _minimumCodePointForLength[7] = { 0, 0, 0x80, 0x800, 0x10000, 0x200000, 0x4000000 };

#pragma warning( push )
#pragma warning( disable : 4127 )
template <bool generate_utf8>
static unsigned int _toUtf8(const ConstString<U32> u32source, U8* u8destination = 0)
{
	static const UCHAR utf8_leading_byte[] = { 0, 0, 0xC0, 0xE0, 0xF0 };
	static const UCHAR utf8_continuation_byte = 0x80;
	unsigned int u8_length = 0;
	for (unsigned int i = 0; i < u32source.length(); ++i)
	{
		U32 u32c = u32source[i];
		if (isInvalidUtf8(u32c)) {
			if (generate_utf8)
				*(u8destination++) = U8(u32c - invalid_utf8_base);
			u8_length += 1;
			continue;
		}
		unsigned int c = unsigned int(u32c);
		const unsigned int u8_character_length = 
			(c <     0x80) ? 1 :
			(c <    0x800) ? 2 :
			(c <  0x10000) ? 3 :
			(c <= maximum_unicode_point) ? 4 : // Should not go higher than maximum_utf8_length.
							 0;
		if (generate_utf8 && u8_character_length != 0) {
			*(u8destination++) = U8((c >> (6*(u8_character_length-1))) | utf8_leading_byte[u8_character_length]);
			switch (u8_character_length)
			{
				case 4: *(u8destination++) = U8(((c >> 12) & 0x3F) | utf8_continuation_byte);
				case 3: *(u8destination++) = U8(((c >>  6) & 0x3F) | utf8_continuation_byte);
				case 2: *(u8destination++) = U8(((c >>  0) & 0x3F) | utf8_continuation_byte);
			}
		}
		u8_length += u8_character_length;
	}
	return u8_length;
}
#pragma warning( pop )

class Utf16Encoder {
public:
	Utf16Encoder(const U32* codepoints, size_t codepointsLeft) :
	            _codepoints(codepoints), _codepointsLeft(codepointsLeft) { }
	
	void encodeTo(U16* utf16Out) const {
		for (size_t i = 0; i < _codepointsLeft; i++) {
			U16_char u16c = toUtf16(_codepoints[i]);
			u16c.writeTo(utf16Out);
		}
	}
	
	unsigned int lengthInUtf16Words() const {
		unsigned int length = 0;
		for (size_t i = 0; i < _codepointsLeft; i++) {
			U16_char u16c = toUtf16(_codepoints[i]);
			length += u16c.length();
		}
		return length;
	}

private:
	const U32* _codepoints;
	size_t _codepointsLeft;
};

u32string toUtf32(const ConstString<U8> utf8)
{
	Utf8Decoder decoder(utf8.c_str(), utf8.length());
	u32string u32;
	u32.reserve(decoder.lengthInCharacters()); // Pre reserving requires to decode UTF-8, but reduces allocation and copy operations.
	while (!decoder.isEnd()) {
		u32 += decoder.decodedChar();
		decoder.advanceToNext();
	}
	return u32;
}

u8string toUtf8(const ConstString<U32> utf32source) {
	unsigned int lengthInUtf8Bytes = _toUtf8<false>(utf32source);
	u8string utf8Result(lengthInUtf8Bytes, 0);
	_toUtf8<true>(utf32source, stringData(utf8Result));
	return utf8Result;
}

u16string toUtf16(const ConstString<U32> utf32source) {
	Utf16Encoder encoder(utf32source.c_str(), utf32source.length());
	u16string utf16Result(encoder.lengthInUtf16Words(), 0);
	encoder.encodeTo(stringData(utf16Result));
	return utf16Result;
}

unsigned int Utf8Decoder::lengthInCharacters() {
	unsigned int bytesLeftBefore = _bytesLeft;
	unsigned int byteIndexBefore = _byteIndex;
	unsigned int length_in_characters = 0;
	while (!isEnd()) {
		length_in_characters++;
		advanceToNext();
	}
	_bytesLeft = bytesLeftBefore;
	_byteIndex = byteIndexBefore;
	decode();
	return length_in_characters;
}

void Utf8Decoder::decodeMultiByte() {
	UCHAR first_byte = getByte(0);
	if (Utf8::isFirstOfMultibyte(first_byte)) {
		unsigned int character_bytes = 1 + Utf8::continuationBytes(first_byte);
		const bool enough_bytes = _bytesLeft >= character_bytes;
		if (!enough_bytes) {
			invalidUtf8(first_byte); return;
		}
		_decodedChar = first_byte & _firstByteMaskForLength[character_bytes];
		for (unsigned int i = 1; i < character_bytes; ++i) {
			UCHAR current_byte = getByte(i);
			if (!Utf8::isContinuation(current_byte)) {
				invalidUtf8(first_byte); return;
			}
			_decodedChar = (_decodedChar << 6) | (current_byte & 0x3F);
		}
		if (_decodedChar < _minimumCodePointForLength[character_bytes]) {
			invalidUtf8(first_byte); return;
		}
		_decodedLength = character_bytes;
		return;
	}
	invalidUtf8(first_byte);
}

}