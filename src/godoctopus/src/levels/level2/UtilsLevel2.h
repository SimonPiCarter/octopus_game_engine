#ifndef __UtilsLevel2__
#define __UtilsLevel2__

#include <string>
#include <list>
#include <vector>

#include <state/Handle.hh>

namespace octopus
{
	class Library;
    class RandomGenerator;
	class Steppable;
} // namespace octopus

namespace godot
{
namespace level2
{

std::list<octopus::Steppable *> createWallSpawners(octopus::Library &lib_p, unsigned long xStart, unsigned long x, unsigned long y,
	unsigned long startYGate, unsigned long endYGate,
	octopus::Handle &handle_p, std::vector<unsigned long> const &players_p);

} // level2
} // godot
#endif
