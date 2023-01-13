#ifndef __ENTITY_STEP__
#define __ENTITY_STEP__

#include "state/entity/Entity.hh"
#include "utils/Vector.hh"

namespace octopus
{
	class EntityStep
	{
		public:
			Entity * _ent { nullptr };
			Vector _newPosition;
	};

	EntityStep createEntityStep(Entity &ent_p);
	/// @brief Compute the step of an Entity going to the target position with a given step speed
	EntityStep createEntityStep(Entity &ent_p, Vector const &target_p, double stepSpeed_p);

	/// @brief check if step is no op (no position update)
	bool isEntityStepNoOp(EntityStep const &step_p);
}

#endif
