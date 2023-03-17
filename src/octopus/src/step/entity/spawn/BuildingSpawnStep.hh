#ifndef __BUILDING_SPAWN_STEP__
#define __BUILDING_SPAWN_STEP__

#include "SpawnModelStep.hh"
#include "state/entity/Building.hh"

namespace octopus
{
class BuildingSpawnStep : public SpawnModelStep<Building>
{
public:
	explicit BuildingSpawnStep(Handle const &handle_p, Building const &model_p, bool builtAlready_p);
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}


	virtual void apply(State &state_p, SteppableData *) const override;
};
}

#endif
