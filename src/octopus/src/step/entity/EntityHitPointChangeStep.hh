#ifndef __EntityHitPointChangeStep__
#define __EntityHitPointChangeStep__

#include "state/entity/Entity.hh"
#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
	/// @brief this is aimed at changing hp
	class EntityHitPointChangeStep : public Steppable
	{
		public:
			/// @brief
			/// @param handle_p
			/// @param delta_p
			/// @param anticipatedHp_p this is the hp of the entity based on the current state and step this steppable is added
			/// using getHpChange from step would be required here
			/// @param hpMax_p max hop of the target to clamp delta if necessary
			EntityHitPointChangeStep(Handle const &handle_p, double delta_p, double anticipatedHp_p, double hpMax_p);

			virtual void apply(State &state_p) const override;
			virtual void revert(State &state_p, SteppableData const *) const override;

			virtual bool isNoOp() const override;
			virtual void visit(SteppableVisitor * visitor_p) const override
			{
				visitor_p->visit(this);
			}

			Handle _handle {0};
			double _delta;
	};
}

#endif
