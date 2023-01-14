#include "CommandQueueStep.hh"

#include "command/CommandQueue.hh"
#include "command/Commandable.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void CommandSpawnStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandSpawnStep :: apply " << this->_handle <<std::endl;
	if(_queueLast)
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
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandSpawnStep :: revert " << this->_handle <<std::endl;
	if(_queueLast)
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
	Logger::getDebug() << "CommandSpawnStep :: apply " << this->_handle <<std::endl;
	ent_l->getQueue().nextCommand();
}

void CommandNextStep::revert(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandSpawnStep :: revert " << this->_handle <<std::endl;
	ent_l->getQueue().prevCommand();
}

} // namespace octopus
