#include "terrain_store.h"

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <system_error>
#include <utility>

namespace {

namespace fs = std::filesystem;

const uint32_t WORLD_FILE_MAGIC = 0x31575456; // "VTW1"
const uint32_t WORLD_FILE_VERSION = 1;
const uint32_t MAX_NAME_LENGTH = 255;
const uint32_t MAX_H2_SIZE = 1u << 20;
const uint32_t MAX_V_SIZE = 1u << 21;
const char *WORLD_FILE_EXTENSION = ".trn";

void writeU32(std::ostream &out, uint32_t value) {
	unsigned char bytes[4] = {
		(unsigned char)(value & 0xFF),
		(unsigned char)((value >> 8) & 0xFF),
		(unsigned char)((value >> 16) & 0xFF),
		(unsigned char)((value >> 24) & 0xFF),
	};
	out.write((const char *)bytes, sizeof(bytes));
}

bool readU32(std::istream &in, uint32_t &value) {
	unsigned char bytes[4];
	if (!in.read((char *)bytes, sizeof(bytes)))
		return false;
	value = (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) | ((uint32_t)bytes[2] << 16) |
			((uint32_t)bytes[3] << 24);
	return true;
}

bool validLayout(uint32_t h2Size, uint32_t vSize) {
	return h2Size > 0 && h2Size <= MAX_H2_SIZE && vSize > 0 && vSize <= MAX_V_SIZE;
}

std::string worldFileName(int worldIndex) {
	char name[64];
	std::snprintf(name, sizeof(name), "world_%d%s", worldIndex, WORLD_FILE_EXTENSION);
	return name;
}

bool writeWorldFile(const fs::path &path, int worldIndex, const TerrainWorldRows &world) {
	std::ofstream out(path, std::ios::binary | std::ios::trunc);
	if (!out) {
		std::cerr << "terrain: can't write " << path.string() << std::endl;
		return false;
	}

	writeU32(out, WORLD_FILE_MAGIC);
	writeU32(out, WORLD_FILE_VERSION);
	writeU32(out, (uint32_t)worldIndex);

	uint32_t nameLength = (uint32_t)world.name.size();
	writeU32(out, nameLength);
	out.write(world.name.data(), nameLength);

	writeU32(out, world.h2Size);
	writeU32(out, world.vSize);
	writeU32(out, (uint32_t)world.rows.size());

	for (const auto &entry : world.rows) {
		writeU32(out, entry.first);
		out.write((const char *)entry.second.data(), (std::streamsize)entry.second.size());
	}

	out.flush();
	if (!out.good()) {
		std::cerr << "terrain: failed to write " << path.string() << std::endl;
		return false;
	}
	return true;
}

bool readWorldFile(const fs::path &path, int &worldIndex, TerrainWorldRows &world) {
	std::ifstream in(path, std::ios::binary);
	if (!in) {
		std::cerr << "terrain: can't read " << path.string() << std::endl;
		return false;
	}

	uint32_t magic = 0;
	uint32_t version = 0;
	uint32_t index = 0;
	if (!readU32(in, magic) || magic != WORLD_FILE_MAGIC || !readU32(in, version) ||
		version != WORLD_FILE_VERSION || !readU32(in, index)) {
		std::cerr << "terrain: bad header in " << path.string() << std::endl;
		return false;
	}

	uint32_t nameLength = 0;
	if (!readU32(in, nameLength) || nameLength > MAX_NAME_LENGTH) {
		std::cerr << "terrain: bad world name in " << path.string() << std::endl;
		return false;
	}
	world.name.resize(nameLength);
	if (nameLength && !in.read(&world.name[0], (std::streamsize)nameLength)) {
		std::cerr << "terrain: truncated world name in " << path.string() << std::endl;
		return false;
	}

	uint32_t rowCount = 0;
	if (!readU32(in, world.h2Size) || !readU32(in, world.vSize) || !readU32(in, rowCount) ||
		!validLayout(world.h2Size, world.vSize) || rowCount > world.vSize) {
		std::cerr << "terrain: bad layout in " << path.string() << std::endl;
		return false;
	}

	for (uint32_t i = 0; i < rowCount; ++i) {
		uint32_t y = 0;
		if (!readU32(in, y) || y >= world.vSize) {
			std::cerr << "terrain: bad row index in " << path.string() << std::endl;
			return false;
		}
		std::vector<unsigned char> row(world.h2Size);
		if (!in.read((char *)row.data(), (std::streamsize)row.size())) {
			std::cerr << "terrain: truncated row in " << path.string() << std::endl;
			return false;
		}
		world.rows[y] = std::move(row);
	}

	worldIndex = (int)index;
	return true;
}

} // namespace

