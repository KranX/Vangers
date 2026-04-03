#include "unicode.h"

namespace text
{

namespace
{

bool is_continuation(unsigned char ch)
{
	return (ch & 0xC0) == 0x80;
}

}

bool utf8_next(std::string_view text, size_t& offset, uint32_t& codepoint)
{
	if(offset >= text.size())
		return false;

	const unsigned char c0 = (unsigned char)text[offset];
	if(c0 < 0x80){
		codepoint = c0;
		offset++;
		return true;
	}

	if(c0 >= 0xC2 && c0 <= 0xDF){
		if(offset + 1 < text.size()){
			const unsigned char c1 = (unsigned char)text[offset + 1];
			if(is_continuation(c1)){
				codepoint = ((uint32_t)(c0 & 0x1F) << 6) | (uint32_t)(c1 & 0x3F);
				offset += 2;
				return true;
			}
		}
		codepoint = 0xFFFD;
		offset++;
		return true;
	}

	if(c0 >= 0xE0 && c0 <= 0xEF){
		if(offset + 2 < text.size()){
			const unsigned char c1 = (unsigned char)text[offset + 1];
			const unsigned char c2 = (unsigned char)text[offset + 2];
			if(is_continuation(c1) && is_continuation(c2)){
				const uint32_t cp = ((uint32_t)(c0 & 0x0F) << 12) |
				                    ((uint32_t)(c1 & 0x3F) << 6) |
				                    (uint32_t)(c2 & 0x3F);
				if(cp >= 0x800 && !(cp >= 0xD800 && cp <= 0xDFFF)){
					codepoint = cp;
					offset += 3;
					return true;
				}
			}
		}
		codepoint = 0xFFFD;
		offset++;
		return true;
	}

	if(c0 >= 0xF0 && c0 <= 0xF4){
		if(offset + 3 < text.size()){
			const unsigned char c1 = (unsigned char)text[offset + 1];
			const unsigned char c2 = (unsigned char)text[offset + 2];
			const unsigned char c3 = (unsigned char)text[offset + 3];
			if(is_continuation(c1) && is_continuation(c2) && is_continuation(c3)){
				const uint32_t cp = ((uint32_t)(c0 & 0x07) << 18) |
				                    ((uint32_t)(c1 & 0x3F) << 12) |
				                    ((uint32_t)(c2 & 0x3F) << 6) |
				                    (uint32_t)(c3 & 0x3F);
				if(cp >= 0x10000 && cp <= 0x10FFFF){
					codepoint = cp;
					offset += 4;
					return true;
				}
			}
		}
		codepoint = 0xFFFD;
		offset++;
		return true;
	}

	codepoint = 0xFFFD;
	offset++;
	return true;
}

bool is_valid_utf8(std::string_view text)
{
	size_t offset = 0;
	uint32_t codepoint = 0;

	while(offset < text.size()){
		size_t prev_offset = offset;
		if(!utf8_next(text, offset, codepoint))
			return false;
		if(codepoint == 0xFFFD && offset == prev_offset + 1 && (unsigned char)text[prev_offset] >= 0x80)
			return false;
	}

	return true;
}

size_t utf8_bom_size(std::string_view text)
{
	if(text.size() >= 3 &&
	   (unsigned char)text[0] == 0xEF &&
	   (unsigned char)text[1] == 0xBB &&
	   (unsigned char)text[2] == 0xBF)
		return 3;

	return 0;
}

size_t utf8_length(std::string_view text)
{
	size_t count = 0;
	size_t offset = 0;
	uint32_t codepoint = 0;

	while(utf8_next(text, offset, codepoint))
		count++;

	return count;
}

size_t utf8_codepoint_to_byte_offset(std::string_view text, size_t codepoint_index)
{
	size_t offset = 0;
	size_t count = 0;
	uint32_t codepoint = 0;

	while(count < codepoint_index && utf8_next(text, offset, codepoint))
		count++;

	return offset;
}

size_t utf8_byte_to_codepoint_index(std::string_view text, size_t byte_offset)
{
	size_t offset = 0;
	size_t count = 0;
	uint32_t codepoint = 0;

	if(byte_offset > text.size())
		byte_offset = text.size();

	while(offset < byte_offset && utf8_next(text, offset, codepoint))
		count++;

	return count;
}

uint32_t utf8_codepoint_at(std::string_view text, size_t codepoint_index, uint32_t fallback)
{
	size_t offset = utf8_codepoint_to_byte_offset(text, codepoint_index);
	uint32_t codepoint = fallback;
	if(!utf8_next(text, offset, codepoint))
		return fallback;
	return codepoint;
}

std::string utf8_substr_by_codepoints(std::string_view text, size_t start_codepoint, size_t codepoint_count)
{
	const size_t start = utf8_codepoint_to_byte_offset(text, start_codepoint);
	if(start >= text.size())
		return std::string();

	size_t end = text.size();
	if(codepoint_count != std::string_view::npos)
		end = utf8_codepoint_to_byte_offset(text.substr(start), codepoint_count) + start;

	if(end > text.size())
		end = text.size();
	return std::string(text.substr(start, end - start));
}

bool append_utf8(std::string& out, uint32_t codepoint)
{
	if(codepoint <= 0x7F){
		out.push_back((char)codepoint);
		return true;
	}

	if(codepoint <= 0x7FF){
		out.push_back((char)(0xC0 | (codepoint >> 6)));
		out.push_back((char)(0x80 | (codepoint & 0x3F)));
		return true;
	}

	if(codepoint >= 0xD800 && codepoint <= 0xDFFF)
		return false;

	if(codepoint <= 0xFFFF){
		out.push_back((char)(0xE0 | (codepoint >> 12)));
		out.push_back((char)(0x80 | ((codepoint >> 6) & 0x3F)));
		out.push_back((char)(0x80 | (codepoint & 0x3F)));
		return true;
	}

	if(codepoint <= 0x10FFFF){
		out.push_back((char)(0xF0 | (codepoint >> 18)));
		out.push_back((char)(0x80 | ((codepoint >> 12) & 0x3F)));
		out.push_back((char)(0x80 | ((codepoint >> 6) & 0x3F)));
		out.push_back((char)(0x80 | (codepoint & 0x3F)));
		return true;
	}

	return false;
}

std::string utf8_from_codepoint(uint32_t codepoint)
{
	std::string result;
	append_utf8(result, codepoint);
	return result;
}

}
