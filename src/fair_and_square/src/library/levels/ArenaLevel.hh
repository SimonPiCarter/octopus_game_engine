#ifndef __ArenaLevel__
#define __ArenaLevel__

#include <list>

namespace octopus
{
	class Command;
	class Library;
	class State;
	class Step;
	class Steppable;
} // namespace octopus


std::list<octopus::Steppable *> ArenaLevelSteps(octopus::Library &lib_p, size_t number_p);
std::list<octopus::Command *> ArenaLevelCommands(octopus::Library &lib_p);

#endif
