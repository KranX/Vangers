#include "layout.h"
#include "utf8.h"

#include <algorithm>
#include <stdexcept>

namespace vangers::text {
namespace {
void wrap(std::string_view line, int width, const Measure &measure, Lines &out, bool keep_spaces) {
	if (line.empty()) {
		out.emplace_back();
		return;
	}
	while (!line.empty()) {
		// Bound metric queries by a line's width, not the entire remaining novel.
		std::size_t low = 0, high = 1;
		bool complete = false;
		while (true) {
			const auto end = prefix_bytes(line, high);
			if (measure(line.substr(0, end)) > width)
				break;
			low = high;
			if (end == line.size()) {
				complete = true;
				break;
			}
			high = std::min(line.size(), high * 2);
		}
		if (complete) {
			out.emplace_back(line);
			break;
		}
		while (low + 1 < high) {
			const auto mid = low + (high - low) / 2;
			if (measure(line.substr(0, prefix_bytes(line, mid))) <= width)
				low = mid;
			else
				high = mid;
		}
		const auto fit = prefix_bytes(line, std::max<std::size_t>(1, low));
		const auto space = line.substr(0, std::min(line.size(), fit + 1)).find_last_of(" \t");
		if (space != std::string_view::npos && space) {
			out.emplace_back(line.substr(0, space));
			line.remove_prefix(space + (keep_spaces ? 0 : 1));
		} else {
			out.emplace_back(line.substr(0, fit));
			line.remove_prefix(fit);
		}
	}
}
} // namespace

Lines layout_lines(
	std::string_view text,
	int width,
	const Measure &measure,
	LineMode mode,
	bool keep_wrap_spaces
) {
	if (width <= 0)
		throw std::invalid_argument("Text box width must be positive");
	Lines out;
	std::string paragraph;
	const auto flush = [&] {
		wrap(paragraph, width, measure, out, keep_wrap_spaces);
		paragraph.clear();
	};
	std::size_t start = 0;
	bool pending_space = false;
	for (std::size_t pos = 0; pos < text.size();) {
		const bool separator = text.substr(pos, 3) == "\xe2\x80\xa8";
		const char c = text[pos];
		if (c != '\r' && c != '\n' && c != '\f' && !separator) {
			++pos;
			continue;
		}
		const auto part = text.substr(start, pos - start);
		if (pending_space && !part.empty() && !paragraph.empty())
			paragraph += ' ';
		paragraph.append(part);
		if (c == '\f') {
			if (!paragraph.empty())
				flush();
			out.emplace_back("\f");
			pending_space = false;
			// A page marker on its own physical line is not an extra blank row.
			if (pos + 1 < text.size() && text[pos + 1] == '\r')
				++pos;
			if (pos + 1 < text.size() && text[pos + 1] == '\n')
				++pos;
		} else if (mode == LineMode::Preserve || separator || part.empty()) {
			const bool blank_paragraph =
				mode == LineMode::Paragraphs && part.empty() && !paragraph.empty();
			flush();
			if (blank_paragraph)
				out.emplace_back();
			pending_space = false;
		} else
			pending_space = true;
		pos +=
			separator ? 3 : (c == '\r' && pos + 1 < text.size() && text[pos + 1] == '\n' ? 2 : 1);
		start = pos;
	}
	const auto tail = text.substr(start);
	if (pending_space && !tail.empty() && !paragraph.empty())
		paragraph += ' ';
	paragraph.append(tail);
	if (!paragraph.empty() || out.empty())
		flush();
	return out;
}

Pages paginate(const Lines &lines, int rows) {
	if (rows <= 0)
		throw std::invalid_argument("Text box must fit at least one row");
	Pages pages(1);
	for (const auto &line : lines) {
		if (line == "\f") {
			if (!pages.back().empty())
				pages.emplace_back();
			continue;
		}
		if (int(pages.back().size()) == rows)
			pages.emplace_back();
		pages.back().push_back(line);
	}
	if (pages.size() > 1 && pages.back().empty())
		pages.pop_back();
	return pages;
}
} // namespace vangers::text
