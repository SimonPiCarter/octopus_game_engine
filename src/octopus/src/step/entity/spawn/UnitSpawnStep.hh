#ifndef __UNIT_SPAWN_STEP__
#define __UNIT_SPAWN_STEP__

#include "SpawnModelStep.hh"
#include "state/entity/Unit.hh"

namespace octopus
{
class UnitSpawnStep : public SpawnModelStep<Unit>
{
public:
	explicit UnitSpawnStep(Handle const &handle_p, Unit const &model_p);
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}
};
}

#endif
