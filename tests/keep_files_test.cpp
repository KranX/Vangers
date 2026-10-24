#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

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
	const std::string fostral = TerrainSnapshotName("thechain/fostral/output.vmt");
	const std::string glorx = TerrainSnapshotName("thechain/glorx/output.vmt");
	const std::string ark = TerrainSnapshotName("thechain/ark-a-znoy/output.vmt");
	if (!check(fostral != glorx, "different worlds share a snapshot file name") ||
		!check(fostral != ark, "different worlds share a snapshot file name") ||
		!check(glorx != ark, "different worlds share a snapshot file name") ||
		!check(
			TerrainSnapshotName("thechain/fostral/output.vmt") == fostral,
			"snapshot file name is not deterministic"
		) ||
		!check(
			fostral.find('/') == std::string::npos && fostral.find('\\') == std::string::npos,
			"snapshot file name contains a path separator"
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
