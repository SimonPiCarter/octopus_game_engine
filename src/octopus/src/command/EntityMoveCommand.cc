
#include "EntityMoveCommand.hh"

#include "logger/Logger.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/entity/EntityMoveStep.hh"

namespace octopus
{

EntityMoveCommand::EntityMoveCommand(Handle const &commandHandle_p, Handle const &source_p, std::list<Vector> waypoints_p)
	: CommandWithData(commandHandle_p, waypoints_p)
	, _source(source_p)
{}

bool EntityMoveCommand::applyCommand(Step & step_p, State const &state_p)
{
	// No waypoint -> terminate
	if(getMetaData(state_p).empty())
	{
		Logger::getDebug() << "no waypoint (entry)" << std::endl;
		return true;
	}

	Entity const * ent_l = state_p.getEntity(_source);

	///
	/// Update waypoints based on current position
	/// Waypoint must be within ray to stop
	///
	Vector next_l = getMetaData(state_p).front();
	Vector delta_l = ent_l->_pos - next_l;
	// Check entity position
	while(square_length(delta_l) < ent_l->_ray*ent_l->_ray)
	{
		// pop front
		getMetaData(state_p).pop_front();
		if(getMetaData(state_p).empty())
		{
			break;
		}
		next_l = getMetaData(state_p).front();
		delta_l = ent_l->_pos - next_l;
	}

	// No waypoint -> terminate
	if(getMetaData(state_p).empty())
	{
		Logger::getDebug() << "no waypoint" << std::endl;
		return true;
	}
	Logger::getDebug() << "Adding move step orig = "<<ent_l->_pos<<" target = "<<next_l<<" step speed = " << ent_l->_stepSpeed << std::endl;

	// Use next waypoint as target
	step_p.addEntityMoveStep(new EntityMoveStep(createEntityMoveStep(*ent_l, next_l, ent_l->_stepSpeed)));

	return false;
}

} // namespace octopus

