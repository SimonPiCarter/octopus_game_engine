#ifndef __BuildingBlueprintCommand__
#define __BuildingBlueprintCommand__

#include <list>
#include <random>

#include "command/Command.hh"

#include "SpawningGrid.hh"

namespace octopus
{
struct BuildingModel;;
struct EntityModel;

struct AreaSpawn
{
	/// @brief left position of the area in the world
	unsigned long x;
	/// @brief top position of the area in the world
	unsigned long y;
	/// @brief width and height of the area in the world
	unsigned long size;
	/// @brief the list of model to spawn in the area
	std::list<std::pair<EntityModel *, unsigned long> > models;
	/// @brief the player id for units and buildings
	unsigned long player;
};

/// @brief This command will spawn models randomly in the given areas
/// @warning if multiple area spawner share the same area during the same
/// step they may spawn static on top of eachoter
class AreaSpawnerCommand : public Command
{
public:
	AreaSpawnerCommand(std::list<AreaSpawn> const &spawns_p);

	virtual void registerCommand(Step & step_p, State const &state_p) override;

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p) const override;

	virtual CommandData * newData() const override { return nullptr; }

	/// @brief for testing purpose mainly spawner can be set up to be non random
	/// @note in that case the first option (first left, top) will be taken
	void setNonRandom() { _nonRandom = true; }

private:
	/// @brief randomize between min_p and max_p - 1
	/// @note if _nonRandom is true then always return min_p
	unsigned long random(unsigned long min_p, unsigned long max_p);

	/// @brief list of spawns
	std::list<AreaSpawn> const _spawns;

	/// @brief random generator
	std::mt19937 _gen;

	bool _nonRandom;
};

} // namespace octopus


#endif
