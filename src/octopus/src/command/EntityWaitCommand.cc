
#include "EntityWaitCommand.hh"

#include "state/State.hh"
#include "step/Step.hh"
#include "step/entity/EntityMoveStep.hh"

namespace octopus
{

EntityWaitCommand::EntityWaitCommand(Handle const &commandHandle_p, Handle const& handle_p)
	: Command(commandHandle_p)
	, _handle(handle_p)
{}

bool EntityWaitCommand::applyCommand(Step & step_p, State const &state_p)
{
	// Use
	step_p.addEntityMoveStep(new EntityMoveStep(createEntityMoveStep(*state_p.getEntity(_handle))));

	return false;
}

} // namespace octopus

