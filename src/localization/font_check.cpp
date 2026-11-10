#include "actint/ttf_font.h"
#include "catalog.h"
#include "settings/text_encoding.h"
#include <SDL3/SDL_main.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <stdexcept>

int main(int argc, char **argv) {
	try {
		if (argc != 3 && argc != 5) {
			std::cerr << "Usage: vangers_font_check LOCALE_ROOT LOCALE [--text UTF8_FILE]\n";
			return 2;
		}
		vangers::localization::Catalog catalog(std::filesystem::u8path(argv[1]), argv[2]);
		std::string extra;
		if (argc == 5) {
			if (std::string(argv[3]) != "--text")
				throw std::invalid_argument("Expected --text UTF8_FILE");
			std::ifstream file(std::filesystem::u8path(argv[4]), std::ios::binary);
			if (!file)
				throw std::runtime_error("Cannot read UTF-8 input file");
			extra.assign(std::istreambuf_iterator<char>(file), {});
			if (file.bad() || !vangers::settings::is_valid_utf8(extra))
				throw std::runtime_error("Invalid UTF-8 input file");
		}
		bool missing = false;
		using vangers::actint::FontFamily;
		int role = 0;
		const char *roles[]{"Text", "Menu", "Display"};
		for (auto family : {FontFamily::Text, FontFamily::Menu, FontFamily::Display}) {
			vangers::actint::TtfFont font(family);
			const auto check = [&](const std::string &key, std::string_view text) {
				for (const auto cp : font.missing_glyphs(text)) {
					missing = true;
					std::cerr << argv[2] << ':' << key << " role=" << roles[role] << " missing U+"
							  << std::hex << std::uppercase << unsigned(cp) << std::dec << '\n';
				}
			};
			for (const auto &[key, value] : catalog.strings())
				check(key, value);
			if (argc == 5)
				check(argv[4], extra);
			++role;
		}
		if (!missing)
			std::cout << "All three font chains cover " << argv[2] << '\n';
		return missing ? 1 : 0;
	} catch (const std::exception &error) {
		std::cerr << error.what() << '\n';
		return 2;
	}
}
