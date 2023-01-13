#ifndef __ENTITY_MOVE_STEP__
#define __ENTITY_MOVE_STEP__

#include "state/entity/Entity.hh"
#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
	/// @brief this is aimed at dmgi
	class EntityHitPointChangeStep : public Steppable
	{
		public:
			EntityHitPointChangeStep(Handle const &handle_p, double delta_p) : _handle(handle_p), _delta(delta_p) {}

			virtual void apply(State &state_p) const override;
			virtual void revert(State &state_p) const override;

			virtual bool isNoOp() const override;

			Handle _handle {0};
			double _delta;
	};
}

#endif
