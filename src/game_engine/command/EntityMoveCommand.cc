
#include "EntityMoveCommand.hh"

#include "step/Step.hh"
#include "entity/EntityStep.hh"

namespace game_engine
{

EntityMoveCommand::EntityMoveCommand(Entity &ent_p, std::list<Vector> waypoints_p)
	: _ent(ent_p)
	, _waypoints(waypoints_p)
{}

bool EntityMoveCommand::registerCommand(Step & step_p)
{
	// No waypoint -> terminate
	if(_waypoints.empty())
	{
		return true;
	}

	///
	/// Update waypoints based on current position
	/// Waypoint must be within ray to stop
	///
	Vector next_l = _waypoints.front();
	Vector delta_l = _ent._pos - next_l;
	// Check entity position
	while(square_length(delta_l) < _ent._ray*_ent._ray)
	{
		// pop front
		_waypoints.pop_front();
		if(_waypoints.empty())
		{
			break;
		}
		next_l = _waypoints.front();
		delta_l = _ent._pos - next_l;
	}

	// No waypoint -> terminate
	if(_waypoints.empty())
	{
		return true;
	}

	// Use next waypoint as target
	step_p.addEntityStep(new EntityStep(createEntityStep(_ent, next_l, _ent._stepSpeed)));

	return false;
}

} // namespace game_engine

