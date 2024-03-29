#include "CommandDataWaypointStep.hh"

#include <list>

#include "command/data/MoveData.hh"
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
	MoveData *data_l = dynamic_cast<MoveData*>(getData(ent_l->getFrontQueue()._var));
	data_l->_waypoints.push_front(_waypoint);
}

void CommandDataWaypointAddStep::revert(State &state_p, SteppableData const *) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDataWaypointAddStep :: revert " << this->_handle <<std::endl;
	MoveData *data_l = dynamic_cast<MoveData*>(getData(ent_l->getFrontQueue()._var));
	data_l->_waypoints.pop_front();
}

bool CommandDataWaypointAddStep::isNoOp() const
{
	return false;
}

void CommandDataWaypointRemoveStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDataWaypointRemoveStep :: apply " << this->_handle <<std::endl;
	MoveData *data_l = dynamic_cast<MoveData*>(getData(ent_l->getFrontQueue()._var));
	data_l->_waypoints.pop_front();
}

void CommandDataWaypointRemoveStep::revert(State &state_p, SteppableData const *) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDataWaypointRemoveStep :: revert " << this->_handle <<std::endl;
	MoveData *data_l = dynamic_cast<MoveData*>(getData(ent_l->getFrontQueue()._var));
	data_l->_waypoints.push_front(_waypoint);
}

bool CommandDataWaypointRemoveStep::isNoOp() const
{
	return false;
}

void CommandDataWaypointSetStep::apply(State &state_p) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDataWaypointSetStep :: apply " << this->_handle <<std::endl;
	MoveData *data_l = dynamic_cast<MoveData*>(getData(ent_l->getFrontQueue()._var));
	data_l->_waypoints = _new;
	data_l->_finalPoint = _new.back();
}

void CommandDataWaypointSetStep::revert(State &state_p, SteppableData const *) const
{
	Commandable * ent_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "CommandDataWaypointSetStep :: revert " << this->_handle <<std::endl;
	MoveData *data_l = dynamic_cast<MoveData*>(getData(ent_l->getFrontQueue()._var));
	data_l->_waypoints = _old;
	data_l->_finalPoint = _old.back();
}

bool CommandDataWaypointSetStep::isNoOp() const
{
	return _old == _new;
}

} // namespace octopus
