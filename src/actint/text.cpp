#include "text.h"

#include <cstring>
#include <stdexcept>

namespace vangers::actint {

std::string decode_text(std::string_view text, TextEncoding encoding) {
	if (encoding == TextEncoding::Cp866)
		return settings::cp866_to_utf8(text);
	if (encoding != TextEncoding::Utf8 || !settings::is_valid_utf8(text))
		throw std::invalid_argument("Invalid actint UTF-8 text");
	return std::string(text);
}

std::string raster_text(std::string_view text, std::size_t glyph_count) {
	if (!glyph_count)
		return {};
	auto bytes = settings::utf8_to_cp866(text);
	const char fallback = glyph_count > '?' ? '?' : glyph_count > ' ' ? ' ' : '\0';
	for (char &byte : bytes)
		if (static_cast<unsigned char>(byte) >= glyph_count)
			byte = fallback;
	return bytes;
}

char *copy_text(std::string_view text) {
	auto *copy = new char[text.size() + 1];
	if (!text.empty())
		std::memcpy(copy, text.data(), text.size());
	copy[text.size()] = '\0';
	return copy;
}

std::size_t prefix_bytes(std::string_view text, std::size_t characters) {
	std::size_t pos = 0;
	while (pos < text.size() && characters) {
		++pos;
		while (pos < text.size() && (static_cast<unsigned char>(text[pos]) & 0xC0) == 0x80)
			++pos;
		--characters;
	}
	return pos;
}

std::size_t text_length(std::string_view text) {
	std::size_t count = 0;
	for (unsigned char byte : text)
		if ((byte & 0xC0) != 0x80)
			++count;
	return count;
}

void pop_back(char *text) {
	std::size_t size = std::strlen(text);
	if (!size)
		return;
	do {
		--size;
	} while (size && (static_cast<unsigned char>(text[size]) & 0xC0) == 0x80);
	text[size] = '\0';
}

std::vector<std::string> wrap_text(std::string_view text, int width, const TextWidth &measure) {
	std::vector<std::string> lines;
	do {
		std::size_t end = text.size();
		while (measure(text.substr(0, end)) >= width && end) {
			const auto space = text.substr(0, end).find_last_of(' ');
			if (space != std::string_view::npos && space) {
				end = space;
			} else {
				// A long word must make progress without cutting a UTF-8 sequence.
				end = 0;
				for (std::size_t next = prefix_bytes(text, 1); next <= text.size();) {
					if (end && measure(text.substr(0, next)) >= width)
						break;
					end = next;
					if (next == text.size())
						break;
					next += prefix_bytes(text.substr(next), 1);
				}
				break;
			}
		}
		lines.emplace_back(text.substr(0, end));
		text.remove_prefix(end);
	} while (!text.empty());
	return lines;
}

std::string wrap_columns(std::string_view text, int columns) {
	std::string result;
	if (columns <= 0)
		return std::string(text);
	while (!text.empty()) {
		const auto newline = text.find('\n');
		auto line = text.substr(0, newline);
		while (text_length(line) > static_cast<std::size_t>(columns)) {
			auto end = prefix_bytes(line, columns);
			const auto space = line.substr(0, end + 1).find_last_of(" \t");
			if (space != std::string_view::npos && space)
				end = space;
			result.append(line.substr(0, end));
			result += '\n';
			line.remove_prefix(end);
			if (!line.empty() && (line.front() == ' ' || line.front() == '\t'))
				line.remove_prefix(1);
		}
		result.append(line);
		if (newline == std::string_view::npos)
			break;
		result += '\n';
		text.remove_prefix(newline + 1);
	}
	return result;
}

} // namespace vangers::actint
