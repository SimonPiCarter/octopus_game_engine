#include "CommandSubAttackStep.hh"

#include "command/data/AttackMoveData.hh"
#include "command/CommandData.hh"
#include "command/Commandable.hh"
#include "command/entity/EntityAttackCommand.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void CommandAddSubCommandStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandAddSubCommandStep :: apply " << this->_handle <<std::endl;
	AttackMoveData *data_l = dynamic_cast<AttackMoveData *>(getData(ent_l->getFrontQueue()._var));

	if(data_l->_hasSubAttackCommand || data_l->_hasSubBuffCommand)
	{
		throw std::logic_error("Cannot add a sub command when one is already set up");
	}
	if(std::holds_alternative<EntityAttackCommand>(_var))
	{
		data_l->_subAttackCommand = std::get<EntityAttackCommand>(_var);
		data_l->_hasSubAttackCommand = true;
	}
	if(std::holds_alternative<EntityBuffCommand>(_var))
	{
		data_l->_subBuffCommand = std::get<EntityBuffCommand>(_var);
		data_l->_hasSubBuffCommand = true;
	}
}

void CommandAddSubCommandStep::revert(State &state_p, SteppableData const *) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandAddSubCommandStep :: revert " << this->_handle <<std::endl;
	AttackMoveData *data_l = dynamic_cast<AttackMoveData *>(getData(ent_l->getFrontQueue()._var));
	if(!data_l->_hasSubAttackCommand && !data_l->_hasSubBuffCommand)
	{
		throw std::logic_error("Cannot remove a sub command when the one set up is not coherent with this step");
	}
	data_l->_hasSubAttackCommand = false;
	data_l->_hasSubBuffCommand = false;
}

bool CommandAddSubCommandStep::isNoOp() const
{
	return false;
}

void CommandDelSubCommandStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDelSubCommandStep :: apply " << this->_handle <<std::endl;
	AttackMoveData *data_l = dynamic_cast<AttackMoveData *>(getData(ent_l->getFrontQueue()._var));
	data_l->_hasSubAttackCommand = false;
	data_l->_hasSubBuffCommand = false;
}

void CommandDelSubCommandStep::revert(State &state_p, SteppableData const *) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDelSubCommandStep :: revert " << this->_handle <<std::endl;
	AttackMoveData *data_l = dynamic_cast<AttackMoveData *>(getData(ent_l->getFrontQueue()._var));
	if(data_l->_hasSubAttackCommand || data_l->_hasSubBuffCommand)
	{
		throw std::logic_error("Cannot add a sub command when one is already set up");
	}
	if(std::holds_alternative<EntityAttackCommand>(_var))
	{
		data_l->_subAttackCommand = std::get<EntityAttackCommand>(_var);
		data_l->_hasSubAttackCommand = true;
	}
	if(std::holds_alternative<EntityBuffCommand>(_var))
	{
		data_l->_subBuffCommand = std::get<EntityBuffCommand>(_var);
		data_l->_hasSubBuffCommand = true;
	}
}

bool CommandDelSubCommandStep::isNoOp() const
{
	return false;
}

} // namespace octopus
