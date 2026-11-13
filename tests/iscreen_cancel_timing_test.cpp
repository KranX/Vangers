#include <cctype>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
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

bool load_macros(const std::string &path, std::map<std::string, int> &macros) {
	std::ifstream file(path);
	if (!file.is_open())
		return false;

	std::string line;
	while (std::getline(file, line)) {
		const std::string define = "#define";
		if (line.rfind(define, 0) != 0)
			continue;
		std::size_t pos = define.size();
		while (pos < line.size() && std::isspace(static_cast<unsigned char>(line[pos])))
			++pos;
		std::size_t name_begin = pos;
		while (pos < line.size() &&
			   (std::isalnum(static_cast<unsigned char>(line[pos])) || line[pos] == '_'))
			++pos;
		if (name_begin == pos)
			continue;
		const std::string name = line.substr(name_begin, pos - name_begin);
		while (pos < line.size() && std::isspace(static_cast<unsigned char>(line[pos])))
			++pos;
		std::size_t value_begin = pos;
		while (pos < line.size() && std::isdigit(static_cast<unsigned char>(line[pos])))
			++pos;
		if (value_begin == pos)
			continue;
		macros[name] = std::stoi(line.substr(value_begin, pos - value_begin));
	}
	return true;
}

// Evaluates the additive `$MACRO + number` expressions used by the script.
bool evaluate_expression(
	const std::string &input,
	const std::map<std::string, int> &macros,
	int &value
) {
	std::string line = input;
	std::size_t begin = line.find('{');
	std::size_t end = line.rfind('}');
	if (begin != std::string::npos && end != std::string::npos && begin < end)
		line = line.substr(begin + 1, end - begin - 1);
	else {
		const std::string keyword =
			line.find("start_time") != std::string::npos ? "start_time" : "time";
		begin = line.find(keyword);
		if (begin == std::string::npos)
			return false;
		line = line.substr(begin + keyword.size());
	}

	int sum = 0;
	bool found = false;
	for (std::size_t i = 0; i < line.size();) {
		const unsigned char c = static_cast<unsigned char>(line[i]);
		if (std::isdigit(c)) {
			int number = 0;
			while (i < line.size() && std::isdigit(static_cast<unsigned char>(line[i]))) {
				number = number * 10 + (line[i] - '0');
				++i;
			}
			sum += number;
			found = true;
			continue;
		}
		if (line[i] == '$') {
			++i;
			std::size_t name_begin = i;
			while (i < line.size() &&
				   (std::isalnum(static_cast<unsigned char>(line[i])) || line[i] == '_'))
				++i;
			const std::string name = line.substr(name_begin, i - name_begin);
			const auto it = macros.find(name);
			if (it == macros.end())
				return false;
			sum += it->second;
			found = true;
			continue;
		}
		++i;
	}
	value = sum;
	return found;
}

bool starts_with_keyword(const std::string &line, const char *keyword) {
	std::size_t i = 0;
	while (i < line.size() && std::isspace(static_cast<unsigned char>(line[i])))
		++i;
	const std::size_t length = std::strlen(keyword);
	if (line.compare(i, length, keyword) != 0)
		return false;
	const std::size_t after = i + length;
	return after == line.size() || std::isspace(static_cast<unsigned char>(line[after])) ||
		   line[after] == '{';
}

int start_time_value(
	const std::vector<std::string> &lines,
	const std::map<std::string, int> &macros,
	std::size_t from,
	std::size_t end
) {
	for (std::size_t i = from; i < end; ++i) {
		if (!starts_with_keyword(lines[i], "start_time"))
			continue;
		int value = 0;
		if (evaluate_expression(lines[i], macros, value))
			return value;
	}
	return -1;
}

int time_value(
	const std::vector<std::string> &lines,
	const std::map<std::string, int> &macros,
	std::size_t from,
	std::size_t end
) {
	for (std::size_t i = from; i < end; ++i) {
		if (!starts_with_keyword(lines[i], "time"))
			continue;
		int value = 0;
		if (evaluate_expression(lines[i], macros, value))
			return value;
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
	const std::map<std::string, int> &macros,
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

		int event_time = -1;
		int config_offset = -1;
		int rise_offset = -1;
		int rise_time = -1;
		for (std::size_t i = event_begin; i < event_end; ++i) {
			if (event_time < 0 && starts_with_keyword(lines[i], "time"))
				event_time = time_value(lines, macros, i, i + 1);

			if (lines[i].find("iEXT_CHECK_SERVER_CONFIG") != std::string::npos)
				config_offset = start_time_value(lines, macros, i + 1, event_end);

			if (lines[i].find("rise_obj") != std::string::npos &&
				lines[i].find("\"Ok Option\"") != std::string::npos) {
				rise_offset = start_time_value(lines, macros, i + 1, event_end);
				rise_time = time_value(lines, macros, i + 1, event_end);
			}
		}
		if (event_time < 0 || config_offset < 0 || rise_offset < 0 || rise_time < 0)
			continue;

		++checked_events;
		if (!check(
				rise_offset > config_offset,
				screen + ": OK recovery starts before configuration cancellation can select it"
			))
			return false;
		if (!check(
				rise_offset + rise_time <= event_time,
				screen + ": OK recovery cannot finish before the event ends"
			))
			return false;
	}

	return check(
		checked_events == 2, screen + ": expected cancellation recovery in both click paths"
	);
}

} // namespace

int main() {
	const std::string data_dir = TEST_DATA_DIR;
	const std::string path = data_dir + "/iscreen/scripts/mainmenu.scr";
	std::ifstream file(path);
	if (!check(file.is_open(), "unable to open " + path))
		return 1;

	std::map<std::string, int> macros;
	if (!check(
			load_macros(data_dir + "/iscreen/scripts/oftr.inc", macros),
			"unable to read script timing macros"
		))
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

	if (!check_screen(lines, macros, "Identification screen", first, first_end))
		return 1;
	return check_screen(lines, macros, "Identification screen2", second, lines.size()) ? 0 : 1;
}
