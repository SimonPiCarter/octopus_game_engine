#ifndef __Mission1__
#define __Mission1__

#include <list>
#include <vector>
#include <functional>
#include <fstream>
#include "controller/trigger/Trigger.hh"
#include "utils/RandomGenerator.hh"

#define LEVEL_ID_MISSION_1 2000

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

std::list<octopus::Steppable *> Mission1Steps(octopus::Library &lib_p, octopus::RandomGenerator &rand_p, unsigned long nbPlayers_p);
std::list<octopus::Command *> Mission1Commands(octopus::Library &lib_p, octopus::RandomGenerator &rand_p, unsigned long nbPlayers_p);

struct Mission1Header
{
	int seed;
	unsigned long nb_players;
};

/// @brief write header for classic mission 1
void writeMission1Header(std::ofstream &file_p, Mission1Header const &header_p);
/// @brief read header for classic mission 1 level and return a pair of steppable and command (warning delete and reset rand_p using the written seed)
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readMission1Header(octopus::Library &lib_p, std::ifstream &file_p,
	octopus::RandomGenerator * &rand_p, Mission1Header &header_r);

} // namespace level1
} // namespace godot

#endif
