#include "CommandSubAttackStep.hh"

#include "command/data/AttackMoveData.hh"
#include "command/CommandData.hh"
#include "command/Commandable.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void CommandAddSubAttackStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandAddSubAttackStep :: apply " << this->_handle <<std::endl;
	AttackMoveData *data_l = dynamic_cast<AttackMoveData *>(ent_l->getFrontQueue()._data);
	if(data_l->_subAttackCommand)
	{
		throw std::logic_error("Cannot add a sub attack command when one is already set up");
	}
	data_l->_subAttackCommand = _subCommand;
}

void CommandAddSubAttackStep::revert(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandAddSubAttackStep :: revert " << this->_handle <<std::endl;
	AttackMoveData *data_l = dynamic_cast<AttackMoveData *>(ent_l->getFrontQueue()._data);
	if(data_l->_subAttackCommand != _subCommand)
	{
		throw std::logic_error("Cannot remove a sub attack command when the one set up is not coherent with this step");
	}
	data_l->_subAttackCommand = nullptr;
}

bool CommandAddSubAttackStep::isNoOp() const
{
	return _subCommand == nullptr;
}

void CommandDelSubAttackStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDelSubAttackStep :: apply " << this->_handle <<std::endl;
	AttackMoveData *data_l = dynamic_cast<AttackMoveData *>(ent_l->getFrontQueue()._data);
	if(data_l->_subAttackCommand != _subCommand)
	{
		throw std::logic_error("Cannot remove a sub attack command when the one set up is not coherent with this step");
	}
	data_l->_subAttackCommand = nullptr;
}

void CommandDelSubAttackStep::revert(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDelSubAttackStep :: revert " << this->_handle <<std::endl;
	AttackMoveData *data_l = dynamic_cast<AttackMoveData *>(ent_l->getFrontQueue()._data);
	if(data_l->_subAttackCommand)
	{
		throw std::logic_error("Cannot add a sub attack command when one is already set up");
	}
	data_l->_subAttackCommand = _subCommand;
}

bool CommandDelSubAttackStep::isNoOp() const
{
	return _subCommand == nullptr;
}

} // namespace octopus
