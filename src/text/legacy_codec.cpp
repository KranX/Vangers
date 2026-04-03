#include "legacy_codec.h"

#include "unicode.h"

namespace text
{

namespace
{

uint32_t decode_legacy_byte(unsigned char ch,LegacyEncoding encoding)
{
	switch(encoding){
		case LegacyEncoding::ASCII:
			return ch < 0x80 ? ch : 0xFFFD;
		case LegacyEncoding::CP866:
			return cp866_to_unicode(ch);
		case LegacyEncoding::CP1251:
			return cp1251_to_unicode(ch);
	}
	return 0xFFFD;
}

std::string legacy_to_utf8_impl(std::string_view text,LegacyEncoding encoding)
{
	std::string result;
	for(unsigned char ch : text){
		uint32_t codepoint = decode_legacy_byte(ch,encoding);
		if(!append_utf8(result, codepoint))
			append_utf8(result, '?');
	}
	return result;
}

}

uint32_t cp866_to_unicode(unsigned char ch)
{
	if(ch < 0x80)
		return ch;

	if(ch >= 0x80 && ch <= 0xAF)
		return 0x0410 + (ch - 0x80);

	if(ch >= 0xE0 && ch <= 0xEF)
		return 0x0440 + (ch - 0xE0);

	switch(ch){
		case 0xF0: return 0x0401;
		case 0xF1: return 0x0451;
		case 0xFC: return 0x2116;
		default: return 0xFFFD;
	}
}

uint32_t cp1251_to_unicode(unsigned char ch)
{
	if(ch < 0x80)
		return ch;

	if(ch >= 0xC0 && ch <= 0xFF)
		return 0x0410 + (ch - 0xC0);

	switch(ch){
		case 0xA8: return 0x0401;
		case 0xB8: return 0x0451;
		case 0xB9: return 0x2116;
		default: return 0xFFFD;
	}
}

unsigned char unicode_to_cp866_lossy(uint32_t codepoint,unsigned char fallback)
{
	if(codepoint < 0x80)
		return (unsigned char)codepoint;

	if(codepoint >= 0x0410 && codepoint <= 0x043F)
		return (unsigned char)(codepoint - 0x0410 + 0x80);

	if(codepoint >= 0x0440 && codepoint <= 0x044F)
		return (unsigned char)(codepoint - 0x0440 + 0xE0);

	switch(codepoint){
		case 0x0401: return 0xF0;
		case 0x0451: return 0xF1;
		case 0x2116: return 0xFC;
		default: return fallback;
	}
}

unsigned char unicode_to_cp1251_lossy(uint32_t codepoint,unsigned char fallback)
{
	if(codepoint < 0x80)
		return (unsigned char)codepoint;

	if(codepoint >= 0x0410 && codepoint <= 0x044F)
		return (unsigned char)(codepoint - 0x0410 + 0xC0);

	switch(codepoint){
		case 0x0401: return 0xA8;
		case 0x0451: return 0xB8;
		case 0x2116: return 0xB9;
		default: return fallback;
	}
}

unsigned char unicode_to_legacy_lossy(uint32_t codepoint,LegacyEncoding encoding,unsigned char fallback)
{
	switch(encoding){
		case LegacyEncoding::ASCII:
			return codepoint < 0x80 ? (unsigned char)codepoint : fallback;
		case LegacyEncoding::CP866:
			return unicode_to_cp866_lossy(codepoint, fallback);
		case LegacyEncoding::CP1251:
			return unicode_to_cp1251_lossy(codepoint, fallback);
	}

	return fallback;
}

std::string cp866_to_utf8(std::string_view text)
{
	return legacy_to_utf8_impl(text, LegacyEncoding::CP866);
}

std::string cp1251_to_utf8(std::string_view text)
{
	return legacy_to_utf8_impl(text, LegacyEncoding::CP1251);
}

std::string legacy_to_utf8(std::string_view text,LegacyEncoding encoding)
{
	return legacy_to_utf8_impl(text, encoding);
}

std::string utf8_to_legacy_lossy(std::string_view text,LegacyEncoding encoding,unsigned char fallback)
{
	std::string result;
	size_t offset = 0;
	uint32_t codepoint = 0;

	while(utf8_next(text, offset, codepoint))
		result.push_back((char)unicode_to_legacy_lossy(codepoint, encoding, fallback));

	return result;
}

std::string utf8_to_cp866_lossy(std::string_view text,unsigned char fallback)
{
	return utf8_to_legacy_lossy(text, LegacyEncoding::CP866, fallback);
}

unsigned char utf8_first_codepoint_to_cp866_lossy(std::string_view text,unsigned char fallback)
{
	size_t offset = 0;
	uint32_t codepoint = 0;
	if(!utf8_next(text, offset, codepoint))
		return fallback;
	return unicode_to_cp866_lossy(codepoint, fallback);
}

std::string cp866_char_to_utf8(unsigned char ch)
{
	std::string result;
	if(!append_utf8(result, cp866_to_unicode(ch)))
		append_utf8(result, '?');
	return result;
}

}
