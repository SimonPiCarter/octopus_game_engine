
#include "EntityFlockMoveCommand.hh"

#include "logger/Logger.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "command/entity/EntityMoveCommand.hh"
#include "step/command/CommandQueueStep.hh"

namespace octopus
{

EntityFlockMoveCommand::EntityFlockMoveCommand(std::list<Handle> const &handles_p, Vector const &finalPoint_p, bool neverStop_p)
	: Command(0)
	, _handles(handles_p)
	, _finalPoint(finalPoint_p)
	, _neverStop(neverStop_p)
{}

void EntityFlockMoveCommand::registerCommand(Step &step_p, State const &)
{
	// just store this command
	step_p.addSteppable(new CommandStorageStep(this));

	// create all move step command and add them
	for(Handle const handle_l : _handles)
	{
		EntityMoveCommand * cmd_l = new EntityMoveCommand(handle_l, handle_l, _finalPoint, 0, {_finalPoint}, true, _neverStop);
		cmd_l->setFlockInformation(&_flockInfo);
		step_p.addSteppable(new CommandSpawnStep(cmd_l));
	}
}

} // namespace octopus

