#ifndef __BuildingCancelStep__
#define __BuildingCancelStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{
/// @brief this is aimed at building building
class BuildingCancelStep : public Steppable
{
	public:
		BuildingCancelStep(Handle const &handle_p, bool old_p, bool new_p) : _handle(handle_p), _old(old_p), _new(new_p) {}

		virtual void apply(State &state_p) const override;
		virtual void revert(State &state_p, SteppableData const *) const override;

		virtual bool isNoOp() const override { return false; }
		virtual void visit(SteppableVisitor * visitor_p) const override
		{
			visitor_p->visit(this);
		}

		Handle const _handle;
		bool const _old;
		bool const _new;
};
}

#endif
