#ifndef __AttackModifierStep__
#define __AttackModifierStep__

#include <stdexcept>

#include "state/entity/Entity.hh"
#include "state/entity/Buff.hh"
#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{


/// @brief memento data for attack mod step
struct AttackModifierStepData : public SteppableData {

	AttackModifierStepData(AttackModifier const &old_p) : SteppableData(), _old(old_p) {}
	/// @brief used in revert to set up correct time
	AttackModifier const _old;
};

/// @brief This struct is aimed at supporting
/// buff data
class AttackModifierStep : public Steppable
{
public:
	AttackModifierStep(Handle const &target_p, AttackModifier const &modifier_p)
	: _target(target_p)
	, _mod(modifier_p)
	{}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *data_p) const override;

	virtual bool isNoOp() const override { return false; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	/// @brief create an optional new SteppableData
	/// @return nullptr if not required
	virtual SteppableData * newData(State const &) const;
private:
	/// @brief target of the buff
	Handle const _target;

	AttackModifier const _mod;
};

} // namespace octopus


#endif
