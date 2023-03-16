#include "CommandIncrementNoProgressStep.hh"

#include "command/data/AttackMoveData.hh"
#include "command/CommandData.hh"
#include "command/Commandable.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void CommandIncrementNoProgressStep::apply(State &state_p, SteppableData *) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandIncrementNoProgressStep :: apply " << this->_handle <<std::endl;
	MoveData *data_l = dynamic_cast<MoveData*>(ent_l->getFrontQueue()._data);
	if(data_l->_countSinceProgress != _old)
	{
		throw std::logic_error("Setting target had incoherent old target regarding content in state");
	}
	data_l->_countSinceProgress = _new;
}

void CommandIncrementNoProgressStep::revert(State &state_p, SteppableData *) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandIncrementNoProgressStep :: revert " << this->_handle <<std::endl;
	MoveData *data_l = dynamic_cast<MoveData*>(ent_l->getFrontQueue()._data);
	if(data_l->_countSinceProgress != _new)
	{
		throw std::logic_error("Setting target had incoherent old target regarding content in state");
	}
	data_l->_countSinceProgress = _old;
}

bool CommandIncrementNoProgressStep::isNoOp() const
{
	return _new == _old;
}

} // namespace octopus
