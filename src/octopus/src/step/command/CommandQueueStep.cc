#include "CommandQueueStep.hh"

#include "command/Command.hh"
#include "command/CommandQueue.hh"
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
	if(_cmd->isQueued())
	{
		ent_l->getQueue().queueCommandLast(_cmd);
	}
	else
	{
		ent_l->getQueue().queueCommand(_cmd);
	}
}

void CommandSpawnStep::revert(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_cmd->getHandleCommand());
	Logger::getDebug() << "CommandSpawnStep :: revert " << _cmd->getHandleCommand() <<std::endl;
	if(_cmd->isQueued())
	{
		ent_l->getQueue().unqueueCommandLast(_cmd);
	}
	else
	{
		ent_l->getQueue().unqueueCommand(_cmd);
	}
}

void CommandNextStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandNextStep :: apply " << this->_handle <<std::endl;
	ent_l->getQueue().nextCommand();
}

void CommandNextStep::revert(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandNextStep :: revert " << this->_handle <<std::endl;
	ent_l->getQueue().prevCommand();
}

void CommandUpdateLastIdStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandUpdateLastIdStep :: apply " << this->_handle <<std::endl;
	ent_l->setIdLast(_newIdLast);
}

void CommandUpdateLastIdStep::revert(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandUpdateLastIdStep :: revert " << this->_handle <<std::endl;
	ent_l->setIdLast(_oldIdLast);
}

} // namespace octopus
