
#include "EntityMoveCommand.hh"

#include "state/State.hh"
#include "step/Step.hh"
#include "step/entity/EntityStep.hh"

namespace octopus
{

EntityMoveCommand::EntityMoveCommand(Handle const &handle_p, std::list<Vector> waypoints_p)
	: _handle(handle_p)
	, _waypoints(waypoints_p)
{}

bool EntityMoveCommand::registerCommand(Step & step_p, State const &state_p)
{
	// No waypoint -> terminate
	if(_waypoints.empty())
	{
		return true;
	}

	Entity const * ent_l = state_p.getEntity(_handle);

	///
	/// Update waypoints based on current position
	/// Waypoint must be within ray to stop
	///
	Vector next_l = _waypoints.front();
	Vector delta_l = ent_l->_pos - next_l;
	// Check entity position
	while(square_length(delta_l) < ent_l->_ray*ent_l->_ray)
	{
		// pop front
		_waypoints.pop_front();
		if(_waypoints.empty())
		{
			break;
		}
		next_l = _waypoints.front();
		delta_l = ent_l->_pos - next_l;
	}

	// No waypoint -> terminate
	if(_waypoints.empty())
	{
		return true;
	}

	// Use next waypoint as target
	step_p.addEntityStep(new EntityStep(createEntityStep(*ent_l, next_l, ent_l->_stepSpeed)));

	return false;
}

} // namespace octopus

