#ifndef __ProductionPaidStep__
#define __ProductionPaidStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{
/// @brief this incread production for the given command
class ProductionPaidStep : public Steppable
{
	public:
		ProductionPaidStep(Handle const &handle_p) : _handle(handle_p) {}

		virtual void apply(State &state_p) const override;
		virtual void revert(State &state_p) const override;

		virtual bool isNoOp() const override
		{
			return false;
		}
		virtual void visit(SteppableVisitor * visitor_p) const override
		{
			visitor_p->visit(this);
		}

		Handle const _handle {0};
};
}

#endif
