#include "catalog.h"

#include "settings/text_encoding.h"

#include <SDL3/SDL_filesystem.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <toml.hpp>

namespace vangers::localization {
namespace {
using Table = std::map<std::string, std::string>;
std::string signature(std::string_view pattern) {
	std::string result;
	for (std::size_t i = 0; i < pattern.size(); ++i) {
		if (pattern[i] != '%')
			continue;
		if (++i == pattern.size() || (pattern[i] != '%' && pattern[i] != 'd' && pattern[i] != 's'))
			throw std::invalid_argument("Item template supports only %d, %s and %%");
		if (pattern[i] != '%')
			result += pattern[i];
	}
	return result;
}
std::string format(std::string_view pattern, char kind, std::string_view value) {
	const auto args = signature(pattern);
	if (!args.empty() && args != std::string(1, kind))
		throw std::invalid_argument("Item template has an incompatible argument");
	std::string result;
	for (std::size_t i = 0; i < pattern.size(); ++i) {
		if (pattern[i] != '%')
			result += pattern[i];
		else if (pattern[++i] == '%')
			result += '%';
		else
			result += value;
	}
	return result;
}
toml::ordered_value read(const std::filesystem::path &path) {
	auto result = toml::try_parse<toml::ordered_type_config>(path);
	if (result.is_err())
		throw std::runtime_error(
			"Invalid locale file: " + path.string() + ": " +
			toml::format_error(result.unwrap_err().front())
		);
	return std::move(result.unwrap());
}
Table table(const toml::ordered_value &document, const std::string &name) {
	Table out;
	if (!document.contains(name) || !document.at(name).is_table())
		throw std::runtime_error("Locale is missing table: " + name);
	for (const auto &[key, value] : document.at(name).as_table(std::nothrow)) {
		if (key.empty() || key.size() > 128)
			throw std::runtime_error("Invalid locale key: " + key);
		for (unsigned char c : key)
			if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
					c == '.' || c == '_' || c == '-'))
				throw std::runtime_error("Invalid locale key: " + key);
		if (!value.is_string())
			throw std::runtime_error("Locale value is not a string: " + key);
		const auto &s = value.as_string(std::nothrow);
		if (s.find('\0') != std::string::npos || !settings::is_valid_utf8(s))
			throw std::runtime_error("Invalid UTF-8 locale string: " + key);
		out.emplace(key, s);
	}
	return out;
}
void paths(const Table &values) {
	for (const auto &[key, value] : values) {
		const std::filesystem::path path(value);
		// Windows /file is rooted on the current drive, but is_absolute() is
		// false because it has no drive name. Resources must have no root at all.
		if (value.empty() || path.has_root_path() || value.find('\\') != std::string::npos ||
			value.find(':') != std::string::npos)
			throw std::runtime_error("Invalid locale resource path: " + key);
		for (const auto &part : path)
			if (part == "..")
				throw std::runtime_error("Locale resource escapes data directory: " + key);
	}
}
std::string requested;
std::unique_ptr<Catalog> active;
} // namespace

bool valid_locale_id(std::string_view id) {
	if (id.empty() || id.size() > 48 || id.front() == '-' || id.back() == '-')
		return false;
	for (unsigned char c : id)
		if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-'))
			return false;
	return true;
}

