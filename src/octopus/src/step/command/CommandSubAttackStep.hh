#ifndef __CommandSubAttackStep__
#define __CommandSubAttackStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "command/EntityAttackCommand.hh"

namespace octopus
{
class EntityAttackCommand;

class CommandAddSubAttackStep : public Steppable
{
public:
	/// @brief take subCommand_p ownership
	CommandAddSubAttackStep(Handle const &handle_p, EntityAttackCommand * subCommand_p)
		: _handle(handle_p) , _subCommand(subCommand_p){}
	~CommandAddSubAttackStep() { delete _subCommand; }

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override;

	Handle const _handle {0};
	EntityAttackCommand * const _subCommand;
};

class CommandDelSubAttackStep : public Steppable
{
public:
	CommandDelSubAttackStep(Handle const &handle_p, EntityAttackCommand * subCommand_p)
		: _handle(handle_p) , _subCommand(subCommand_p){}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override;

	Handle const _handle {0};
	EntityAttackCommand * const _subCommand;
};

}

#endif
