#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifndef TEST_DATA_DIR
#	error "TEST_DATA_DIR must point to the data directory"
#endif

namespace {

bool check(bool condition, const std::string &message) {
	if (!condition)
		std::cerr << message << '\n';
	return condition;
}

// Returns the last integer appearing in a `start_time { ... }` expression.
bool parse_last_offset(const std::string &line, int &offset) {
	int last = -1;
	bool found = false;
	for (std::size_t i = 0; i < line.size(); ++i) {
		if (!std::isdigit(static_cast<unsigned char>(line[i])))
			continue;
		int value = 0;
		while (i < line.size() && std::isdigit(static_cast<unsigned char>(line[i]))) {
			value = value * 10 + (line[i] - '0');
			++i;
		}
		last = value;
		found = true;
	}
	offset = last;
	return found;
}

int start_time_offset(const std::vector<std::string> &lines, std::size_t from, std::size_t end) {
	for (std::size_t i = from; i < end; ++i) {
		if (lines[i].find("start_time") == std::string::npos)
			continue;
		int offset = -1;
		if (parse_last_offset(lines[i], offset))
			return offset;
	}
	return -1;
}

std::size_t find_screen(const std::vector<std::string> &lines, const std::string &name) {
	const std::string marker = "Screen \"" + name + "\"";
	for (std::size_t i = 0; i < lines.size(); ++i) {
		if (lines[i].find(marker) != std::string::npos)
			return i;
	}
	return lines.size();
}

bool check_screen(
	const std::vector<std::string> &lines,
	const std::string &screen,
	std::size_t begin,
	std::size_t end
) {
	std::vector<std::size_t> events;
	for (std::size_t i = begin; i < end; ++i) {
		if (lines[i].find("Event {") != std::string::npos)
			events.push_back(i);
	}

	int checked_events = 0;
	for (std::size_t e = 0; e < events.size(); ++e) {
		const std::size_t event_begin = events[e];
		const std::size_t event_end = (e + 1 < events.size()) ? events[e + 1] : end;

		int config_offset = -1;
		int rise_offset = -1;
		for (std::size_t i = event_begin; i < event_end; ++i) {
			if (lines[i].find("iEXT_CHECK_SERVER_CONFIG") != std::string::npos)
				config_offset = start_time_offset(lines, i + 1, event_end);
			if (lines[i].find("rise_obj") != std::string::npos &&
				lines[i].find("\"Ok Option\"") != std::string::npos)
				rise_offset = start_time_offset(lines, i + 1, event_end);
		}
		if (config_offset < 0 || rise_offset < 0)
			continue;

		++checked_events;
		if (!check(
				rise_offset > config_offset,
				screen + ": OK recovery starts before configuration cancellation can select it"
			))
			return false;
	}

	return check(
		checked_events == 2, screen + ": expected cancellation recovery in both click paths"
	);
}

} // namespace

int main() {
	const std::string path = std::string(TEST_DATA_DIR) + "/iscreen/scripts/mainmenu.scr";
	std::ifstream file(path);
	if (!check(file.is_open(), "unable to open " + path))
		return 1;

	std::vector<std::string> lines;
	std::string line;
	while (std::getline(file, line))
		lines.push_back(line);

	const std::size_t first = find_screen(lines, "Identification screen");
	const std::size_t second = find_screen(lines, "Identification screen2");
	if (!check(first < lines.size() && second < lines.size(), "identification screens not found"))
		return 1;

	std::size_t first_end = second;
	for (std::size_t i = first + 1; i < lines.size(); ++i) {
		if (lines[i].rfind("Screen ", 0) == 0) {
			first_end = i;
			break;
		}
	}

	if (!check_screen(lines, "Identification screen", first, first_end))
		return 1;
	return check_screen(lines, "Identification screen2", second, lines.size()) ? 0 : 1;
}
