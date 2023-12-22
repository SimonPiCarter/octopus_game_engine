#ifndef __HeroSiegeLevel2__
#define __HeroSiegeLevel2__

#include <list>
#include <vector>
#include <functional>
#include <fstream>
#include "controller/trigger/Trigger.hh"
#include "utils/RandomGenerator.hh"

#define LEVEL_ID_HEROSIEGE 1005

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
namespace hero_siege
{

std::list<octopus::Steppable *> HeroSiegeLevelSteps(octopus::Library &lib_p, octopus::RandomGenerator &rand_p, unsigned long playerCount_p);
std::list<octopus::Command *> HeroSiegeLevelCommands(octopus::Library &lib_p, octopus::RandomGenerator &rand_p, unsigned long playerCount_p);

struct HeroSiegeLevelHeader
{
	// seed for random generation
	int seed;

	/// @brief player count
	unsigned long playerCount;
};

/// @brief write header for classic wave level
void writeHeroSiegeLevelHeader(std::ofstream &file_p, HeroSiegeLevelHeader const &header_p);
/// @brief read header for classic wave level
void readHeroSiegeLevelHeader(std::ifstream &file_p, HeroSiegeLevelHeader &header_r);
/// @brief read header for classic wave level and return a pair of steppable and command (warning delete and reset rand_p using the written seed)
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readHeroSiegeLevelHeader(octopus::Library &lib_p, std::ifstream &file_p,
	octopus::RandomGenerator * &rand_p, HeroSiegeLevelHeader &header_r);

} // namespace level2
} // namespace godot

#endif
