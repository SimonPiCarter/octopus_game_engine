#include "CommandDataWaypointStep.hh"

#include <list>

#include "command/data/AttackMoveData.hh"
#include "command/CommandData.hh"
#include "command/Commandable.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void CommandDataWaypointAddStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDataWaypointAddStep :: apply " << this->_handle <<std::endl;
	CommandDataWithData<AttackMoveData> *data_l = dynamic_cast<CommandDataWithData<AttackMoveData> *>(ent_l->getFrontQueue()._data);
	data_l->_data._waypoints.push_front(_waypoint);
}

void CommandDataWaypointAddStep::revert(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDataWaypointAddStep :: revert " << this->_handle <<std::endl;
	CommandDataWithData<AttackMoveData> *data_l = dynamic_cast<CommandDataWithData<AttackMoveData> *>(ent_l->getFrontQueue()._data);
	data_l->_data._waypoints.pop_front();
}

bool CommandDataWaypointAddStep::isNoOp() const
{
	return false;
}

void CommandDataWaypointRemoveStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDataWaypointRemoveStep :: apply " << this->_handle <<std::endl;
	CommandDataWithData<AttackMoveData> *data_l = dynamic_cast<CommandDataWithData<AttackMoveData> *>(ent_l->getFrontQueue()._data);
	data_l->_data._waypoints.pop_front();
}

void CommandDataWaypointRemoveStep::revert(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDataWaypointRemoveStep :: revert " << this->_handle <<std::endl;
	CommandDataWithData<AttackMoveData> *data_l = dynamic_cast<CommandDataWithData<AttackMoveData> *>(ent_l->getFrontQueue()._data);
	data_l->_data._waypoints.push_front(_waypoint);
}

bool CommandDataWaypointRemoveStep::isNoOp() const
{
	return false;
}

} // namespace octopus
