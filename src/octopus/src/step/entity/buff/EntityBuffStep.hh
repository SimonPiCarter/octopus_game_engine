#ifndef __EntityBuffStep__
#define __EntityBuffStep__

#include <stdexcept>

#include "state/entity/Entity.hh"
#include "state/entity/buff/ConditionalBuff.hh"
#include "state/entity/buff/TimedBuff.hh"
#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{


/// @brief memento data for buff step
struct EntityBuffStepData : public SteppableData {

	EntityBuffStepData(unsigned long old_p, bool new_p) : SteppableData(), _oldTime(old_p), _new(new_p) {}
	/// @brief used in revert to set up correct time
	unsigned long const _oldTime;

	/// @brief true if the buff was not applied before this step
	/// @note it may have been applied in the same step
	bool const _new;
};

/// @brief class handles applying a buff to an entity
class EntityBuffStep : public Steppable
{
public:
	EntityBuffStep(Handle const &target_p, TimedBuff const &buff_p)
	: _target(target_p)
	, _buff(buff_p)
	{}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *data_p) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	/// @brief create an optional new SteppableData
	/// @return nullptr if not required
	virtual SteppableData * newData(State const &) const;

	/// @brief target of the buff
	Handle const _target;

	/// @brief buff to be applied
	TimedBuff const _buff;
};

/// @brief class handles applying a conditional buff to an entity
class EntityConditionalBuffStep : public Steppable
{
public:
	EntityConditionalBuffStep(Handle const &target_p, ConditionalBuff const &buff_p)
	: _target(target_p)
	, _buff(buff_p)
	{}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *data_p) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}
private:
	/// @brief target of the buff
	Handle const _target;

	/// @brief buff to be applied
	ConditionalBuff const _buff;
};

} // namespace octopus


#endif
