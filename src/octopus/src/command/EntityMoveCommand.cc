
#include "EntityMoveCommand.hh"

#include "command/data/AttackMoveData.hh"
#include "logger/Logger.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/command/CommandDataWaypointStep.hh"
#include "step/command/CommandIncrementNoProgressStep.hh"
#include "step/command/CommandUpdateLastPosStep.hh"
#include "step/entity/EntityMoveStep.hh"

namespace octopus
{

EntityMoveCommand::EntityMoveCommand(Handle const &commandHandle_p, Handle const &source_p, std::list<Vector> const &waypoints_p)
	: Command(commandHandle_p)
	, _source(source_p)
	, _waypoints(waypoints_p)
{}

bool EntityMoveCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *data_p) const
{
	AttackMoveData const &data_l = static_cast<CommandDataWithData<AttackMoveData> const *>(data_p)->_data;
	std::list<Vector> const &waypoints_l =  data_l._waypoints;
	// No waypoint -> terminate
	if(waypoints_l.empty())
	{
		Logger::getDebug() << "no waypoint (entry)" << std::endl;
		return true;
	}

	Entity const * ent_l = state_p.getEntity(_source);

	///
	/// Update waypoints based on current position
	/// Waypoint must be within ray to stop
	///
	auto it_l = waypoints_l.begin();
	Vector next_l = *it_l;
	Vector delta_l = ent_l->_pos - next_l;
	// Check entity position
	while(square_length(delta_l) < ent_l->_ray*ent_l->_ray)
	{
		// pop front
		step_p.addSteppable(new CommandDataWaypointRemoveStep(_handleCommand, *it_l));
		// go next
		++it_l;
		if(it_l == waypoints_l.end())
		{
			break;
		}
		next_l = *it_l;
		delta_l = ent_l->_pos - next_l;
	}

	// No more waypoint -> terminate
	if(it_l == waypoints_l.end())
	{
		Logger::getDebug() << "no waypoint" << std::endl;
		return true;
	}

	// check for progress
	double sqLastDiff_l = square_length(next_l - data_l._lastPos);
	double sqCurDiff_l = square_length(next_l - ent_l->_pos);
	// no progress
	if(sqLastDiff_l + 1e-3 < sqCurDiff_l)
	{
		// add step to increment count since progress
		step_p.addSteppable(new CommandIncrementNoProgressStep(_handleCommand, data_l._countSinceProgress, data_l._countSinceProgress+1));
	}
	else
	{
		// reset no progress count
		step_p.addSteppable(new CommandIncrementNoProgressStep(_handleCommand, data_l._countSinceProgress, 0));
	}
	// add step to record last position
	step_p.addSteppable(new CommandUpdateLastPosStep(_handleCommand, _source, data_l._lastPos));
	// If no progress for too long we stop
	if(data_l._countSinceProgress > 50)
	{
		Logger::getDebug() << "no progress since 25 steps" << std::endl;
		return true;
	}

	Logger::getDebug() << "Adding move step orig = "<<ent_l->_pos<<" target = "<<next_l<<" step speed = " << ent_l->_stepSpeed << std::endl;

	// Use next waypoint as target
	step_p.addEntityMoveStep(new EntityMoveStep(createEntityMoveStep(*ent_l, next_l, ent_l->_stepSpeed)));

	return false;
}

} // namespace octopus

