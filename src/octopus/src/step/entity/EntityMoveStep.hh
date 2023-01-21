#ifndef __ENTITY_MOVE_STEP__
#define __ENTITY_MOVE_STEP__

#include "state/entity/Entity.hh"
#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
	class EntityMoveStep : public Steppable
	{
		public:
			EntityMoveStep(Handle const &handle_p, Vector const &move_p) : _handle(handle_p), _move(move_p) {}

			virtual void apply(State &state_p) const override;
			virtual void revert(State &state_p) const override;

			virtual bool isNoOp() const override;
			virtual void visit(SteppableVisitor * visitor_p) const override
			{
				visitor_p->visit(this);
			}

			Handle _handle {0};
			Vector _move;
	};

	EntityMoveStep createEntityMoveStep(Entity const &ent_p);
	/// @brief Compute the step of an Entity going to the target position with a given step speed
	EntityMoveStep createEntityMoveStep(Entity const &ent_p, Vector const &target_p, double stepSpeed_p);

	/// @brief check if step is no op (no position update)
	bool isEntityMoveStepNoOp(EntityMoveStep const &step_p);
}

#endif
