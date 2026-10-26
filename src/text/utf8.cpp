#include "utf8.h"

namespace vangers::text {

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

void pop_back(std::string &text) {
	if (text.empty())
		return;
	std::size_t size = text.size();
	do {
		--size;
	} while (size && (static_cast<unsigned char>(text[size]) & 0xC0) == 0x80);
	text.resize(size);
}

} // namespace vangers::text
