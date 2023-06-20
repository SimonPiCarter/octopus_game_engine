#ifndef __EntityUpdateReloadAbilityStep__
#define __EntityUpdateReloadAbilityStep__

#include "state/entity/Entity.hh"
#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
	/// @brief this is aimed at changing ability reloead time
	class EntityUpdateReloadAbilityStep : public Steppable
	{
		public:
			EntityUpdateReloadAbilityStep(Handle const &handle_p, std::string const &key_p, unsigned long old_p, unsigned long new_p)
				: _handle(handle_p), _key(key_p), _old(old_p), _new(new_p) {}

			virtual void apply(State &state_p) const override;
			virtual void revert(State &state_p, SteppableData const *) const override;

			virtual bool isNoOp() const override;
			virtual void visit(SteppableVisitor * visitor_p) const override
			{
				visitor_p->visit(this);
			}

			Handle const _handle;
			std::string _key;
			unsigned long const _old;
			unsigned long const _new;
	};
}

#endif
