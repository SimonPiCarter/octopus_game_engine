#ifndef __MazeLevel__
#define __MazeLevel__

#include <cstddef>
#include <fstream>
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

/// @brief write header for classic arena level
void writeMazeLevelHeader(std::ofstream &file_p, size_t number_p);
/// @brief read header for classic arena level and return a pair of steppable and command
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readMazeLevelHeader(octopus::Library &lib_p, std::ifstream &file_p);

#endif
