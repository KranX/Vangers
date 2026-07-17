#ifndef VANGERS_SETTINGS_LEGACY_SETTINGS_IMPORT_H
#define VANGERS_SETTINGS_LEGACY_SETTINGS_IMPORT_H

#include "settings.h"

#include <filesystem>
#include <string>

namespace vangers::settings {

struct LegacyImportResult {
	bool options_imported = false;
	bool controls_imported = false;
	std::string diagnostic;

	bool any_imported() const {
		return options_imported || controls_imported;
	}
};

bool import_legacy_options(
	const std::filesystem::path &path,
	GameSettings &settings,
	std::string *diagnostic = nullptr
);
bool import_legacy_controls(
	const std::filesystem::path &path,
	GameSettings &settings,
	std::string *diagnostic = nullptr
);
LegacyImportResult import_legacy_settings(const SettingsPaths &paths, GameSettings &settings);

} // namespace vangers::settings

#endif
