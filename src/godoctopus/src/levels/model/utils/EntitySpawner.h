#ifndef __Godoctopus_Entity_Spawner__
#define __Godoctopus_Entity_Spawner__

#include <list>

namespace octopus
{
	class Library;
	class Steppable;
	struct Handle;
}

namespace godot {

struct GodotEntity;

bool spawnEntity(std::list<octopus::Steppable *> &steps_r, octopus::Handle const&idx_p, GodotEntity const &ent_p,
	octopus::Library const &lib_p, unsigned long playerCount_p);

}

#endif