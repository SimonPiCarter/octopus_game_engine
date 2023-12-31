#ifndef __Godoctopus_Entity_Spawner__
#define __Godoctopus_Entity_Spawner__

#include <list>
#include <string>
#include <vector>

namespace octopus
{
	class Library;
	class Steppable;
	struct Handle;
}

namespace godot {

struct GodotEntity;

struct GodotEntityInfo
{
	std::string model;
	unsigned long handle;
	unsigned long player;
	unsigned long x;
	unsigned long y;
};

/// @brief Compute handle info of all entities based on the number of player
/// @param entities_p
/// @param playerCount_p
/// @param startHandle_p
std::vector<GodotEntityInfo> getEntityInfo(std::vector<GodotEntity> const &entities_p, unsigned long playerCount_p, unsigned long startHandle_p=0);

/// @brief Get the handles for the given player and model
std::vector<unsigned long> getHandles(std::vector<GodotEntityInfo> const &info_p, unsigned long player_p, std::string const &model_p);
/// @brief Get the handles for the model
std::vector<unsigned long> getHandlesAnyPlayer(std::vector<GodotEntityInfo> const &info_p, std::string const &model_p);

bool spawnEntity(std::list<octopus::Steppable *> &steps_r, octopus::Handle const&idx_p, GodotEntity const &ent_p,
	octopus::Library const &lib_p, unsigned long playerCount_p);

}

#endif