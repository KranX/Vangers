#include "text/network_text.h"

#include <iostream>
#include <stdexcept>

namespace {
void check(bool condition, const char *message) {
	if (!condition)
		throw std::runtime_error(message);
}
} // namespace

int main() {
	try {
		using namespace vangers::network_text;
		const std::string legacy = "\x8f\xe0\xa8\xa2\xa5\xe2?";
		const auto ru = encode("Привет?", false);
		check(ru && *ru == legacy, "CP866 packet bytes changed");
		check(decode(legacy, false) == "Привет?", "Legacy chat/name decode changed");
		check(!decode(legacy, true), "UTF-8 mode silently guessed CP866");
		check(!encode("日本語 É", false), "Legacy mode silently replaced Unicode");
		check(encode("日本語 É", true) == "日本語 É", "Unicode mode changed the bytes");
		check(decode("日本語 É", true) == "日本語 É", "Unicode display decode changed");
		check(!decode(std::string_view("a\0b", 3), true), "Embedded NUL accepted");
		check(!encode(std::string_view("\xE6\x97", 2), true), "Incomplete scalar accepted");
		check(
			decode("日本語\nÉ\u2028Привет", true) == "日本語 É Привет",
			"Line controls leaked into UI"
		);
	} catch (const std::exception &error) {
		std::cerr << error.what() << '\n';
		return 1;
	}
}
