#ifndef __CommandSubAttackStep__
#define __CommandSubAttackStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "command/entity/EntityAttackCommand.hh"
#include "command/CommandVar.hh"

namespace octopus
{
class EntityAttackCommand;

class CommandAddSubCommandStep : public Steppable
{
public:
	/// @brief take subCommand_p ownership
	CommandAddSubCommandStep(Handle const &handle_p, CommandVar const &var_p)
		: _handle(handle_p) , _var(var_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle const _handle {0};
	CommandVar const _var;
};

class CommandDelSubCommandStep : public Steppable
{
public:
	CommandDelSubCommandStep(Handle const &handle_p, CommandVar const &var_p)
		: _handle(handle_p) , _var(var_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle const _handle {0};
	CommandVar const _var;
};

}

#endif
