#ifndef __EntityAttackStep__
#define __EntityAttackStep__

#include "state/entity/Entity.hh"
#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
	/// @brief this is aimed at dmg or heal on enitites
	class EntityAttackStep : public Steppable
	{
		public:
			EntityAttackStep(Handle const &handle_p, unsigned long timeSinceLastAttack_p)
				: _handle(handle_p) , _timeSinceLastAttack(timeSinceLastAttack_p){}

			virtual void apply(State &state_p) const override;
			virtual void revert(State &state_p) const override;

			virtual bool isNoOp() const override;

			Handle _handle {0};
			unsigned long _timeSinceLastAttack {0};
	};
}

#endif
