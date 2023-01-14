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
	CommandSpawnStep(Handle const &handle_p, Command *cmd_p, bool queueLast_p)
		: _handle(handle_p) , _cmd(cmd_p) , _queueLast(queueLast_p) {}
	~CommandSpawnStep();

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override { return _cmd == nullptr; }

private:
	Handle _handle {0};
	Command * const _cmd;
	bool _queueLast;
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
