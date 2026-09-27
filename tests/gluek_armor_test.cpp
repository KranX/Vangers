#include <iostream>
#include <limits>

#include "units/gluek_armor.h"

int main() {
	struct TestCase {
		const char *name;
		int armor;
		int max_armor;
		int expected;
	};
	const int int_max = std::numeric_limits<int>::max();
	const TestCase cases[] = {
		{"empty", 0, 60 << 16, 10 << 16},
		{"ordinary healing", 30 << 16, 60 << 16, 40 << 16},
		{"exact fit", 50 << 16, 60 << 16, 60 << 16},
		{"partial healing", 55 << 16, 60 << 16, 60 << 16},
		{"fraction preserved", (50 << 16) - 1, 60 << 16, (60 << 16) - 1},
		{"fraction capped", (55 << 16) + 1, 60 << 16, 60 << 16},
		{"one fixed-point unit missing", (60 << 16) - 1, 60 << 16, 60 << 16},
		{"full", 60 << 16, 60 << 16, 60 << 16},
		{"previously overfilled", 160 << 16, 60 << 16, 60 << 16},
		{"small maximum", 0, 5 << 16, 5 << 16},
		{"zero maximum", 0, 0, 0},
		{"different vehicle", 95 << 16, 100 << 16, 100 << 16},
		{"large overfilled value", int_max, 60 << 16, 60 << 16},
		{"near integer limit", int_max - 1, int_max, int_max},
	};
	for (const auto &test : cases) {
		const int actual = vangers::items::restore_gluek_armor(test.armor, test.max_armor);
		if (actual != test.expected) {
			std::cerr << test.name << ": expected " << test.expected << ", got " << actual << '\n';
			return 1;
		}
	}

	// Repeated manual uses must not bank armor above either vehicle's maximum.
	const int maxima[] = {60 << 16, 100 << 16};
	for (const int max_armor : maxima) {
		int armor = max_armor - (5 << 16);
		for (int use = 0; use < 1000; ++use) {
			armor = vangers::items::restore_gluek_armor(armor, max_armor);
			if (armor != max_armor) {
				std::cerr << "repeated use " << use << " exceeded the armor limit\n";
				return 1;
			}
		}
	}
	return 0;
}
