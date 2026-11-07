#ifndef VANGERS_TEXT_LAYOUT_H
#define VANGERS_TEXT_LAYOUT_H

#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace vangers::text {
// Encoding and layout are independent. Existing authored lines stay hard breaks.
enum class LineMode { Preserve, Paragraphs };
using Measure = std::function<int(std::string_view)>;
using Lines = std::vector<std::string>;
using Pages = std::vector<Lines>;

// Form feed is an explicit page break. U+2028 is always a hard line break.
// Paragraph mode folds physical lines, preserving blank paragraphs and NBSP.
Lines layout_lines(
	std::string_view text,
	int width,
	const Measure &measure,
	LineMode mode = LineMode::Preserve,
	bool keep_wrap_spaces = false
);
Pages paginate(const Lines &lines, int rows);
} // namespace vangers::text
#endif
