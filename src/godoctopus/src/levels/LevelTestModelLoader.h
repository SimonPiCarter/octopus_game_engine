#ifndef __LevelTestModel__
#define __LevelTestModel__

#include <list>
#include <vector>
#include <functional>
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
namespace level_test_model
{

std::list<octopus::Steppable *> LevelSteps(octopus::Library &lib_p, octopus::RandomGenerator &rand_p);
std::list<octopus::Command *> LevelCommands(octopus::Library &lib_p, octopus::RandomGenerator &rand_p);

/// @brief write header for classic arena level
void writeLevelHeader(std::ofstream &file_p, int seed_p);
/// @brief read header for classic arena level and return a pair of steppable and command (warning delete and reset rand_p using the written seed)
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readLevelHeader(octopus::Library &lib_p, std::ifstream &file_p,
	octopus::RandomGenerator * &rand_p);

} // namespace level2
} // namespace godot

#endif