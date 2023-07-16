#ifndef __Area__
#define __Area__

#include <list>

namespace octopus
{
class Entity;

struct Area
{
	/// @brief left position of the area in the world
	unsigned long x{ 0 };
	/// @brief top position of the area in the world
	unsigned long y{ 0 };
	/// @brief width of the area in the world
	unsigned long width{ 1 };
	/// @brief height of the area in the world
	unsigned long height{ 1 };
};

struct AreaSpawn : public Area
{
	/// @brief the list of model to spawn in the area
	std::list<std::pair<Entity *, unsigned long> > entities;
};

} // namespace octopus


#endif
