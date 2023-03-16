#ifndef __BuildingStep__
#define __BuildingStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{
/// @brief this is aimed at building building
class BuildingStep : public Steppable
{
	public:
		BuildingStep(Handle const &handle_p, double delta_p) : _handle(handle_p), _delta(delta_p) {}

		virtual void apply(State &state_p, SteppableData *) const override;
		virtual void revert(State &state_p, SteppableData *) const override;

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
