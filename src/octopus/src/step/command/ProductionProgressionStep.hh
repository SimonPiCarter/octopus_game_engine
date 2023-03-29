#ifndef __ProductionProgressionStep__
#define __ProductionProgressionStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
/// @brief this incread production for the given command
class ProductionProgressionStep : public Steppable
{
	public:
		ProductionProgressionStep(Handle const &handle_p, double delta_p) : _handle(handle_p), _delta(delta_p) {}

		virtual void apply(State &state_p) const override;
		virtual void revert(State &state_p, SteppableData const *) const override;

		virtual bool isNoOp() const override;
		virtual void visit(SteppableVisitor * visitor_p) const override
		{
			visitor_p->visit(this);
		}

		Handle const _handle {0};
		double const _delta;
};
}

#endif
