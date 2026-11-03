#include "iscreen/text.h"
#include "settings/text_encoding.h"
#include "text/utf8.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace {
void check(bool condition, const char *message) {
	if (!condition) {
		std::cerr << message << '\n';
		std::exit(1);
	}
}
} // namespace

int main() {
	using namespace vangers::iscreen;
	const std::string sample = "Есть Ёж";
	check(
		resource_text(vangers::settings::utf8_to_cp866(sample), false) == sample,
		"Legacy input must decode once"
	);
	check(
		resource_text("Ё日😀", true) == "Ё日😀", "UTF-8 resource must not round-trip through CP866"
	);
	check(
		text_lines("a\r\n\r\nб\nв", true) == std::vector<std::string>({"a", "", "б", "в"}),
		"Preserve blank lines and final unterminated line"
	);
	check(text_lines("", true).empty(), "Empty text must have no invalid first line");
	check(
		text_lines("a\n", true) == std::vector<std::string>({"a"}),
		"A terminator must not add a page line"
	);
	char buffer[4 * 4 + 2]{};
	copy_text(buffer, "Ё日😀abc", 4);
	check(std::string(buffer) == "Ё日😀a", "Copy must truncate only at a scalar boundary");
	copy_text(buffer, buffer + 2, 4);
	check(std::string(buffer) == "日😀a", "Aliased copy must be safe");
	std::string input;
	check(
		append_input(input, "Есть Ёж", 49, false) && input == sample,
		"Use the complete SDL text event"
	);
	check(
		append_input(input, "日😀", 9, false) && input == sample + "日😀",
		"Keep unsupported glyphs in storage"
	);
	vangers::text::pop_back(input);
	check(input == sample + "日", "Backspace must remove a complete scalar");
	check(!append_input(input, "\xff", 49, false), "Invalid input must not corrupt storage");
	input.clear();
	check(
		append_input(input, "12я3\n45", 4, true) && input == "1234",
		"Numeric fields must process all digits and enforce their limit"
	);
	bool rejected = false;
	try {
		resource_text("\xff", true);
	} catch (const std::invalid_argument &) {
		rejected = true;
	}
	check(rejected, "Explicit UTF-8 resources must reject malformed bytes");
}
