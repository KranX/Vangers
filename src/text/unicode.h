#ifndef __TEXT_UNICODE_H__
#define __TEXT_UNICODE_H__

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace text
{

bool utf8_next(std::string_view text, size_t& offset, uint32_t& codepoint);
size_t utf8_length(std::string_view text);
size_t utf8_codepoint_to_byte_offset(std::string_view text, size_t codepoint_index);
size_t utf8_byte_to_codepoint_index(std::string_view text, size_t byte_offset);
uint32_t utf8_codepoint_at(std::string_view text, size_t codepoint_index, uint32_t fallback = 0xFFFD);
std::string utf8_substr_by_codepoints(std::string_view text, size_t start_codepoint, size_t codepoint_count = std::string_view::npos);
bool append_utf8(std::string& out, uint32_t codepoint);
std::string utf8_from_codepoint(uint32_t codepoint);

}

#endif
