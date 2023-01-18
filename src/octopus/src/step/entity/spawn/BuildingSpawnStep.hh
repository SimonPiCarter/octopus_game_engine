#ifndef __BUILDING_SPAWN_STEP__
#define __BUILDING_SPAWN_STEP__

#include "SpawnModelStep.hh"
#include "state/entity/Building.hh"

namespace octopus
{
class BuildingSpawnStep : public SpawnModelStep<Building>
{
public:
	explicit BuildingSpawnStep(Building const &model_p);
};
}

#endif
