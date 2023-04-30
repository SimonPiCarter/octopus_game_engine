#ifndef __LevelId__
#define __LevelId__

#include <cstddef>
#include <fstream>

#define LEVEL_ID_ARENA 0
#define LEVEL_ID_ARENA_KAMIKAZE 1
#define LEVEL_ID_MAZE 2

/// @brief write level id and size of the world in a binary file
void writeLevelId(std::ofstream &file_p, size_t const &id_p, size_t const &size_p);

#endif