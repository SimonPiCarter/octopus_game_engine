#include "CommandHarvestStep.hh"


#include "command/data/HarvestMoveData.hh"
#include "command/CommandData.hh"
#include "command/Commandable.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void CommandResourceChangeStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandResourceChangeStep :: apply " << this->_handle <<std::endl;
	HarvestMoveData *data_l = dynamic_cast<HarvestMoveData*>(ent_l->getFrontQueue()._data);
	data_l->_resource = _newResource;
}
void CommandResourceChangeStep::revert(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandResourceChangeStep :: revert " << this->_handle <<std::endl;
	HarvestMoveData *data_l = dynamic_cast<HarvestMoveData*>(ent_l->getFrontQueue()._data);
	data_l->_resource = _oldResource;
}

bool CommandResourceChangeStep::isNoOp() const
{
	return _oldResource == _newResource;
}

void CommandDepositChangeStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDepositChangeStep :: apply " << this->_handle <<std::endl;
	HarvestMoveData *data_l = dynamic_cast<HarvestMoveData*>(ent_l->getFrontQueue()._data);
	data_l->_deposit = _newDeposit;
}
void CommandDepositChangeStep::revert(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDepositChangeStep :: revert " << this->_handle <<std::endl;
	HarvestMoveData *data_l = dynamic_cast<HarvestMoveData*>(ent_l->getFrontQueue()._data);
	data_l->_deposit = _oldDeposit;
}

bool CommandDepositChangeStep::isNoOp() const
{
	return _oldDeposit == _newDeposit;
}

void CommandHarvestingChangeStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandHarvestingChangeStep :: apply " << this->_handle <<std::endl;
	HarvestMoveData *data_l = dynamic_cast<HarvestMoveData*>(ent_l->getFrontQueue()._data);
	data_l->_harvesting = _new;
}
void CommandHarvestingChangeStep::revert(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandHarvestingChangeStep :: revert " << this->_handle <<std::endl;
	HarvestMoveData *data_l = dynamic_cast<HarvestMoveData*>(ent_l->getFrontQueue()._data);
	data_l->_harvesting = _old;
}

bool CommandHarvestingChangeStep::isNoOp() const
{
	return _new == _old;
}

} // namespace octopus
