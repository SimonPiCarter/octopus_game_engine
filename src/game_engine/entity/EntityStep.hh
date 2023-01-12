#ifndef __ENTITY_STEP__
#define __ENTITY_STEP__

#include "Entity.hh"
#include "Vector.hh"

namespace game_engine
{
	class EntityStep
	{
		public:
			Entity * _ent { nullptr };
			Vector _newPosition;
	};

	EntityStep createEntityStep(Entity &ent_p);
	/// @brief Compute the step of an Entity going to the target position with a given step speed
	EntityStep createEntityStep(Entity &ent_p, Vector const &target_p, double stepSpeed_p);
}

#endif