Catalog::Catalog(const std::filesystem::path &root, const std::string &id): id_(id) {
	if (!valid_locale_id(id))
		throw std::invalid_argument("Invalid locale ID: " + id);
	const auto english = read(root / "en" / "manifest.toml");
	resources_ = table(english, "resources");
	paths(resources_);
	strings_ = table(read(root / "en" / "strings.toml"), "strings");
	encodings_ = table(english, "encodings");
	for (const auto &[key, value] : encodings_)
		if (!resources_.count(key) || (value != "utf8" && value != "cp866"))
			throw std::runtime_error("Invalid locale resource encoding: " + key);
	if (id == "en")
		return;
	const auto manifest = root / id / "manifest.toml";
	if (!std::filesystem::exists(manifest)) {
		report("missing package; using en");
		return;
	}
	const auto local = read(manifest);
	if (local.contains("legacy_language")) {
		const auto &language = local.at("legacy_language").as_string();
		if (language != "en" && language != "ru")
			throw std::runtime_error("Invalid legacy_language in " + manifest.string());
		russian_ = language == "ru";
	}
	const auto translated = table(read(root / id / "strings.toml"), "strings");
	for (const auto &[key, value] : strings_) {
		(void)value;
		if (!translated.count(key))
			report("missing string " + key + "; using en");
	}
	for (const auto &[key, value] : translated) {
		if (!strings_.count(key))
			throw std::runtime_error("Unknown locale string key: " + key);
		const auto &english_text = strings_.at(key);
		if ((english_text.find("%d") != std::string::npos ||
				english_text.find("%s") != std::string::npos) &&
			signature(english_text) != signature(value))
			throw std::runtime_error("Locale template argument changed: " + key);
		strings_[key] = value;
	}
	const auto assets = table(local, "resources");
	paths(assets);
	const auto encodings = local.contains("encodings") ? table(local, "encodings") : Table{};
	for (const auto &[key, value] : encodings)
		if (!assets.count(key) || (value != "utf8" && value != "cp866"))
			throw std::runtime_error("Invalid locale resource encoding: " + key);
	for (const auto &[key, value] : assets) {
		if (!resources_.count(key))
			throw std::runtime_error("Unknown locale resource key: " + key);
		resources_[key] = value;
		encodings_[key] = encodings.count(key) ? encodings.at(key) : "utf8";
	}
	for (const auto &[key, value] : table(english, "resources")) {
		(void)value;
		if (!assets.count(key))
			report("missing resource " + key + "; using en");
	}
}

void Catalog::report(const std::string &message) const {
	if (reported_.insert(message).second) {
		diagnostics_.push_back("locale " + id_ + ": " + message);
		std::cerr << "Localization: " << diagnostics_.back() << '\n';
	}
}
const std::string &Catalog::text(const std::string &key) const {
	const auto it = strings_.find(key);
	if (it == strings_.end())
		throw std::runtime_error("Unknown locale string key: " + key);
	return it->second;
}
const std::string *Catalog::find_text(const std::string &key) const {
	const auto it = strings_.find(key);
	return it == strings_.end() ? nullptr : &it->second;
}
const std::string &Catalog::legacy_text(const std::string &key) const {
	if (const auto it = legacy_.find(key); it != legacy_.end())
		return it->second;
	return legacy_.emplace(key, settings::utf8_to_cp866(text(key))).first->second;
}
const std::string &Catalog::resource(const std::string &key) const {
	const auto it = resources_.find(key);
	if (it == resources_.end())
		throw std::runtime_error("Unknown locale resource key: " + key);
	return it->second;
}
bool Catalog::resource_is_utf8(const std::string &key) const {
	(void)resource(key);
	const auto it = encodings_.find(key);
	return it == encodings_.end() || it->second == "utf8";
}
void request_locale(const std::string &id) {
	if (!valid_locale_id(id))
		throw std::invalid_argument("Invalid locale ID: " + id);
	if (requested != id) {
		active.reset();
		requested = id;
	}
}
const std::string &requested_locale() {
	return requested;
}
const Catalog &catalog() {
	if (!active) {
		const auto *root = std::getenv("VANGERS_LOCALES");
		std::filesystem::path directory = root ? std::filesystem::u8path(root) : "locales";
		// Purchased data need not contain code-owned catalogs. Runtime packages
		// install them at SDL's application base (Resources in a macOS bundle).
		// An explicit override or a local package must not hide malformed data.
		if (!root && !std::filesystem::exists(directory)) {
			if (const char *base = SDL_GetBasePath())
				directory = std::filesystem::u8path(base) / "locales";
		}
		active = std::make_unique<Catalog>(directory, requested.empty() ? "en" : requested);
	}
	return *active;
}
const char *text(const std::string &key) {
	return catalog().text(key).c_str();
}
const char *legacy_text(const std::string &key) {
	return catalog().legacy_text(key).c_str();
}
const char *resource(const std::string &key) {
	return catalog().resource(key).c_str();
}
std::string resolve(std::string_view display_text) {
	constexpr std::string_view prefix = "@l10n:";
	return display_text.substr(0, prefix.size()) == prefix
			   ? text(std::string(display_text.substr(prefix.size())))
			   : std::string(display_text);
}
std::string format_integer(std::string_view pattern, int value) {
	return format(pattern, 'd', std::to_string(value));
}
std::string format_text(std::string_view pattern, std::string_view value) {
	return format(pattern, 's', value);
}
} // namespace vangers::localization
