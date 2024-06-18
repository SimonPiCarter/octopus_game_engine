#ifndef __BuildingAutoBuildStep__
#define __BuildingAutoBuildStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{
	struct UnitModel;

	/// @brief this is aimed at dmg or heal on enitites
	class BuildingAutoBuildStep : public Steppable
	{
		public:
			BuildingAutoBuildStep(Handle const &handle_p, UnitModel const *old_p, UnitModel const *new_p)
				: _handle(handle_p) , _old(old_p), _new(new_p) {}

			virtual void apply(State &state_p) const override;
			virtual void revert(State &state_p, SteppableData const *) const override;

			virtual bool isNoOp() const override
			{
				return _old == _new;
			}
			virtual void visit(SteppableVisitor * visitor_p) const override
			{
				visitor_p->visit(this);
			}

			Handle _handle {0};
			UnitModel const * _old { nullptr };
			UnitModel const * _new { nullptr };
	};
}

#endif
