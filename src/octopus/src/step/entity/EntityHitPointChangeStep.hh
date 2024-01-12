#ifndef __EntityHitPointChangeStep__
#define __EntityHitPointChangeStep__

#include "state/entity/Entity.hh"
#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
	/// @brief this is aimed at changing hp
	class EntityHitPointChangeStep : public Steppable
	{
		public:
			/// @brief
			/// @param handle_p
			/// @param delta_p
			EntityHitPointChangeStep(Handle const &handle_p, Fixed delta_p);

			virtual void consolidate(State const &state_p, Step const & step_p);

			virtual void apply(State &state_p) const override;
			virtual void revert(State &state_p, SteppableData const *) const override;

			virtual bool isNoOp() const override;
			virtual void visit(SteppableVisitor * visitor_p) const override
			{
				visitor_p->visit(this);
			}

			Handle _handle {0};
			Fixed _delta;
	};

	EntityHitPointChangeStep * makeNewEntityHitPointChangeStep(Entity const &entity_p, Step const &step_p, Fixed const &change_p);
}

#endif
