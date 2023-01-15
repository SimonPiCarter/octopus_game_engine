#ifndef __CommandSetPositionFromStep__
#define __CommandSetPositionFromStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{
class EntityAttackCommand;

class CommandSetPositionFromStep : public Steppable
{
public:
	CommandSetPositionFromStep(Handle const &handle_p, Vector const & newPos_p, Vector const & oldPos_p)
		: _handle(handle_p) , _newPos(newPos_p) , _oldPos(oldPos_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override;

	Handle const _handle {0};
	Vector const _newPos;
	Vector const _oldPos;
};

}

#endif
