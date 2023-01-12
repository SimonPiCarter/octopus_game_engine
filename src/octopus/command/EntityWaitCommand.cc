
#include "EntityWaitCommand.hh"

#include "step/Step.hh"
#include "entity/EntityStep.hh"

namespace octopus
{

EntityWaitCommand::EntityWaitCommand(Entity &ent_p)
	: _ent(ent_p)
{}

bool EntityWaitCommand::registerCommand(Step & step_p)
{
	// Use
	step_p.addEntityStep(new EntityStep(createEntityStep(_ent)));

	return false;
}

} // namespace octopus

