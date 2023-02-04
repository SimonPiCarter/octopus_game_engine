#include "CommandMoveUpdateStep.hh"

#include <list>

#include "command/data/MoveData.hh"
#include "command/CommandData.hh"
#include "command/Commandable.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void CommandMoveUpdateStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandMoveUpdateStep :: apply " << this->_handle <<std::endl;
	MoveData *data_l = dynamic_cast<MoveData*>(ent_l->getFrontQueue()._data);
	data_l->_stepSinceUpdate = 0;
	data_l->_gridStatus = _newGridStatus;
}

void CommandMoveUpdateStep::revert(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandMoveUpdateStep :: revert " << this->_handle <<std::endl;
	MoveData *data_l = dynamic_cast<MoveData*>(ent_l->getFrontQueue()._data);
	data_l->_stepSinceUpdate = _old;
	data_l->_gridStatus = _oldGridStatus;
}

bool CommandMoveUpdateStep::isNoOp() const
{
	return false;
}

void CommandMoveStepSinceUpdateIncrementStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandMoveStepSinceUpdateIncrementStep :: apply " << this->_handle <<std::endl;
	MoveData *data_l = dynamic_cast<MoveData*>(ent_l->getFrontQueue()._data);
	++data_l->_stepSinceUpdate;
}

void CommandMoveStepSinceUpdateIncrementStep::revert(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandMoveStepSinceUpdateIncrementStep :: revert " << this->_handle <<std::endl;
	MoveData *data_l = dynamic_cast<MoveData*>(ent_l->getFrontQueue()._data);
	--data_l->_stepSinceUpdate;
}

bool CommandMoveStepSinceUpdateIncrementStep::isNoOp() const
{
	return false;
}

} // namespace octopus