void TerrainStore::clear(void) {
	worlds_.clear();
}

bool TerrainStore::empty(void) const {
	return rowCount() == 0;
}

size_t TerrainStore::rowCount(void) const {
	size_t count = 0;
	for (const auto &entry : worlds_)
		count += entry.second.rows.size();
	return count;
}

void TerrainStore::setRow(
	int worldIndex,
	const char *name,
	uint32_t h2Size,
	uint32_t vSize,
	uint32_t y,
	const unsigned char *row
) {
	if (!name || !row || !validLayout(h2Size, vSize) || y >= vSize || worldIndex < 0)
		return;

	TerrainWorldRows &world = worlds_[worldIndex];
	world.name = name;
	world.h2Size = h2Size;
	world.vSize = vSize;
	world.rows[y].assign(row, row + h2Size);
}

const std::vector<unsigned char> *TerrainStore::getRow(
	int worldIndex,
	const char *name,
	uint32_t h2Size,
	uint32_t vSize,
	uint32_t y
) const {
	if (!name)
		return nullptr;

	const auto worldIt = worlds_.find(worldIndex);
	if (worldIt == worlds_.end())
		return nullptr;

	const TerrainWorldRows &world = worldIt->second;
	if (world.name != name || world.h2Size != h2Size || world.vSize != vSize)
		return nullptr;

	const auto rowIt = world.rows.find(y);
	if (rowIt == world.rows.end())
		return nullptr;
	return &rowIt->second;
}

bool TerrainStore::saveDir(const std::string &dir) const {
	if (dir.empty())
		return false;

	if (rowCount() == 0) {
		std::error_code ec;
		fs::remove_all(dir, ec);
		return !ec;
	}

	std::error_code ec;
	fs::create_directories(dir, ec);
	if (ec) {
		std::cerr << "terrain: can't create " << dir << ": " << ec.message() << std::endl;
		return false;
	}

	bool ok = true;
	std::set<std::string> written;
	for (const auto &entry : worlds_) {
		if (entry.second.rows.empty())
			continue;

		const std::string fileName = worldFileName(entry.first);
		if (!writeWorldFile(fs::path(dir) / fileName, entry.first, entry.second)) {
			ok = false;
			continue;
		}
		written.insert(fileName);
	}

	// Drop world files left from a previous state of the same save slot.
	for (const auto &entry : fs::directory_iterator(dir, ec)) {
		if (ec)
			break;
		if (!entry.is_regular_file())
			continue;
		if (entry.path().extension() != WORLD_FILE_EXTENSION)
			continue;

		const std::string fileName = entry.path().filename().string();
		if (written.find(fileName) == written.end()) {
			std::error_code removeError;
			fs::remove(entry.path(), removeError);
		}
	}

	return ok;
}

bool TerrainStore::loadDir(const std::string &dir) {
	if (dir.empty())
		return false;

	std::error_code ec;
	if (!fs::is_directory(dir, ec))
		return true; // no sidecar for this save - nothing to restore

	for (const auto &entry : fs::directory_iterator(dir, ec)) {
		if (ec)
			break;
		if (!entry.is_regular_file())
			continue;
		if (entry.path().extension() != WORLD_FILE_EXTENSION)
			continue;

		int worldIndex = -1;
		TerrainWorldRows world;
		if (!readWorldFile(entry.path(), worldIndex, world) || worldIndex < 0)
			continue;

		TerrainWorldRows &target = worlds_[worldIndex];
		target.name = std::move(world.name);
		target.h2Size = world.h2Size;
		target.vSize = world.vSize;
		for (auto &row : world.rows)
			target.rows[row.first] = std::move(row.second);
	}

	return true;
}

TerrainStore &terrainStore(void) {
	static TerrainStore store;
	return store;
}

std::string terrainStoreDirForSave(const std::string &savePath) {
	if (savePath.empty())
		return std::string();
	fs::path path(savePath);
	path.replace_extension(".terrain");
	return path.string();
}
