#ifndef VANGERS_LOCALIZATION_CATALOG_H
#define VANGERS_LOCALIZATION_CATALOG_H
#include "description.h"

#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace vangers::localization {

// Catalog storage is immutable after loading; returned strings live with the catalog.
// Locale changes require restarting the interface, not replacing borrowed UI pointers.
class Catalog {
  public:
	Catalog(const std::filesystem::path &root, const std::string &id);
	const std::string &text(const std::string &key) const;
	const std::string *find_text(const std::string &key) const;
	const std::string &legacy_text(const std::string &key) const;
	const std::string &resource(const std::string &key) const;
	bool resource_is_utf8(const std::string &key) const;
	DescriptionStyle description_style(const std::string &key = {}) const {
		const auto found = description_styles_.find(key);
		return found == description_styles_.end() ? description_style_ : found->second;
	}
	bool legacy_russian() const {
		return russian_;
	}
	const std::vector<std::string> &diagnostics() const {
		return diagnostics_;
	}

  private:
	std::string id_;
	bool russian_ = false;
	DescriptionStyle description_style_ = DescriptionStyle::RegularTop;
	std::map<std::string, DescriptionStyle> description_styles_;
	std::map<std::string, std::string> strings_, resources_, encodings_;
	mutable std::map<std::string, std::string> legacy_;
	mutable std::set<std::string> reported_;
	mutable std::vector<std::string> diagnostics_;
	void report(const std::string &message) const;
};

bool valid_locale_id(std::string_view id);
void request_locale(const std::string &id);
const std::string &requested_locale();
const Catalog &catalog();
const char *text(const std::string &key);
const char *legacy_text(const std::string &key); // Explicit CP866 boundary for legacy consumers.
const char *resource(const std::string &key);
std::string resolve(std::string_view display_text); // @l10n:key or unchanged literal UTF-8.
// The item UI uses one integer or one escave name; never execute translator-provided printf.
std::string format_integer(std::string_view pattern, int value);
std::string format_text(std::string_view pattern, std::string_view value);

} // namespace vangers::localization
#endif
