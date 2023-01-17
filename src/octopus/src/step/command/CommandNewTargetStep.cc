#include "CommandNewTargetStep.hh"

#include "command/data/AttackMoveData.hh"
#include "command/CommandData.hh"
#include "command/Commandable.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void CommandNewTargetStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandNewTargetStep :: apply " << this->_handle <<std::endl;
	AttackMoveData *data_l = dynamic_cast<AttackMoveData *>(ent_l->getFrontQueue()._data);
	if(data_l->_target != _oldTarget)
	{
		throw std::logic_error("Setting target had incoherent old target regarding content in state");
	}
	data_l->_target = _newTarget;
}

void CommandNewTargetStep::revert(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandNewTargetStep :: revert " << this->_handle <<std::endl;
	AttackMoveData *data_l = dynamic_cast<AttackMoveData *>(ent_l->getFrontQueue()._data);
	if(data_l->_target != _newTarget)
	{
		throw std::logic_error("Setting target had incoherent old target regarding content in state");
	}
	data_l->_target = _oldTarget;
}

bool CommandNewTargetStep::isNoOp() const
{
	return _newTarget == _oldTarget;
}

} // namespace octopus
