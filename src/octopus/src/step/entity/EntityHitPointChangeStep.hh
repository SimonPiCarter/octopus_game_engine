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
			EntityHitPointChangeStep(Handle const &handle_p, double delta_p) : _handle(handle_p), _delta(delta_p) {}

			virtual void apply(State &state_p, SteppableData *) const override;
			virtual void revert(State &state_p, SteppableData *) const override;

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
