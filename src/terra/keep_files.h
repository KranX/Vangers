#ifndef __TERRA__KEEP_FILES_H__
#define __TERRA__KEEP_FILES_H__

#include <string>

// Terrain persistence is active when either the global keep option or the
// per-save terrain option is enabled in single player.
extern int KeepON;
// Per-save terrain files are copied to the save slot on save and restored from
// it on load. The working .vmt files themselves are shared session storage.
extern int KeepSaveON;

// Writes resident dirty rows and the keep table of the loaded world to its .vmt.
void FlushKeep(void);
// Copies the current working .vmt files into dir (replacing its contents).
bool SaveKeepSnapshotTo(const std::string &dir);
// Clears working .vmt files and copies the snapshot from dir into their places.
void LoadKeepSnapshotFrom(const std::string &dir);
// Removes all working .vmt files (used when a new game starts).
void ClearKeepSession(void);

// "savegame/save03.dat" -> "savegame/save03.terrain"
std::string TerrainDirForSave(const std::string &savePath);

// Publishes a new save file and (optionally) its terrain snapshot as one
// generation. On failure the previous files are restored and false is
// returned with a description in error.
bool PublishSaveGeneration(
	const std::string &savePath,
	const std::string &newSavePath,
	bool writeTerrain,
	std::string *error
);

#endif
