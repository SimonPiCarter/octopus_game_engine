#ifndef __ArenaLevel__
#define __ArenaLevel__

#include <vector>
#include <list>
#include <string>

namespace octopus
{
	class Command;
	class Library;
	class State;
	class Step;
	class Steppable;
} // namespace octopus

struct ArenaInfo
{
	size_t nb;
	std::string model;
};

std::list<octopus::Steppable *> ArenaLevelSteps(octopus::Library &lib_p, size_t number_p);
std::list<octopus::Steppable *> ArenaLevelSteps(octopus::Library &lib_p, std::vector<ArenaInfo> const &you_p, std::vector<ArenaInfo> const &them_p);

/// @brief Create a steps to setup a level where you need to dodge kamikaze units
/// @param numberYou_p number of unit to dodge
/// @param numberThem_p number of kamikaze units
std::list<octopus::Steppable *> ArenaKamikazeSteps(octopus::Library &lib_p, size_t numberYou_p, size_t numberThem_p, bool fast_p);

std::list<octopus::Command *> ArenaLevelCommands(octopus::Library &lib_p);

/// @brief write header for classic arena level
void writeArenaLevelHeader(std::ofstream &file_p, std::vector<ArenaInfo> const &you_p, std::vector<ArenaInfo> const &them_p);
/// @brief read header for classic arena level and return a pair of steppable and command
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readArenaLevelHeader(octopus::Library &lib_p, std::ifstream &file_p);

/// @brief write header for kamikaze arena level
void writeArenaKamikazeHeader(std::ofstream &file_p, size_t const &you_p, size_t const &them_p, bool fast_p);
/// @brief read header for kamikaze arena level and return a pair of steppable and command
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readArenaKamikazeHeader(octopus::Library &lib_p, std::ifstream &file_p);

#endif
