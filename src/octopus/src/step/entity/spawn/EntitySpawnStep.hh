#ifndef __ENTITY_SPAWN_STEP__
#define __ENTITY_SPAWN_STEP__

#include "SpawnModelStep.hh"
#include "state/entity/Entity.hh"

namespace octopus
{
class EntitySpawnStep : public SpawnModelStep<Entity>
{
public:
	explicit EntitySpawnStep(Entity const &model_p);
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}
};
}

#endif
