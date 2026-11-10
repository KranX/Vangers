#include "layout.h"
#include "utf8.h"

#include <algorithm>
#include <stdexcept>

namespace vangers::text {
namespace {
bool cjk(char32_t cp) {
	return (cp >= 0x2E80 && cp <= 0x9FFF) || (cp >= 0xF900 && cp <= 0xFAFF) ||
		   (cp >= 0xFF00 && cp <= 0xFFEF) || (cp >= 0x20000 && cp <= 0x323AF);
}

bool listed(char32_t cp, std::u32string_view values) {
	return values.find(cp) != std::u32string_view::npos;
}

// Horizontal Japanese kinsoku: choose a legal boundary within the measured
// width, never add a page, shrink glyphs, or hang punctuation outside the box.
bool legal_boundary(char32_t before, char32_t after) {
	return !combining_mark(after) && !listed(before, U"（〔［｛〈《「『【〖〘〚‘“([{\uFF62") &&
		   !listed(
			   after,
			   U"）〕］｝〉》」』】〗〙〛’”、。，．・：；？！ー々ゝゞヽヾ゛゜ぁぃぅぇぉっゃゅょゎゕ"
			   U"ゖァィゥェォッャュョヮヵヶㇰㇱㇲㇳㇴㇵㇶㇷㇸㇹㇺㇻㇼㇽㇾㇿ､｡･ｰｧｨｩｪｫｬｭｮｯﾞﾟ),.!?:;]}"
			   U"\uFF63"
		   );
}

// Measure only whole combining sequences: a completed accented glyph may be
// narrower than its bare starter. Scalar-only binary search would reject it.
std::size_t complete_prefix(std::string_view text, std::size_t count) {
	auto end = prefix_bytes(text, count);
	while (end < text.size() && combining_mark(first_scalar(text.substr(end))))
		end += prefix_bytes(text.substr(end), 1);
	return end;
}

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
			const auto end = complete_prefix(line, high);
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
			if (measure(line.substr(0, complete_prefix(line, mid))) <= width)
				low = mid;
			else
				high = mid;
		}
		const auto fit = complete_prefix(line, std::max<std::size_t>(1, low));
		if (!low && (cjk(first_scalar(line)) || fit > prefix_bytes(line, 1)))
			throw std::runtime_error("Text box cannot fit a complete CJK/combining sequence");
		const auto space = line.substr(0, std::min(line.size(), fit + 1)).find_last_of(" \t");
		std::size_t candidate = 0;
		bool constrained = false;
		for (std::size_t pos = 0; pos < fit;) {
			const auto before = first_scalar(line.substr(pos));
			pos += prefix_bytes(line.substr(pos), 1);
			const auto after = first_scalar(line.substr(pos));
			if (cjk(before) || cjk(after)) {
				constrained = true;
				if (legal_boundary(before, after))
					candidate = pos;
			}
		}
		if (constrained) {
			// A space is also a valid opportunity unless it strands prohibited
			// punctuation. Prefer the last legal CJK boundary, not an early space.
			if (space != std::string_view::npos && space &&
				legal_boundary(U' ', first_scalar(line.substr(space + 1))))
				candidate = std::max(candidate, space);
			if (!candidate)
				throw std::runtime_error("Text box cannot fit a legal CJK line");
			out.emplace_back(line.substr(0, candidate));
			line.remove_prefix(candidate);
			if (!keep_spaces && !line.empty() && (line.front() == ' ' || line.front() == '\t'))
				line.remove_prefix(1);
			continue;
		}
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
		if (pending_space && !part.empty() && !paragraph.empty() &&
			!(cjk(last_scalar(paragraph)) && cjk(first_scalar(part))))
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
	if (pending_space && !tail.empty() && !paragraph.empty() &&
		!(cjk(last_scalar(paragraph)) && cjk(first_scalar(tail))))
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
