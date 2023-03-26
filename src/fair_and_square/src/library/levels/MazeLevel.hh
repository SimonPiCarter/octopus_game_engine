#ifndef __MazeLevel__
#define __MazeLevel__

#include <list>

namespace octopus
{
	class Command;
	class Library;
	class State;
	class Step;
	class Steppable;
} // namespace octopus


std::list<octopus::Steppable *> MazeLevelSteps(octopus::Library &lib_p, size_t number_p);
std::list<octopus::Command *> MazeLevelCommands(octopus::Library &lib_p);

#endif
