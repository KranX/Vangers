#include "settings/text_encoding.h"
#include "text/layout.h"
#include "text/utf8.h"
#include <iostream>
#include <stdexcept>

using namespace vangers::text;
void check(bool value, const char *message) {
	if (!value)
		throw std::runtime_error(message);
}
int main() {
	const auto measure = [](std::string_view s) { return int(text_length(s)); };
	check(
		layout_lines("one two three", 7, measure) == Lines({"one two", "three"}),
		"Measured word wrapping"
	);
	check(layout_lines("a\r\n\nb", 20, measure) == Lines({"a", "", "b"}), "Authored hard breaks");
	check(
		layout_lines("one\ntwo\n\nthree", 20, measure, LineMode::Paragraphs) ==
			Lines({"one two", "", "three"}),
		"Paragraph folding"
	);
	check(
		layout_lines("one\xe2\x80\xa8two", 20, measure, LineMode::Paragraphs) ==
			Lines({"one", "two"}),
		"Forced line separator"
	);
	check(
		layout_lines("one\xc2\xa0two", 5, measure).front() == "one\xc2\xa0t",
		"NBSP is not discarded as ASCII space"
	);
	const auto pages = paginate(layout_lines("first\fsecond\nthird", 20, measure), 1);
	check(
		pages == Pages({{"first"}, {"second"}, {"third"}}), "Height pagination and hard page break"
	);
	check(
		paginate(layout_lines("one\f\r\ntwo", 20, measure), 5) == Pages({{"one"}, {"two"}}),
		"Page marker line created an empty row"
	);
	for (const auto &text : {std::string(),
			 std::string("  hello world again"),
			 std::string("Ёж 日本 😀 longwordwithoutspaces")})
		for (int width : {1, 2, 5, 20}) {
			std::string restored;
			for (const auto &line : layout_lines(text, width, measure, LineMode::Preserve, true)) {
				check(vangers::settings::is_valid_utf8(line), "Split UTF-8");
				check(measure(line) <= width, "Line exceeds width");
				restored += line;
			}
			check(restored == text, "Legacy spaces or characters lost");
		}
	check(
		layout_lines("hello world again", 11, measure, LineMode::Preserve, true) ==
			Lines({"hello world", " again"}),
		"Dialogue leading-space convention"
	);
	check(
		layout_lines("one\n\n\ntwo", 20, measure, LineMode::Paragraphs) ==
			Lines({"one", "", "", "two"}),
		"Intentional paragraph gaps"
	);
	check(
		layout_lines("$\x1e\x1f", 3, measure) == Lines({"$\x1e\x1f"}),
		"Game icon/control glyphs lost"
	);

	std::size_t maximum_query = 0;
	const auto bounded = [&](std::string_view s) {
		maximum_query = std::max(maximum_query, s.size());
		return int(s.size());
	};
	auto long_lines = layout_lines(std::string(20000, 'A'), 80, bounded);
	check(long_lines.size() == 250 && maximum_query <= 128, "Unbounded paragraph metric queries");
	check(paginate({}, 3).size() == 1, "Empty text has a page");
	for (int invalid : {0, -1}) {
		bool rejected = false;
		try {
			paginate({"x"}, invalid);
		} catch (const std::invalid_argument &) {
			rejected = true;
		}
		check(rejected, "Invalid height accepted");
	}
	std::cout
		<< "Measured lines, paragraphs, page breaks, Unicode and metric query budget passed\n";
}
