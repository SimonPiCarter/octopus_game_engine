#ifndef __CommandUpdateLastPosStep__
#define __CommandUpdateLastPosStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
class EntityAttackCommand;

class CommandUpdateLastPosStep : public Steppable
{
public:
	CommandUpdateLastPosStep(Handle const &handle_p, Handle const &entity_p, Vector const &oldPos_p)
		: _handle(handle_p) , _entity(entity_p), _oldPos(oldPos_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	/// @brief command
	Handle const _handle {0};

	Handle const _entity;
	Vector const _oldPos;
};

}

#endif
