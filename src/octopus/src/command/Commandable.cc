#include "Commandable.hh"
#include "command/Command.hh"

#include "step/command/CommandQueueStep.hh"
#include "step/Step.hh"

namespace octopus
{
Commandable::Commandable()
{}

Commandable::~Commandable()
{}

void Commandable::enqueue(Command * cmd_p, bool enqueue_p)
{
	if(!enqueue_p)
	{
		_queue.queueCommand(getVarFromCommand(cmd_p));
	}
	else
	{
		_queue.queueCommandLast(getVarFromCommand(cmd_p));
	}
}

bool Commandable::isActive() const
{
	return true;
}

void Commandable::runCommands(Step & step_p, State const &state_p, CommandContext &commandContext_p)
{
	if(!_queue.hasCommand())
	{
		return;
	}
	std::list<CommandBundle>::const_iterator it_l = _queue.getList().begin();
	CommandBundle const &bundle_l = *it_l;
	if(_hasLastCommand && _lastCommand._idx != bundle_l._idx)
	{
		cleanUp(_lastCommand._var, step_p, state_p, getData(_lastCommand._var));
	}
	// while we have commands and the front one is over go on
	while(it_l != _queue.getList().cend()
	   && applyCommand(it_l->_var, step_p, state_p, getData(it_l->_var), commandContext_p))
	{
		// clean up
		cleanUp(it_l->_var, step_p, state_p, getData(it_l->_var));
		step_p.addSteppable(new CommandNextStep(_commandableHandle));
		++it_l;
	}
	if(it_l != _queue.getList().cend())
	{
		step_p.addSteppable(new CommandUpdateLastIdStep(_commandableHandle, _lastCommand, *it_l, _hasLastCommand));
	}
}

CommandBundle & Commandable::getFrontQueue()
{
	return _queue.getFrontCommand();
}

CommandBundle const & Commandable::getFrontQueue() const
{
	return _queue.getFrontCommand();
}

CommandQueue &Commandable::getQueue()
{
	return _queue;
}

const CommandQueue &Commandable::getQueue() const
{
	return _queue;
}

void Commandable::setLastCommand(CommandBundle const& bundle_p)
{
	_lastCommand = bundle_p;
}

void Commandable::setHasLastCommand(bool hasLastCommand_p)
{
	_hasLastCommand = hasLastCommand_p;
}

} // namespace octopus

