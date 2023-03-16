#ifndef __EntityUpdateWaitingStep__
#define __EntityUpdateWaitingStep__

#include "state/entity/Entity.hh"
#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
	/// @brief this is aimed at changing hp
	class EntityUpdateWaitingStep : public Steppable
	{
		public:
			EntityUpdateWaitingStep(Handle const &handle_p, unsigned long old_p, unsigned long new_p) : _handle(handle_p), _old(old_p), _new(new_p) {}

			virtual void apply(State &state_p, SteppableData *) const override;
			virtual void revert(State &state_p, SteppableData *) const override;

			virtual bool isNoOp() const override;
			virtual void visit(SteppableVisitor * visitor_p) const override
			{
				visitor_p->visit(this);
			}

			Handle const _handle;
			unsigned long const _old;
			unsigned long const _new;
	};
}

#endif
