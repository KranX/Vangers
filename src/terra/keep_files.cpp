#include "keep_files.h"

#include <filesystem>
#include <system_error>

namespace {

namespace fs = std::filesystem;

bool RenamePath(const std::string &from, const std::string &to, std::error_code &ec) {
	ec.clear();
	fs::rename(from, to, ec);
	return !ec;
}

} // namespace

std::string TerrainDirForSave(const std::string &savePath) {
	fs::path path(savePath);
	path.replace_extension(".terrain");
	return path.string();
}

bool PublishSaveGeneration(
	const std::string &savePath,
	const std::string &newSavePath,
	bool writeTerrain,
	std::string *error
) {
	const std::string terrainDir = TerrainDirForSave(savePath);
	const std::string terrainNew = terrainDir + ".new";
	const std::string terrainOld = terrainDir + ".old";
	const std::string saveOld = savePath + ".old";
	std::error_code ec;

	fs::remove_all(terrainOld, ec);
	fs::remove(saveOld, ec);
	ec.clear();

	const bool hadSave = fs::exists(savePath, ec);
	const bool hadTerrain = fs::exists(terrainDir, ec);

	if (hadSave && !RenamePath(savePath, saveOld, ec)) {
		*error = "can't back up the previous save";
		return false;
	}

	if (writeTerrain && hadTerrain && !RenamePath(terrainDir, terrainOld, ec)) {
		if (hadSave)
			RenamePath(saveOld, savePath, ec);
		*error = "can't back up the previous terrain snapshot";
		return false;
	}

	if (writeTerrain && !RenamePath(terrainNew, terrainDir, ec)) {
		if (hadTerrain)
			RenamePath(terrainOld, terrainDir, ec);
		if (hadSave)
			RenamePath(saveOld, savePath, ec);
		*error = "can't publish the terrain snapshot";
		return false;
	}

	if (!RenamePath(newSavePath, savePath, ec)) {
		std::error_code rollbackError;
		if (writeTerrain) {
			fs::remove_all(terrainDir, rollbackError);
			if (hadTerrain)
				RenamePath(terrainOld, terrainDir, rollbackError);
		}
		if (hadSave)
			RenamePath(saveOld, savePath, rollbackError);
		*error = "can't publish the save file";
		return false;
	}

	if (!writeTerrain)
		fs::remove_all(terrainDir, ec);

	fs::remove_all(terrainOld, ec);
	fs::remove(saveOld, ec);
	return true;
}
