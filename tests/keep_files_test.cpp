#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "terra/keep_files.h"

namespace {

namespace fs = std::filesystem;

bool check(bool condition, const char *message) {
	if (!condition)
		std::cerr << message << '\n';
	return condition;
}

void writeFile(const fs::path &path, const std::string &value) {
	fs::create_directories(path.parent_path());
	std::ofstream out(path, std::ios::binary | std::ios::trunc);
	out.write(value.data(), value.size());
}

std::string readFile(const fs::path &path) {
	std::ifstream in(path, std::ios::binary);
	return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

} // namespace

int main() {
	const std::vector<std::string> worlds = {
		"thechain/fostral/output.vmt",
		"thechain/glorx/output.vmt",
		"thechain/ark-a-znoy/output.vmt",
	};
	const std::vector<std::string> names = TerrainSnapshotNames(worlds);
	if (!check(names.size() == worlds.size(), "snapshot name count mismatch") ||
		!check(names[0] == "fostral.vmt", "fostral snapshot name is not readable") ||
		!check(names[1] == "glorx.vmt", "glorx snapshot name is not readable") ||
		!check(names[2] == "ark-a-znoy.vmt", "ark-a-znoy snapshot name is not readable") ||
		!check(
			names[0] != names[1] && names[1] != names[2] && names[0] != names[2],
			"different worlds share a snapshot file name"
		) ||
		!check(TerrainSnapshotNames(worlds) == names, "snapshot names are not deterministic") ||
		!check(
			names[0].find('/') == std::string::npos && names[0].find('\\') == std::string::npos,
			"snapshot file name contains a path separator"
		))
		return 1;

	// Same-named world directories must still get distinct, order-independent
	// names; otherwise terrain from one world can be restored into another.
	const std::vector<std::string> colliding = {
		"chain-a/same/output.vmt",
		"chain-b/same/output.vmt",
	};
	const std::vector<std::string> collidingNames = TerrainSnapshotNames(colliding);
	const std::vector<std::string> reversedNames =
		TerrainSnapshotNames({colliding[1], colliding[0]});
	if (!check(collidingNames.size() == 2, "collision snapshot name count mismatch") ||
		!check(collidingNames[0] != collidingNames[1], "colliding worlds share a name") ||
		!check(
			collidingNames[0] == reversedNames[1] && collidingNames[1] == reversedNames[0],
			"collision names depend on world order"
		))
		return 1;

	const fs::path root = fs::temp_directory_path() / "vangers_keep_files_test";
	std::error_code ec;
	fs::remove_all(root, ec);
	fs::create_directories(root, ec);

	const fs::path savePath = root / "save03.dat";
	const fs::path newSavePath = root / "save03.dat.new";
	const fs::path terrainDir = TerrainDirForSave(savePath.string());
	const fs::path newTerrainDir = terrainDir.string() + ".new";
	const fs::path terrainFile = terrainDir / "fostral.vmt";

	// Successful generation replaces both the save file and the terrain folder.
	writeFile(savePath, "old save");
	writeFile(newSavePath, "new save");
	writeFile(terrainFile, "old terrain");
	writeFile(newTerrainDir / "fostral.vmt", "new terrain");
	std::string error;
	if (!check(
			PublishSaveGeneration(savePath.string(), newSavePath.string(), true, &error),
			"publish failed"
		) ||
		!check(readFile(savePath) == "new save", "save file was not replaced") ||
		!check(readFile(terrainFile) == "new terrain", "terrain snapshot was not replaced") ||
		!check(!fs::exists(newSavePath, ec), "temporary save file was left behind") ||
		!check(!fs::exists(newTerrainDir, ec), "temporary terrain folder was left behind") ||
		!check(!fs::exists(savePath.string() + ".old", ec), "save backup was left behind") ||
		!check(!fs::exists(terrainDir.string() + ".old", ec), "terrain backup was left behind"))
		return 1;

	// A failed publication keeps the previous generation intact.
	writeFile(savePath, "old save");
	writeFile(terrainFile, "old terrain");
	writeFile(newTerrainDir / "fostral.vmt", "new terrain");
	error.clear();
	if (!check(
			!PublishSaveGeneration(savePath.string(), newSavePath.string(), true, &error),
			"publish unexpectedly succeeded"
		) ||
		!check(!error.empty(), "publish failure was not reported") ||
		!check(readFile(savePath) == "old save", "save file was damaged by a failed publish") ||
		!check(readFile(terrainFile) == "old terrain", "terrain was damaged by a failed publish") ||
		!check(!fs::exists(newTerrainDir, ec), "temporary terrain folder was left behind"))
		return 1;

	// Saving without the per-save mode removes a stale terrain snapshot.
	writeFile(terrainFile, "stale terrain");
	writeFile(newSavePath, "new save");
	error.clear();
	if (!check(
			PublishSaveGeneration(savePath.string(), newSavePath.string(), false, &error),
			"plain publish failed"
		) ||
		!check(readFile(savePath) == "new save", "save file was not replaced") ||
		!check(!fs::exists(terrainDir, ec), "stale terrain snapshot was not removed"))
		return 1;

	fs::remove_all(root, ec);
	return 0;
}
