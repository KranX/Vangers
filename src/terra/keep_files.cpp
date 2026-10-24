#include "keep_files.h"

#include <filesystem>
#include <map>
#include <system_error>
#include <vector>

namespace {

namespace fs = std::filesystem;

bool RenamePath(const std::string &from, const std::string &to, std::error_code &ec) {
	ec.clear();
	fs::rename(from, to, ec);
	return !ec;
}

} // namespace

namespace {

std::string BaseSnapshotName(const std::string &worldKeepPath) {
	std::filesystem::path path(worldKeepPath);
	std::string base = path.parent_path().filename().string();
	if (base.empty())
		base = path.stem().string();

	std::string safe;
	safe.reserve(base.size());
	for (unsigned char c : base) {
		const bool keep = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
						  (c >= '0' && c <= '9') || c == '-' || c == '_' || c == ' ';
		safe.push_back(keep ? static_cast<char>(c) : '_');
	}
	if (safe.empty())
		safe = "world";
	return safe;
}

std::string PathHash(const std::string &value) {
	static const char hex[] = "0123456789abcdef";
	unsigned int hash = 2166136261u;
	for (unsigned char c : value) {
		hash ^= c;
		hash *= 16777619u;
	}

	std::string result;
	result.reserve(8);
	for (int shift = 28; shift >= 0; shift -= 4)
		result.push_back(hex[(hash >> shift) & 0x0F]);
	return result;
}

} // namespace

std::string TerrainDirForSave(const std::string &savePath) {
	fs::path path(savePath);
	path.replace_extension(".terrain");
	return path.string();
}

std::vector<std::string> TerrainSnapshotNames(const std::vector<std::string> &worldKeepPaths) {
	std::vector<std::string> bases;
	std::map<std::string, int> counts;
	bases.reserve(worldKeepPaths.size());
	for (const std::string &path : worldKeepPaths) {
		bases.push_back(BaseSnapshotName(path));
		++counts[bases.back()];
	}

	// A directory name alone is readable but not guaranteed unique. If two
	// worlds would collide, both get a stable path hash instead of relying on
	// the order of the world list.
	std::vector<std::string> names;
	names.reserve(worldKeepPaths.size());
	for (size_t i = 0; i < worldKeepPaths.size(); ++i) {
		std::string name = bases[i];
		if (counts[bases[i]] > 1)
			name += "_" + PathHash(worldKeepPaths[i]);
		name += ".vmt";
		names.push_back(std::move(name));
	}
	return names;
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
