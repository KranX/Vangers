#include "settings/text_encoding.h"

#include <cstdint>
#include <iostream>
#include <string>

namespace {

using namespace vangers::settings;

bool check(bool condition, const std::string &message) {
	if (!condition)
		std::cerr << "FAIL: " << message << '\n';
	return condition;
}

bool test_russian_round_trip() {
	const std::string utf8 = "Вангер Ёж";
	const std::string cp866 = utf8_to_cp866(utf8);
	return check(cp866_to_utf8(cp866) == utf8, "Russian text did not round-trip") &&
		   check(is_valid_utf8(utf8), "valid Russian UTF-8 was rejected");
}

bool test_complete_cp866_round_trip() {
	std::string original;
	for (int value = 0x20; value <= 0xff; ++value)
		original.push_back(static_cast<char>(value));
	return check(
		utf8_to_cp866(cp866_to_utf8(original)) == original,
		"printable CP866 repertoire did not round-trip"
	);
}

bool test_invalid_and_unrepresentable_input() {
	const std::string invalid = std::string("A") + static_cast<char>(0xc0) + "B";
	const std::string emoji = "A😀B";
	return check(!is_valid_utf8(invalid), "invalid UTF-8 was accepted") &&
		   check(utf8_to_cp866(invalid) == "A?B", "invalid UTF-8 replacement is inconsistent") &&
		   check(utf8_to_cp866(emoji) == "A?B", "unrepresentable character was not replaced");
}

bool test_box_drawing_and_special_characters() {
	const std::string bytes = {
		static_cast<char>(0xb3),
		static_cast<char>(0xdb),
		static_cast<char>(0xfc),
		static_cast<char>(0xff),
	};
	return check(
		cp866_to_utf8(bytes) == "│█№ ", "CP866 special characters were converted incorrectly"
	);
}

} // namespace

int main() {
	if (!check(
			cp1251_to_utf8("\xc0\xff\xa8\xb8\xb9\x97") == "АяЁё№—", "CP1251 repertoire mismatch"
		) ||
		!check(
			cp1251_to_utf8(std::string("A\0\xb8", 3)) == std::string("A\0ё", 4),
			"CP1251 embedded separators lost"
		) ||
		!check(cp1251_to_utf8("\x98") == "�", "Undefined CP1251 byte mismatch"))
		return 1;
	return test_russian_round_trip() && test_complete_cp866_round_trip() &&
				   test_invalid_and_unrepresentable_input() &&
				   test_box_drawing_and_special_characters()
			   ? 0
			   : 1;
}
