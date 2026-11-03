#include "saves/text.h"
#include "settings/text_encoding.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace {
void check(bool value, const char *message) {
	if (!value)
		throw std::runtime_error(message);
}
void write(uint32_t header, const std::string &name, const std::string &description) {
	std::ofstream file("test-save.dat", std::ios::binary);
	for (int i = 0; i < 4; ++i)
		file.put(char(header >> (8 * i)));
	file << name << description;
	check(file.good(), "Cannot write fixture");
}
} // namespace
int main() {
	try {
		using namespace vangers::saves;
		using namespace vangers::settings;
		const std::string name = "Ёж 日本 😀", detail = "Место日, 1:00:00";
		for (bool packed : {false, true}) {
			const int flags = packed ? 1 << 24 : 0;
			write(text_header(name, detail) | flags, name, detail);
			auto result = read_text("test-save.dat");
			check(
				result && result->name == name && result->description == detail,
				"Unicode header round-trip failed"
			);
			const auto legacy_name = utf8_to_cp866("Ёж"), legacy_detail = utf8_to_cp866("Фострал");
			write(
				legacy_name.size() | (legacy_detail.size() << 16) | flags,
				legacy_name,
				legacy_detail
			);
			result = read_text("test-save.dat");
			check(
				result && result->name == "Ёж" && result->description == "Фострал",
				"Old save decode failed"
			);
			// Old bytes that also happen to be valid UTF-8 are still CP866, never guessed.
			write(name.size() | flags, name, "");
			check(
				read_text("test-save.dat")->name == cp866_to_utf8(name), "Encoding was autodetected"
			);
			write(utf8_text | flags | 1, "\xff", "");
			check(!read_text("test-save.dat"), "Malformed UTF-8 accepted");
			write(utf8_text | flags | 1, std::string(1, '\0'), "");
			check(!read_text("test-save.dat"), "Embedded NUL accepted");
			write(text_header(name, detail) | flags, name, "");
			check(!read_text("test-save.dat"), "Truncated text accepted");
		}
		const std::string maximum(255, 'x');
		write(text_header(maximum, maximum), maximum, maximum);
		check(read_text("test-save.dat")->name == maximum, "Maximum length failed");
		for (const auto bad :
			{std::string("\xff"), std::string(256, 'x'), std::string("a\0b", 3)}) {
			bool rejected = false;
			try {
				text_header(bad, "");
			} catch (const std::invalid_argument &) {
				rejected = true;
			}
			check(rejected, "Invalid/oversized metadata accepted");
		}
		std::ofstream("test-save.dat", std::ios::binary).put(1);
		check(!read_text("test-save.dat"), "Truncated header accepted");
		check(!read_text("absent-save.dat"), "Missing save accepted");
		return 0;
	} catch (const std::exception &e) {
		std::cerr << e.what() << '\n';
		return 1;
	}
}
