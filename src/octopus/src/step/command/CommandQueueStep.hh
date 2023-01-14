#ifndef __CommandQueueStep__
#define __CommandQueueStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{

class Command;

class CommandSpawnStep : public Steppable
{
public:
	CommandSpawnStep(Command *cmd_p)
		: _cmd(cmd_p) {}
	~CommandSpawnStep();

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override { return _cmd == nullptr; }

private:
	Command * const _cmd;
};

class CommandNextStep : public Steppable
{
public:
	CommandNextStep(Handle const &handle_p)
		: _handle(handle_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override { return false; }

private:
	Handle _handle {0};
};


}

#endif
