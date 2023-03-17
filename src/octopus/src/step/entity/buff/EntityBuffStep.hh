#ifndef __EntityBuffStep__
#define __EntityBuffStep__

#include <stdexcept>

#include "state/entity/Entity.hh"
#include "state/entity/Buff.hh"
#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{

/// @brief This struct is aimed at supporting
/// buff data
class EntityBuffStep : public Steppable
{
public:
	EntityBuffStep(Handle const &target_p, TyppedBuff const &buff_p, unsigned long oldTime_p, bool new_p)
	: _target(target_p)
	, _buff(buff_p)
	, _oldTime(oldTime_p)
	, _new(new_p)
	{}

	virtual void apply(State &state_p, SteppableData *) const override;
	virtual void revert(State &state_p, SteppableData *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}
private:
	/// @brief target of the buff
	Handle const _target;

	/// @brief buff to be applied
	TyppedBuff const _buff;

	/// @brief used in revert to set up correct time
	unsigned long const _oldTime;

	/// @brief true if the buff was not applied before this step
	/// @note it may have been applied in the same step
	bool const _new;
};

} // namespace octopus


#endif
