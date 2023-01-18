#ifndef __UNIT_SPAWN_STEP__
#define __UNIT_SPAWN_STEP__

#include "SpawnModelStep.hh"
#include "state/entity/Unit.hh"

namespace octopus
{
class UnitSpawnStep : public SpawnModelStep<Unit>
{
public:
	explicit UnitSpawnStep(Unit const &model_p);
};
}

#endif
