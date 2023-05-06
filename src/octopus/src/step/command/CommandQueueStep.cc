#include "CommandQueueStep.hh"

#include "command/CommandVar.hh"
#include "command/Command.hh"
#include "command/Commandable.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

CommandStorageStep::~CommandStorageStep()
{
}

CommandSpawnStep::~CommandSpawnStep()
{
}

void CommandSpawnStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(_cmd->getHandleCommand());
	Logger::getDebug() << "CommandSpawnStep :: apply " << _cmd->getHandleCommand() <<std::endl;
	ent_l->enqueue(_cmd, _cmd->isQueued());
}

void CommandSpawnStep::revert(State &state_p, SteppableData const *data_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_cmd->getHandleCommand());
	Logger::getDebug() << "CommandSpawnStep :: revert " << _cmd->getHandleCommand() <<std::endl;
	CommandSpawnStepData const *data_l = dynamic_cast<CommandSpawnStepData const *>(data_p);
	if(_cmd->isQueued())
	{
		ent_l->getQueue().unqueueCommandLast();
	}
	else
	{
		ent_l->getQueue().unqueueCommand(data_l->bundles);
	}
}

SteppableData * CommandSpawnStep::newData(State const &state_p)
{
	Commandable const * ent_l = state_p.getCommandable(this->_cmd->getHandleCommand());
	return new CommandSpawnStepData {ent_l->getQueue().getList()};
}

void CommandNextStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandNextStep :: apply " << this->_handle <<std::endl;
	ent_l->getQueue().nextCommand();
}

void CommandNextStep::revert(State &state_p, SteppableData const *data_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandNextStep :: revert " << this->_handle <<std::endl;
	CommandNextStepData const *data_l = dynamic_cast<CommandNextStepData const *>(data_p);
	ent_l->getQueue().prevCommand(data_l->bundle);
}

SteppableData * CommandNextStep::newData(State const &state_p)
{
	Commandable const * ent_l = state_p.getCommandable(this->_handle);
	return new CommandNextStepData {ent_l->getQueue().getFrontCommand()};
}

void CommandUpdateLastIdStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandUpdateLastIdStep :: apply " << this->_handle <<std::endl;
	ent_l->setLastCommand(_new);
}

void CommandUpdateLastIdStep::revert(State &state_p, SteppableData const *) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandUpdateLastIdStep :: revert " << this->_handle <<std::endl;
	ent_l->setLastCommand(_old);
}

} // namespace octopus
