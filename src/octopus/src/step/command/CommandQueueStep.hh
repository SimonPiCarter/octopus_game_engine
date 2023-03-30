#ifndef __CommandQueueStep__
#define __CommandQueueStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"

#include <cstddef>

namespace octopus
{

class Command;

/// @brief this step is only used to store and delete properly commands
/// that wont register as usual (missing resources for example)
class CommandStorageStep : public Steppable
{
public:
	CommandStorageStep(Command *cmd_p)
		: _cmd(cmd_p) {}
	~CommandStorageStep();

	virtual void apply(State &) const override {}
	virtual void revert(State &, SteppableData const *) const override {}

	virtual bool isNoOp() const override { return _cmd == nullptr; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Command * getCmd() const { return _cmd; }

private:
	Command * const _cmd;
};

class CommandSpawnStep : public Steppable
{
public:
	CommandSpawnStep(Command *cmd_p)
		: _cmd(cmd_p) {}
	~CommandSpawnStep();

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override { return _cmd == nullptr; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Command * getCmd() const { return _cmd; }

private:
	Command * const _cmd;
};

class CommandNextStep : public Steppable
{
public:
	CommandNextStep(Handle const &handle_p)
		: _handle(handle_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override { return false; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

private:
	Handle const _handle {0};
};

class CommandUpdateLastIdStep : public Steppable
{
public:
	CommandUpdateLastIdStep(Handle const &handle_p, size_t oldIdLast_p, size_t newIdLast_p)
		: _handle(handle_p), _oldIdLast(oldIdLast_p), _newIdLast(newIdLast_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override { return _oldIdLast == _newIdLast; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

private:
	Handle const _handle {0};
	size_t const _oldIdLast {0};
	size_t const _newIdLast {0};
};


}

#endif
