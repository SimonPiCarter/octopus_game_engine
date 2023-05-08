#ifndef __CommandQueueStep__
#define __CommandQueueStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"

#include "command/CommandQueue.hh"

#include <cstddef>

namespace octopus
{
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

/// @brief data used to restore command removed
struct CommandSpawnStepData: public SteppableData
{
	CommandSpawnStepData() {}
	CommandSpawnStepData(std::list<CommandBundle> const &bundles_p) : bundles(bundles_p) {}
	std::list<CommandBundle> bundles;
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

	virtual SteppableData * newData(State const &state_p);

	Command * getCmd() const { return _cmd; }

private:
	Command * const _cmd;
};

/// @brief data used to restore command removed
struct CommandNextStepData: public SteppableData
{
	CommandNextStepData() {}
	CommandNextStepData(CommandBundle const &bundle_p) : bundle(bundle_p) {}
	CommandBundle bundle;
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

	virtual SteppableData * newData(State const &state_p);

private:
	Handle const _handle {0};
};

class CommandUpdateLastIdStep : public Steppable
{
public:
	CommandUpdateLastIdStep(Handle const &handle_p, CommandBundle const &old_p, CommandBundle const &new_p, bool hasLast_p)
		: _handle(handle_p), _old(old_p), _new(new_p), _hasLast(hasLast_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override { return false; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

private:
	Handle const _handle {0};
	CommandBundle _old;
	CommandBundle _new;
	bool _hasLast;
};


}

#endif
