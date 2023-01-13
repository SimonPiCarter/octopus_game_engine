#ifndef __ENTITY_SPAWN_STEP__
#define __ENTITY_SPAWN_STEP__

#include "state/entity/Entity.hh"
#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
	class EntitySpawnStep : public Steppable
	{
		public:
			EntitySpawnStep(Entity const &model_p) : _model(model_p) {}

			virtual void apply(State &state_p) const override;
			virtual void revert(State &state_p) const override;

			virtual bool isNoOp() const override;
		private:
			/// @brief setup on first apply (that's why it must be mutable)
			mutable bool _initialized {false};
			mutable Handle _handle {0};

			Entity _model;
	};
}

#endif
