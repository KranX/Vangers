#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "terra/terrain_store.h"

namespace {

namespace fs = std::filesystem;

bool check(bool condition, const char *message) {
	if (!condition)
		std::cerr << message << '\n';
	return condition;
}

std::vector<unsigned char> makeRow(unsigned char h2Size, unsigned char value) {
	return std::vector<unsigned char>(h2Size, value);
}

fs::path makeTestDir() {
	fs::path dir = fs::temp_directory_path() / "vangers_terrain_store_test";
	std::error_code ec;
	fs::remove_all(dir, ec);
	fs::create_directories(dir, ec);
	return dir;
}

void writeGarbage(const fs::path &path) {
	std::ofstream out(path, std::ios::binary | std::ios::trunc);
	out.write("not a terrain file", 19);
}

} // namespace

int main() {
	const fs::path root = makeTestDir();
	const fs::path saveDir = root / "save03.terrain";

	if (!check(
			terrainStoreDirForSave("savegame/save03.dat") == "savegame/save03.terrain",
			"unexpected sidecar directory name for a save slot"
		))
		return 1;

	// A save without surface changes must not leave a stale sidecar directory.
	{
		fs::create_directories(saveDir);
		writeGarbage(saveDir / "world_1.trn");
		TerrainStore empty;
		if (!check(empty.saveDir(saveDir.string()), "empty store failed to clean the directory") ||
			!check(!fs::exists(saveDir), "empty store did not remove the sidecar directory"))
			return 1;
	}

	// Round-trip of two worlds and several rows.
	const uint32_t h2Size = 8;
	const uint32_t vSize = 16;
	const std::vector<unsigned char> firstRow = makeRow(h2Size, 0x11);
	const std::vector<unsigned char> secondRow = makeRow(h2Size, 0x22);
	const std::vector<unsigned char> otherWorldRow = makeRow(h2Size, 0x33);
	{
		TerrainStore store;
		store.setRow(3, "Fostral", h2Size, vSize, 5, firstRow.data());
		store.setRow(3, "Fostral", h2Size, vSize, 9, secondRow.data());
		store.setRow(1, "Glorx", h2Size, vSize, 2, otherWorldRow.data());
		if (!check(!store.empty(), "store with rows reported empty") ||
			!check(store.rowCount() == 3, "unexpected row count") ||
			!check(store.saveDir(saveDir.string()), "failed to save terrain dir") ||
			!check(fs::exists(saveDir / "world_3.trn"), "missing world file") ||
			!check(fs::exists(saveDir / "world_1.trn"), "missing second world file"))
			return 1;
	}
	{
		TerrainStore store;
		if (!check(store.loadDir(saveDir.string()), "failed to load terrain dir") ||
			!check(store.rowCount() == 3, "loaded unexpected row count"))
			return 1;

		const std::vector<unsigned char> *row = store.getRow(3, "Fostral", h2Size, vSize, 5);
		if (!check(row != nullptr && *row == firstRow, "row 5 was not restored") ||
			!check(store.getRow(3, "Fostral", h2Size, vSize, 9) != nullptr, "row 9 lost") ||
			!check(store.getRow(1, "Glorx", h2Size, vSize, 2) != nullptr, "other world lost") ||
			!check(
				store.getRow(3, "Fostral", h2Size, vSize, 6) == nullptr,
				"unexpected row restored"
			) ||
			!check(
				store.getRow(3, "Fostral", h2Size, vSize + 1, 5) == nullptr,
				"layout mismatch was not rejected"
			) ||
			!check(
				store.getRow(3, "Necross", h2Size, vSize, 5) == nullptr,
				"world name mismatch was not rejected"
			))
			return 1;
	}

	// Stale world files from a previous state of the slot are removed.
	{
		std::error_code ec;
		fs::copy_file(saveDir / "world_3.trn", saveDir / "world_7.trn", ec);
		TerrainStore store;
		if (!check(store.loadDir(saveDir.string()), "failed to reload terrain dir") ||
			!check(store.saveDir(saveDir.string()), "failed to rewrite terrain dir") ||
			!check(!fs::exists(saveDir / "world_7.trn"), "stale world file was not removed"))
			return 1;
	}

	// Corrupted and truncated files are ignored without breaking the load.
	{
		fs::create_directories(root / "corrupt.terrain");
		writeGarbage(root / "corrupt.terrain" / "world_0.trn");
		writeGarbage(root / "corrupt.terrain" / "world_1.trn");
		TerrainStore store;
		if (!check(store.loadDir((root / "corrupt.terrain").string()), "corrupt dir failed to load") ||
			!check(store.empty(), "corrupt data produced rows"))
			return 1;
	}

	// A missing sidecar file is a normal situation for old saves.
	{
		TerrainStore store;
		if (!check(
				store.loadDir((root / "missing.terrain").string()),
				"missing directory should load as empty"
			) ||
			!check(store.empty(), "missing directory produced rows"))
			return 1;
	}

	std::error_code ec;
	fs::remove_all(root, ec);
	return 0;
}
