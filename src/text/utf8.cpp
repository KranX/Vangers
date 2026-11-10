#include "utf8.h"
#include "unicode_tables.h"

#include <algorithm>
#include <utility>

namespace vangers::text {

char32_t first_scalar(std::string_view value) {
	if (value.empty())
		return 0;
	const auto first = static_cast<unsigned char>(value[0]);
	if (first < 0x80)
		return first;
	char32_t cp = first & (first < 0xE0 ? 0x1F : first < 0xF0 ? 0x0F : 0x07);
	for (std::size_t i = 1; i < prefix_bytes(value, 1); ++i)
		cp = (cp << 6) | (static_cast<unsigned char>(value[i]) & 0x3F);
	return cp;
}

bool combining_mark(char32_t cp) {
	if (cp < 0x300)
		return false;
	const auto *end = std::end(unicode::marks);
	const auto *range = std::lower_bound(
		std::begin(unicode::marks), end, cp, [](const auto &r, char32_t c) { return r.last < c; }
	);
	return range != end && range->first <= cp;
}

char32_t last_scalar(std::string_view value) {
	if (value.empty())
		return 0;
	std::size_t pos = value.size();
	do {
		--pos;
	} while (pos && (static_cast<unsigned char>(value[pos]) & 0xC0) == 0x80);
	return first_scalar(value.substr(pos));
}

std::string compose_pairs(std::string_view value) {
	std::string out;
	std::size_t previous = 0;
	char32_t starter = 0;
	while (!value.empty()) {
		const auto cp = first_scalar(value);
		const auto bytes = prefix_bytes(value, 1);
		const auto key = std::make_pair(starter, cp);
		const auto *end = std::end(unicode::pairs);
		const auto *found = !out.empty() && cp >= 0x300
								? std::lower_bound(
									  std::begin(unicode::pairs),
									  end,
									  key,
									  [](const auto &p, const auto &k) {
										  return std::make_pair(p.first, p.second) < k;
									  }
								  )
								: end;
		if (!out.empty() && found != end && found->first == starter && found->second == cp) {
			out.resize(previous);
			starter = found->composed;
			if (starter < 0x80)
				out += char(starter);
			else {
				if (starter >= 0x10000)
					out += char(0xF0 | (starter >> 18));
				if (starter >= 0x800)
					out += char((starter >= 0x10000 ? 0x80 : 0xE0) | ((starter >> 12) & 0x3F));
				out += char((starter >= 0x800 ? 0x80 : 0xC0) | ((starter >> 6) & 0x3F));
				out += char(0x80 | (starter & 0x3F));
			}
		} else {
			previous = out.size();
			out.append(value.substr(0, bytes));
			starter = cp;
		}
		value.remove_prefix(bytes);
	}
	return out;
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
