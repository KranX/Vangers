#ifndef __TERRA__TERRAIN_STORE_H__
#define __TERRA__TERRAIN_STORE_H__

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

// Surface rows (heights + attributes, H2_SIZE bytes each) captured for the
// current save. The data is stored next to the save file in a
// "saveNN.terrain" directory, one file per modified world.
struct TerrainWorldRows {
	std::string name;
	uint32_t h2Size = 0;
	uint32_t vSize = 0;
	std::map<uint32_t, std::vector<unsigned char>> rows;
};

class TerrainStore {
public:
	void clear(void);
	bool empty(void) const;
	size_t rowCount(void) const;

	// Replaces the stored row of the world. Ignored when the row does not fit.
	void setRow(
		int worldIndex,
		const char *name,
		uint32_t h2Size,
		uint32_t vSize,
		uint32_t y,
		const unsigned char *row
	);

	// Returns nullptr when the world/row is unknown or the map layout differs.
	const std::vector<unsigned char> *getRow(
		int worldIndex,
		const char *name,
		uint32_t h2Size,
		uint32_t vSize,
		uint32_t y
	) const;

	bool saveDir(const std::string &dir) const;
	bool loadDir(const std::string &dir);

private:
	std::map<int, TerrainWorldRows> worlds_;
};

TerrainStore &terrainStore(void);

// Captures changed surface rows of the currently loaded world. No-op when no
// world is loaded or in network game.
void captureTerrainChangedRows(void);

// "savegame/save03.dat" -> "savegame/save03.terrain"
std::string terrainStoreDirForSave(const std::string &savePath);

#endif
