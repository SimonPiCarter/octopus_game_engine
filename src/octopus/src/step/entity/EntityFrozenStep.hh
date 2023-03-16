#ifndef __EntityFrozenStep__
#define __EntityFrozenStep__

#include "state/entity/Entity.hh"
#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
	/// @brief this is aimed at changing hp
	class EntityFrozenStep : public Steppable
	{
		public:
			EntityFrozenStep(Handle const &handle_p, bool old_p, bool new_p) : _handle(handle_p), _old(old_p), _new(new_p) {}

			virtual void apply(State &state_p, SteppableData *) const override;
			virtual void revert(State &state_p, SteppableData *) const override;

			virtual bool isNoOp() const override;
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
