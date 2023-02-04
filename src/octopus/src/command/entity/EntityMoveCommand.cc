
#include "EntityMoveCommand.hh"

#include "command/data/AttackMoveData.hh"
#include "logger/Logger.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/command/CommandDataWaypointStep.hh"
#include "step/command/CommandIncrementNoProgressStep.hh"
#include "step/command/CommandUpdateLastPosStep.hh"
#include "step/command/CommandMoveUpdateStep.hh"
#include "step/entity/EntityMoveStep.hh"

namespace octopus
{

EntityMoveCommand::EntityMoveCommand(Handle const &commandHandle_p, Handle const &source_p,
		Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p, bool init_p)
	: Command(commandHandle_p)
	, _source(source_p)
	, _finalPoint(finalPoint_p)
	, _gridStatus(gridStatus_p)
	, _waypoints(waypoints_p)
	, _init(init_p)
{}

bool EntityMoveCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *data_p) const
{
	MoveData const &data_l = *static_cast<MoveData const *>(data_p);
	std::list<Vector> const &waypoints_l =  data_l._waypoints;
	// No waypoint -> terminate
	if(waypoints_l.empty())
	{
		Logger::getDebug() << "no waypoint (entry)" << std::endl;
		return true;
	}

	Entity const * ent_l = state_p.getEntity(_source);

	///
	/// Check if we need to update
	///
	unsigned long gridStatus_l = state_p.getPathGridStatus();
	if((gridStatus_l > data_l._gridStatus
	&& data_l._stepSinceUpdate > 100)
	|| (data_l._gridStatus == 0 && _init))
	{
		// compute new path
		std::list<Vector> path_l = computePath(state_p, _source, data_l._finalPoint);
		step_p.addSteppable(new CommandDataWaypointSetStep(_handleCommand, waypoints_l, path_l));
		step_p.addSteppable(new CommandMoveUpdateStep(_handleCommand, data_l._stepSinceUpdate, data_l._gridStatus, gridStatus_l));

		std::cout.clear();
		std::cout<<"recompute path"<<std::endl;
		for(Vector const &v : path_l)
		{
			std::cout<<"\t"<<v<<std::endl;
		}
		return false;
	}
	step_p.addSteppable(new CommandMoveStepSinceUpdateIncrementStep(_handleCommand));

	///
	/// Update waypoints based on current position
	/// Waypoint must be within two steps
	///
	auto it_l = waypoints_l.begin();
	Vector next_l = *it_l;
	Vector delta_l = ent_l->_pos - next_l;
	// Check entity position
	while(square_length(delta_l) < ent_l->_model._ray*ent_l->_model._ray)
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
		Logger::getDebug() << "no progress since 50 steps" << std::endl;
		return true;
	}

	Logger::getDebug() << "Adding move step orig = "<<ent_l->_pos<<" target = "<<next_l<<" step speed = " << ent_l->getStepSpeed() << std::endl;

	// Use next waypoint as target
	step_p.addEntityMoveStep(new EntityMoveStep(createEntityMoveStep(*ent_l, next_l, ent_l->getStepSpeed())));

	return false;
}

} // namespace octopus

