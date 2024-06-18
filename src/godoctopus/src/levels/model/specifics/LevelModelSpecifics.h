#ifndef __Godoctopus_LevelModelSpecifics__
#define __Godoctopus_LevelModelSpecifics__

#include <list>
#include <vector>
#include <string>

namespace octopus
{
    class Library;
    class Steppable;
} // namespace octopus

namespace godot
{

struct GodotEntityInfo;

void add_specifics(std::string const &specific_p, std::list<octopus::Steppable *> &steps_r, octopus::Library const &lib_p,
	unsigned long playerCount_p, std::vector<GodotEntityInfo> const &entityInfo_p);

}

#endif
