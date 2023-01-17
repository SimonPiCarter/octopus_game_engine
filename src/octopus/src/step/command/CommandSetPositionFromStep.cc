#include "CommandSetPositionFromStep.hh"

#include "command/data/AttackMoveData.hh"
#include "command/CommandData.hh"
#include "command/Commandable.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void CommandSetPositionFromStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandSetPositionFromStep :: apply " << this->_handle <<std::endl;
	AttackMoveData *data_l = dynamic_cast<AttackMoveData *>(ent_l->getFrontQueue()._data);
	if(data_l->_positionFromAttack != _oldPos)
	{
		throw std::logic_error("Setting positionFrom had incoherent old position regarding content in state");
	}
	data_l->_positionFromAttack = _newPos;
}

void CommandSetPositionFromStep::revert(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandSetPositionFromStep :: revert " << this->_handle <<std::endl;
	AttackMoveData *data_l = dynamic_cast<AttackMoveData *>(ent_l->getFrontQueue()._data);
	if(data_l->_positionFromAttack != _newPos)
	{
		throw std::logic_error("Setting positionFrom had incoherent old position regarding content in state");
	}
	data_l->_positionFromAttack = _oldPos;
}

bool CommandSetPositionFromStep::isNoOp() const
{
	return _newPos == _oldPos;
}

} // namespace octopus
