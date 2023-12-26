#ifndef __LevelTestModel__
#define __LevelTestModel__

#include <list>
#include <vector>
#include <functional>
#include <fstream>
#include "utils/RandomGenerator.hh"

#define LEVEL_ID_LEVEL_TEST_MODEL 1003

namespace octopus
{
	class Command;
	class Library;
	class State;
	class Step;
	class Steppable;
} // namespace octopus

namespace godot
{

class FileHeader;

namespace level_test_model
{

std::list<octopus::Steppable *> LevelSteps(octopus::Library &lib_p, octopus::RandomGenerator &rand_p, bool buffProd_p,
	bool upgrades_rune_p, unsigned long idx_first_player_p, unsigned long nb_players_p);
std::list<octopus::Command *> LevelCommands(octopus::Library &lib_p, octopus::RandomGenerator &rand_p);

struct ModelLoaderHeader
{
	int seed {0};
	bool buff_prod {false};
	bool upgrade_rune {false};
};

/// @brief write header for classic arena level
void writeLevelHeader(std::ofstream &file_p, ModelLoaderHeader const &header_p);
/// @brief read header for classic arena level and return a pair of steppable and command (warning delete and reset rand_p using the written seed)
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readLevelHeader(octopus::Library &lib_p, std::ifstream &file_p,
	octopus::RandomGenerator * &rand_p, ModelLoaderHeader &header_r, FileHeader const &_fileHeader);

} // namespace level2
} // namespace godot

#endif
