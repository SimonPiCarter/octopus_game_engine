
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
#include "graph/CanonicalDijkstra.hh"
#include "graph/PathManager.hh"
#include "graph/Grid.hh"
#include "graph/GridNode.hh"

namespace octopus
{

EntityMoveCommand::EntityMoveCommand(Handle const &commandHandle_p, Handle const &source_p,
		Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p, bool init_p, bool neverStop_p)
	: Command(commandHandle_p)
	, _source(source_p)
	, _finalPoint(finalPoint_p)
	, _gridStatus(gridStatus_p)
	, _waypoints(waypoints_p)
	, _init(init_p)
	, _neverStop(neverStop_p)
	, _data(_finalPoint, _gridStatus, _waypoints)
{}

void EntityMoveCommand::setFlockInformation(std::array<FlockInformation, 3> * flockInfo_p)
{
	_data._flockInfo = flockInfo_p;
}

void EntityMoveCommand::setRayTolerance(Fixed const &rayTolerance_p)
{
	_rayTolerance = rayTolerance_p;
}

Fixed const & EntityMoveCommand::getRayTolerance() const
{
	return _rayTolerance;
}

Vector const &getLeftMost(Vector const &pos1_p, Vector const &pos2_p)
{
	if(pos1_p.x > pos2_p.x)
	{
		return pos2_p;
	}
	return pos1_p;
}

Vector const &getRightMost(Vector const &pos1_p, Vector const &pos2_p)
{
	if(pos1_p.x > pos2_p.x)
	{
		return pos1_p;
	}
	return pos2_p;
}

bool losCheck(Grid const &grid_p, Vector const &pos1_p, Vector const &pos2_p)
{
	Vector const & leftMost_l = getLeftMost(pos1_p, pos2_p);
	Vector const & rightMost_l = getRightMost(pos2_p, pos1_p);

	Vector const leftToRight_l = rightMost_l - leftMost_l;

	// floor x to perform ray cast check
	int x = to_int(leftMost_l.x);
	int x_end = to_int(rightMost_l.x);
	int cur_l = 0;
	Fixed range_l(x_end-x);

	// start point is put on a integer value of x (x computed right above)
	Vector start_l = leftMost_l;
	if(range_l > 1)
	{
		start_l += leftToRight_l * (Fixed(x) - leftMost_l.x) / range_l;
	}
	do
	{
		Fixed t0 = 0;
		Fixed t1 = 1;
		if(range_l > 1)
		{
			t0 = cur_l / range_l;
			t1 = (cur_l+1) / range_l;
		}
		// y range along 1 range of x (warning lower may be higher than upper along y in case dy < 0)
		int lower_y = to_int(start_l.y + leftToRight_l.y * t0);
		int upper_y = to_int(start_l.y + leftToRight_l.y * t1);
		if(lower_y > upper_y)
		{
			std::swap(lower_y, upper_y);
		}

		// perform check on every casted node
		for(int y = std::max(0, lower_y) ; y <= upper_y && y < grid_p.getSizeY()*grid_p.getStepY() - 1e-3 ; ++ y)
		{
			if(!grid_p.getNode(Vector(x+cur_l, y))->isFree())
			{
				return false;
			}
		}

		++cur_l;
	} while(cur_l < range_l && x+cur_l < grid_p.getSizeX()*grid_p.getStepX() - 1e-3);

	return true;
}

bool EntityMoveCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const
{
	MoveData const *data_l = dynamic_cast<MoveData const *>(data_p);
	std::list<Vector> const &waypoints_l =  data_l->_waypoints;
	// No waypoint -> terminate
	if(waypoints_l.empty())
	{
		Logger::getNormal() << "no waypoint (entry)" << std::endl;
		return true;
	}

	Entity const * ent_l = state_p.getEntity(_source);

	///
	/// Update waypoints based on current position
	/// Waypoint must be within two steps
	///
	Vector next_l = data_l->_finalPoint;

	bool los_l = data_l->_los;
	// line check every several steps
	if(data_l->_stepSinceUpdate % 50 == 0 || PathManager::ForceLoSCheck)
	{
		// reset step since update count (do not update grid status (not used anyway))
		step_p.addSteppable(new CommandMoveUpdateStep(_handleCommand, data_l->_stepSinceUpdate, data_l->_gridStatus, data_l->_gridStatus));
		// perform a line check
		los_l = losCheck(state_p.getPathGrid(), ent_l->_pos, data_l->_finalPoint);
		Logger::getNormal() << "los check returned " << los_l << std::endl;
		step_p.addSteppable(new CommandMoveLosStep(_handleCommand, data_l->_los, los_l));
	}
	// increment step since update (everytime otherwise will loop reseting)
	step_p.addSteppable(new CommandMoveStepSinceUpdateIncrementStep(_handleCommand));

	// query field if no los
	if(!los_l)
	{
		pathManager_p.queryFlowField(to_int(data_l->_finalPoint.x), to_int(data_l->_finalPoint.y));
		FlowField const * field_l = pathManager_p.getFlowField(to_int(data_l->_finalPoint.x), to_int(data_l->_finalPoint.y));
		if(field_l )
		{
			// direction directly on the square
			Vector unitDir_l = unitary_direction(ent_l->_pos.x, ent_l->_pos.y, *field_l);
			if(unitDir_l.x > 1e-5 || unitDir_l.x < -1e-5 || unitDir_l.y > 1e-5 || unitDir_l.y < -1e-5)
			{
				next_l = ent_l->_pos + direction(ent_l->_pos.x, ent_l->_pos.y, *field_l);
			}
		}
	}

	Vector delta_l = ent_l->_pos - data_l->_finalPoint;
	Fixed tol_l = Fixed(100000, true)+_rayTolerance;
	if(data_l->_flockInfo)
	{
		Fixed newTol_l = ent_l->_model._ray + ent_l->_model._ray*data_l->_flockInfo->at(state_p._id).sqrtQtyReached*Fixed(1100000, true);
		tol_l = std::max(tol_l, std::max(ent_l->_model._ray, newTol_l));
	}
	// No more waypoint -> terminate
	if(square_length(delta_l) < tol_l*tol_l)
	{
		Logger::getNormal() << "no waypoint" << std::endl;
		step_p.addSteppable(new CommandUpdateFlockingReached(_handleCommand));
		return true;
	}

	// skip this if we never want to stop
	if(!_neverStop)
	{
		// add step to increment count since progress
		step_p.addSteppable(new CommandIncrementNoProgressStep(_handleCommand, data_l->_countSinceProgress, data_l->_countSinceProgress+1));

		// check for progress
		Fixed sqLastDiff_l = square_length(ent_l->_pos - data_l->_lastPos);
		// progress => update position and reset count
		if(sqLastDiff_l >= 0.5)
		{
			step_p.addSteppable(new CommandUpdateLastPosStep(_handleCommand, _source, data_l->_lastPos));
			// reset no progress count
			step_p.addSteppable(new CommandIncrementNoProgressStep(_handleCommand, data_l->_countSinceProgress+1, 0));
		}
		// add step to record last position
		// If no progress for too long we stop
		// skip this if we never want to stop
		else if(data_l->_countSinceProgress == 500)
		{
			// no progress
			if(sqLastDiff_l < 0.5)
			{
				return true;
			}
			step_p.addSteppable(new CommandUpdateLastPosStep(_handleCommand, _source, data_l->_lastPos));
			// reset no progress count
			step_p.addSteppable(new CommandIncrementNoProgressStep(_handleCommand, data_l->_countSinceProgress+1, 0));
		}
	}

	Logger::getNormal() << "Adding move step orig = "<<ent_l->_pos<<" target = "<<next_l<<" step speed = " << ent_l->getStepSpeed() << std::endl;

	// Use next waypoint as target
	step_p.addEntityMoveStep(new EntityMoveStep(createEntityMoveStep(*ent_l, next_l, ent_l->getStepSpeed())));

	return false;
}

} // namespace octopus

