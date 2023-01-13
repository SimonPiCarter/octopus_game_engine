#ifndef __ENTITY_STEP__
#define __ENTITY_STEP__

#include "state/entity/Entity.hh"
#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
	class EntityStep : public Steppable
	{
		public:
			EntityStep(Handle handle_p, Vector move_p) : _handle(handle_p), _move(move_p) {}

			virtual void apply(State &state_p) const override;
			virtual void revert(State &state_p) const override;

			virtual bool isNoOp() const override;

			Handle _handle {0};
			Vector _move;
	};

	EntityStep createEntityStep(Entity const &ent_p);
	/// @brief Compute the step of an Entity going to the target position with a given step speed
	EntityStep createEntityStep(Entity const &ent_p, Vector const &target_p, double stepSpeed_p);

	/// @brief check if step is no op (no position update)
	bool isEntityStepNoOp(EntityStep const &step_p);
}

#endif
