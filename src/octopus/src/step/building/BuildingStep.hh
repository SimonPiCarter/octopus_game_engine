#ifndef __BuildingStep__
#define __BuildingStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Fixed.hh"

namespace octopus
{
/// @brief this is aimed at building building
class BuildingStep : public Steppable
{
	public:
		BuildingStep(Handle const &source_p, Handle const &handle_p, Fixed delta_p) : _source(source_p), _handle(handle_p), _delta(delta_p) {}

		virtual void apply(State &state_p) const override;
		virtual void revert(State &state_p, SteppableData const *) const override;

		virtual bool isNoOp() const override;
		virtual void visit(SteppableVisitor * visitor_p) const override
		{
			visitor_p->visit(this);
		}

		Handle const _source {0};
		Handle const _handle {0};
		Fixed const _delta;
};
}

#endif
