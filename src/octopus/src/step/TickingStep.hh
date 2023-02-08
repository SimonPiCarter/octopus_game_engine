#ifndef __TickingStep__
#define __TickingStep__

#include "state/entity/Entity.hh"
#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
	/// @brief This step will contains all ticking that must happen on every step
	/// eg reload times, buff duration
	class TickingStep : public Steppable
	{
		public:
			TickingStep() {}

			virtual void apply(State &state_p) const override;
			virtual void revert(State &state_p) const override;

			virtual bool isNoOp() const override;

			virtual void visit(SteppableVisitor * visitor_p) const override
			{
				visitor_p->visit(this);
			}

			static double const _anchorLoss;
	};
}

#endif
