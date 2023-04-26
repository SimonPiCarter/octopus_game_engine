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
std::list<octopus::Command *> ArenaLevelCommands(octopus::Library &lib_p);

#endif
