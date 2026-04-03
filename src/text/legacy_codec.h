#ifndef __TEXT_LEGACY_CODEC_H__
#define __TEXT_LEGACY_CODEC_H__

#include <cstdint>
#include <string>
#include <string_view>

namespace text
{

enum class LegacyEncoding
{
	ASCII,
	CP866,
	CP1251
};

uint32_t cp866_to_unicode(unsigned char ch);
uint32_t cp1251_to_unicode(unsigned char ch);
unsigned char unicode_to_cp866_lossy(uint32_t codepoint,unsigned char fallback = 0xDB);
unsigned char unicode_to_cp1251_lossy(uint32_t codepoint,unsigned char fallback = '?');
unsigned char unicode_to_legacy_lossy(uint32_t codepoint,LegacyEncoding encoding,unsigned char fallback = '?');

std::string cp866_to_utf8(std::string_view text);
std::string cp1251_to_utf8(std::string_view text);
std::string legacy_to_utf8(std::string_view text,LegacyEncoding encoding);
std::string utf8_to_legacy_lossy(std::string_view text,LegacyEncoding encoding,unsigned char fallback = '?');
std::string utf8_to_cp866_lossy(std::string_view text,unsigned char fallback = 0xDB);

unsigned char utf8_first_codepoint_to_cp866_lossy(std::string_view text,unsigned char fallback = 0xDB);
std::string cp866_char_to_utf8(unsigned char ch);

}

#endif
