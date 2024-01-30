#ifndef __Mission2__
#define __Mission2__

#include <list>
#include <vector>
#include <functional>
#include <fstream>
#include "utils/RandomGenerator.hh"

#include "levels/model/utils/EntitySpawner.h"

#define LEVEL_ID_MISSION_2 2001

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

namespace mission
{

std::list<octopus::Steppable *> Mission2Steps(octopus::Library &lib_p, octopus::RandomGenerator &rand_p, unsigned long nbPlayers_p, std::vector<GodotEntityInfo> const &entityInfo_p);
std::list<octopus::Command *> Mission2Commands(octopus::Library &lib_p, octopus::RandomGenerator &rand_p, unsigned long nbPlayers_p);

struct Mission2Header
{
	int seed;
	unsigned long nb_players;
};

/// @brief write header for classic mission 2
void writeMission2Header(std::ofstream &file_p, Mission2Header const &header_p);
/// @brief read header for classic mission 2 level and return a pair of steppable and command (warning delete and reset rand_p using the written seed)
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readMission2Header(octopus::Library &lib_p, std::ifstream &file_p,
	std::vector<GodotEntityInfo> const &entityInfo_p,
	octopus::RandomGenerator * &rand_p, Mission2Header &header_r);

} // namespace mission
} // namespace godot

#endif
