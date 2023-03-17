#ifndef __CommandSubAttackStep__
#define __CommandSubAttackStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "command/entity/EntityAttackCommand.hh"

namespace octopus
{
class EntityAttackCommand;

struct CommandAddSubAttackStepData : public SteppableData
{
	/// @brief store the command generated for assert purpose
	EntityAttackCommand const * _cmd {nullptr};
};

class CommandAddSubAttackStep : public Steppable
{
public:
	/// @brief take subCommand_p ownership
	CommandAddSubAttackStep(Handle const &handle_p, Handle const &source_p, Handle const &target_p)
		: _handle(handle_p) , _source(source_p) , _target(target_p){}

	virtual void apply(State &state_p, SteppableData *data_p) const override;
	virtual void revert(State &state_p, SteppableData *data_p) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	/// @brief create an optional new SteppableData
	/// @return nullptr if not required
	virtual SteppableData * newData() const { return new CommandAddSubAttackStepData(); }

	Handle const _handle {0};
	Handle const _source {0};
	Handle const _target {0};
};

class CommandDelSubAttackStep : public Steppable
{
public:
	CommandDelSubAttackStep(Handle const &handle_p, Handle const &source_p, Handle const &target_p)
		: _handle(handle_p) , _source(source_p), _target(target_p) {}

	virtual void apply(State &state_p, SteppableData *) const override;
	virtual void revert(State &state_p, SteppableData *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle const _handle {0};
	Handle const _source {0};
	Handle const _target {0};
};

}

#endif
