#include "CommandUpdateLastPosStep.hh"

#include "command/data/AttackMoveData.hh"
#include "command/CommandData.hh"
#include "command/Commandable.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "logger/Logger.hh"

namespace octopus
{
void CommandUpdateLastPosStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandUpdateLastPosStep :: apply " << this->_handle <<std::endl;
	MoveData *data_l = dynamic_cast<MoveData *>(ent_l->getFrontQueue()._data);
	data_l->_lastPos = state_p.getEntity(_entity)->_pos;
}

void CommandUpdateLastPosStep::revert(State &state_p, SteppableData const *) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandUpdateLastPosStep :: revert " << this->_handle <<std::endl;
	MoveData *data_l = dynamic_cast<MoveData *>(ent_l->getFrontQueue()._data);
	data_l->_lastPos = _oldPos;
}

bool CommandUpdateLastPosStep::isNoOp() const
{
	return false;
}

} // namespace octopus
