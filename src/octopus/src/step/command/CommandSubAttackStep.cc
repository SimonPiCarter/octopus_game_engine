#include "CommandSubAttackStep.hh"

#include "command/data/AttackMoveData.hh"
#include "command/CommandData.hh"
#include "command/Commandable.hh"
#include "command/entity/EntityAttackCommand.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void CommandAddSubAttackStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandAddSubAttackStep :: apply " << this->_handle <<std::endl;
	AttackMoveData *data_l = dynamic_cast<AttackMoveData *>(getData(ent_l->getFrontQueue()._var));

	if(data_l->_hasSubAttackCommand)
	{
		throw std::logic_error("Cannot add a sub attack command when one is already set up");
	}
	data_l->_hasSubAttackCommand = true;
	data_l->_subAttackCommand = EntityAttackCommand(_handle, _source, _target, false);
}

void CommandAddSubAttackStep::revert(State &state_p, SteppableData const *) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandAddSubAttackStep :: revert " << this->_handle <<std::endl;
	AttackMoveData *data_l = dynamic_cast<AttackMoveData *>(getData(ent_l->getFrontQueue()._var));
	if(!data_l->_hasSubAttackCommand)
	{
		throw std::logic_error("Cannot remove a sub attack command when the one set up is not coherent with this step");
	}
	data_l->_hasSubAttackCommand = false;
}

bool CommandAddSubAttackStep::isNoOp() const
{
	return false;
}

void CommandDelSubAttackStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDelSubAttackStep :: apply " << this->_handle <<std::endl;
	AttackMoveData *data_l = dynamic_cast<AttackMoveData *>(getData(ent_l->getFrontQueue()._var));
	data_l->_hasSubAttackCommand = false;
}

void CommandDelSubAttackStep::revert(State &state_p, SteppableData const *) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDelSubAttackStep :: revert " << this->_handle <<std::endl;
	AttackMoveData *data_l = dynamic_cast<AttackMoveData *>(getData(ent_l->getFrontQueue()._var));
	if(data_l->_hasSubAttackCommand)
	{
		throw std::logic_error("Cannot add a sub attack command when one is already set up");
	}
	data_l->_subAttackCommand = EntityAttackCommand(_handle, _source, _target, false);
	data_l->_hasSubAttackCommand = true;
}

bool CommandDelSubAttackStep::isNoOp() const
{
	return false;
}

} // namespace octopus
