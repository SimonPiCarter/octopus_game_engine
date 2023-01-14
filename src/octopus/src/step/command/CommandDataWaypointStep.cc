#include "CommandDataWaypointStep.hh"

#include <list>

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
	CommandDataWithData<std::list<Vector>> *data_l = dynamic_cast<CommandDataWithData<std::list<Vector>> *>(ent_l->getFrontQueue()._data);
	data_l->_data.push_front(_waypoint);
}

void CommandDataWaypointAddStep::revert(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDataWaypointAddStep :: revert " << this->_handle <<std::endl;
	CommandDataWithData<std::list<Vector>> *data_l = dynamic_cast<CommandDataWithData<std::list<Vector>> *>(ent_l->getFrontQueue()._data);
	data_l->_data.pop_front();
}

bool CommandDataWaypointAddStep::isNoOp() const
{
	return false;
}

void CommandDataWaypointRemoveStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDataWaypointRemoveStep :: apply " << this->_handle <<std::endl;
	CommandDataWithData<std::list<Vector>> *data_l = dynamic_cast<CommandDataWithData<std::list<Vector>> *>(ent_l->getFrontQueue()._data);
	data_l->_data.pop_front();
}

void CommandDataWaypointRemoveStep::revert(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDataWaypointRemoveStep :: revert " << this->_handle <<std::endl;
	CommandDataWithData<std::list<Vector>> *data_l = dynamic_cast<CommandDataWithData<std::list<Vector>> *>(ent_l->getFrontQueue()._data);
	data_l->_data.push_front(_waypoint);
}

bool CommandDataWaypointRemoveStep::isNoOp() const
{
	return false;
}

} // namespace octopus
