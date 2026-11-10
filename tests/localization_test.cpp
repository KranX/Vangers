#include "localization/catalog.h"
#include "settings/text_encoding.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <stdexcept>
#include <toml.hpp>
#include <zlib.h>

namespace {
namespace fs = std::filesystem;
using namespace vangers::localization;
void check(bool ok, const char *message) {
	if (!ok)
		throw std::runtime_error(message);
}
template<class F> void rejects(F &&action, const char *message) {
	try {
		action();
	} catch (const std::exception &) {
		return;
	}
	throw std::runtime_error(message);
}
void write(const fs::path &path, const std::string &value) {
	std::ofstream f(path, std::ios::binary);
	f << value;
	check(f.good(), "Unable to write locale fixture");
}
void conservation(const fs::path &root, const char *id, unsigned long expected, int group = 0) {
	Catalog c(root, id);
	const auto parsed = toml::parse<toml::ordered_type_config>(root / id / "strings.toml");
	std::map<std::string, std::string> sorted;
	for (const auto &[key, value] : parsed.at("strings").as_table(std::nothrow)) {
		(void)value;
		const bool description = key.rfind("descriptions.", 0) == 0;
		const int field = key.rfind("items.", 0) == 0									 ? 1
						  : (key.rfind("world.", 0) == 0 || key.rfind("cycle.", 0) == 0) ? 2
						  : key.rfind("service.", 0) == 0								 ? 3
																						 : 0;
		if ((description ? 4 : field) != group)
			continue;
		sorted.emplace(key, c.text(key));
	}
	const std::size_t sizes[]{394, 219, 19, 2, 92};
	check(sorted.size() == sizes[group], "Incomplete stock UI catalog");
	auto checksum = crc32(0, nullptr, 0);
	for (const auto &[key, value] : sorted) {
		checksum = crc32(checksum, (const Bytef *)key.c_str(), uInt(key.size() + 1));
		checksum = crc32(checksum, (const Bytef *)value.c_str(), uInt(value.size() + 1));
	}
	check(checksum == expected, "Existing UI wording changed during catalog extraction");
	check(c.diagnostics().empty(), "Stock locale silently falls back");
}
} // namespace
int main(int argc, char **argv) {
	fs::path temporary;
	try {
		if (argc == 4 && std::string(argv[1]) == "--installed") {
			request_locale(argv[2]);
			// Read UTF-8 expectations as bytes, not through Windows' narrow argv
			// conversion, which depends on the host's active code page.
			std::ifstream expected_file(argv[3], std::ios::binary);
			check(expected_file.is_open(), "Cannot open expected catalog text fixture");
			const std::string expected{std::istreambuf_iterator<char>(expected_file), {}};
			check(!expected_file.bad(), "Cannot read expected catalog text fixture");
			check(catalog().text("menu.iSTR_NEW_GAME") == expected, "Installed catalog not loaded");
			return 0;
		}
		check(argc == 2, "Expected stock package directory");
		const fs::path root(argv[1]);
		// Frozen from the five original macro tables at integration commit 8af1017.
		conservation(root, "en", 0xa4f67129);
		conservation(root, "ru", 0x03c56c3b);
		conservation(root, "en", 0x70547599, 1);
		conservation(root, "ru", 0x59e0c209, 1);
		conservation(root, "en", 0xaae6d5ca, 2);
		conservation(root, "ru", 0xc509d86e, 2);
		// Separate audit: keep checksums of the original UI macros unchanged.
		conservation(root, "en", 0xfbce565c, 4);
		conservation(root, "ru", 0xc9799444, 4);
		check(
			format_integer("Ресурс: %d %%", -17) == "Ресурс: -17 %",
			"Typed integer formatting corrupted text"
		);
		check(
			format_text("из %s", "Японский мир 😀") == "из Японский мир 😀",
			"Typed string formatting lost Unicode"
		);
		check(
			format_integer(std::string(20000, 'x') + "%d", 42).size() == 20002,
			"Long translation was truncated"
		);
		for (const auto *pattern : {"%n", "%s", "%d %d", "%0200000d", "%"})
			rejects([&] { format_integer(pattern, 1); }, "Unsafe template accepted");
		Catalog ru(root, "ru"), en(root, "en");
		check(
			ru.description_style() == DescriptionStyle::CompactCentered &&
				en.description_style() == DescriptionStyle::RegularTop,
			"Description presentation lost package metadata"
		);
		check(
			en.description_style("descriptions.item.33") == DescriptionStyle::RegularCentered,
			"Authored centered block lost its per-description override"
		);
		check(ru.legacy_russian() && !en.legacy_russian(), "Wrong legacy-resource adapter");
		check(ru.text("actint.aciSTR_PRICE") == "Цена: ", "Russian UTF-8 catalog corrupted");
		const auto *stable = ru.text("actint.aciSTR_PRICE").c_str();
		check(
			ru.legacy_text("actint.aciSTR_PRICE") == vangers::settings::utf8_to_cp866("Цена: "),
			"Wrong output-boundary encoding"
		);
		check(
			stable == ru.text("actint.aciSTR_PRICE").c_str(),
			"Lookup invalidated borrowed UI storage"
		);
		check(!ru.resource_is_utf8("history.prelude"), "Legacy history lost its explicit encoding");
		check(ru.resource("diagen") == "diagen.text", "Russian archive changed");
		check(en.resource("diagen") == "diagen_eng.text", "English archive changed");
		for (const auto *id : {"", "../ru", "ru/en", "ru\\en", "EN", "ru:", "-ru", "ru-"})
			rejects([&] { Catalog bad(root, id); }, "Unsafe locale ID accepted");
		rejects([&] { ru.text("not.a.key"); }, "Unknown source key silently accepted");
		rejects([&] { ru.resource("not.a.resource"); }, "Unknown resource key silently accepted");
		temporary =
			fs::temp_directory_path() /
			("vangers-locales-" +
				std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
		fs::create_directories(temporary / "en");
		fs::copy_file(root / "en" / "manifest.toml", temporary / "en" / "manifest.toml");
		fs::copy_file(root / "en" / "strings.toml", temporary / "en" / "strings.toml");
		fs::create_directories(temporary / "zz-test");
		const auto manifest = temporary / "zz-test" / "manifest.toml";
		const auto strings = temporary / "zz-test" / "strings.toml";
		write(
			manifest,
			"[resources]\ndiagen = 'locales/zz-test/dialogue.text'\n\"history.prelude\" = "
			"'locales/zz-test/prelude.txt'\n"
		);
		write(
			strings, "[strings]\n\"menu.iSTR_NEW_GAME\" = '日本 😀 A very long translated label'\n"
		);
		Catalog custom(temporary, "zz-test");
		check(
			custom.text("menu.iSTR_NEW_GAME").find("日本 😀") == 0,
			"New locale was reduced to the RU/EN alphabet"
		);
		check(
			custom.text("actint.aciSTR_PRICE") == en.text("actint.aciSTR_PRICE"),
			"Missing key failed to use English"
		);
		check(
			custom.text("items.ITM41_COMMENTS") == en.text("items.ITM41_COMMENTS"),
			"Missing item comment did not fall back as a complete semantic unit"
		);
		check(
			custom.text("descriptions.item.27") == en.text("descriptions.item.27") &&
				custom.description_style() == en.description_style(),
			"Third-locale description did not inherit English text/presentation"
		);
		check(
			custom.resource("iscreen.binary") == en.resource("iscreen.binary"),
			"Missing resource failed to use English"
		);
		check(custom.resource_is_utf8("history.prelude"), "New text did not default to UTF-8");
		check(
			!custom.resource_is_utf8("history.genesis"), "Fallback text lost the fallback encoding"
		);
		const auto count = custom.diagnostics().size();
		for (int i = 0; i < 100; ++i)
			(void)custom.text("actint.aciSTR_PRICE");
		check(
			count > 0 && custom.diagnostics().size() == count,
			"Fallback reports spam on every frame"
		);
		Catalog missing(temporary, "missing");
		check(
			missing.text("menu.iSTR_NEW_GAME") == en.text("menu.iSTR_NEW_GAME"),
			"Missing package did not fall back"
		);
		write(strings, "[strings]\n\"items.ITM01_PARAMS\" = '%s'\n");
		rejects(
			[&] { Catalog c(temporary, "zz-test"); },
			"Translation changed the template argument type"
		);
		write(strings, "[strings]\n\"not.a.key\" = 'typo'\n");
		rejects([&] { Catalog c(temporary, "zz-test"); }, "Translation typo accepted");
		write(strings, "[strings]\n\"menu.iSTR_NEW_GAME\" = 42\n");
		rejects([&] { Catalog c(temporary, "zz-test"); }, "Non-string translation accepted");
		write(strings, "[strings]\n\"menu.iSTR_NEW_GAME\" = \"\\u0000\"\n");
		rejects([&] { Catalog c(temporary, "zz-test"); }, "Embedded NUL accepted");
		write(strings, "[strings]\n\"\" = 'empty key'\n");
		rejects([&] { Catalog c(temporary, "zz-test"); }, "Empty stable key accepted");
		write(
			strings, "[strings]\n\"menu.iSTR_NEW_GAME\" = \"" + std::string(1, char(0xff)) + "\"\n"
		);
		rejects([&] { Catalog c(temporary, "zz-test"); }, "Invalid UTF-8 accepted");
		write(strings, "[strings]\n");
		for (const auto *path : {"../outside",
				 "/outside",
				 "/",
				 "//server/share",
				 "C:/outside",
				 "C:relative",
				 "folder\\file"}) {
			write(manifest, std::string("[resources]\ndiagen = '") + path + "'\n");
			rejects([&] { Catalog c(temporary, "zz-test"); }, "Unsafe resource path accepted");
		}
		write(strings, "[strings]\n");
		write(manifest, "description_style = 'compact-centered'\n[resources]\n");
		check(
			Catalog(temporary, "zz-test").description_style() == DescriptionStyle::CompactCentered,
			"Non-RU package cannot select centered presentation"
		);
		write(manifest, "description_style = 'shrink-and-scroll'\n[resources]\n");
		rejects([&] { Catalog c(temporary, "zz-test"); }, "Invalid description style accepted");
		write(manifest, "[resources]\n[description_styles]\n'descriptions.typo' = 'regular-top'\n");
		rejects([&] { Catalog c(temporary, "zz-test"); }, "Unknown description style key accepted");
		request_locale("ru");
		check(resolve("@l10n:actint.aciSTR_PRICE") == "Цена: ", "Keyed script text not resolved");
		check(
			resolve("literal 日本 @l10n:unchanged") == "literal 日本 @l10n:unchanged",
			"Literal text was rewritten"
		);
		fs::remove_all(temporary);
		std::cout
			<< "Locale catalogs, wording conservation, Unicode, resources and fallback passed\n";
		return 0;
	} catch (const std::exception &error) {
		if (!temporary.empty())
			fs::remove_all(temporary);
		std::cerr << error.what() << '\n';
		return 1;
	}
}
