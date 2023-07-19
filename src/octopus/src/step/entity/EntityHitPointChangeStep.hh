#ifndef __EntityHitPointChangeStep__
#define __EntityHitPointChangeStep__

#include "state/entity/Entity.hh"
#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{

	struct HitPointChangeData : public SteppableData
	{
		HitPointChangeData(const Fixed &fixed_p) : _delta(fixed_p) {}
		Fixed _delta;
	};

	/// @brief this is aimed at changing hp
	class EntityHitPointChangeStep : public Steppable
	{
		public:
			/// @brief
			/// @param handle_p
			/// @param delta_p
			/// @param lethal_p if set to false this step will not kill the entity (it will let it at 1hp)
			/// @param forced_p should only be used in testing, will force healing even when entity is dead
			/// as it may result in failure in real run circumstance
			EntityHitPointChangeStep(Handle const &handle_p, Fixed delta_p, bool lethal_p=true, bool forced_p=false);

			virtual void apply(State &state_p) const override;
			virtual void revert(State &state_p, SteppableData const *) const override;

			virtual bool isNoOp() const override;
			virtual void visit(SteppableVisitor * visitor_p) const override
			{
				visitor_p->visit(this);
			}

			/// @brief create data to know how much of the delta was applied regarding
			/// current state of application
			virtual SteppableData * newData(State const &state_p) const override;

			Handle _handle {0};
			Fixed _delta;
			bool _lethal;
			bool _forced;
	};
}

#endif
