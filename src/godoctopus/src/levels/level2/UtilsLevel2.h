#ifndef __UtilsLevel2__
#define __UtilsLevel2__

#include <string>
#include <list>

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

std::string genModelName(octopus::RandomGenerator &gen_p);

std::list<octopus::Steppable *> createWallSpawners(octopus::Library &lib_p, unsigned long x, unsigned long y, unsigned long startYGate, unsigned long endYGate, octopus::Handle &handle_p);

} // level2
} // godot
#endif
