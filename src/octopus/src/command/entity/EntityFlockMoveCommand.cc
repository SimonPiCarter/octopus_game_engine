
#include "EntityFlockMoveCommand.hh"

#include "logger/Logger.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "step/Step.hh"
#include "command/entity/EntityAttackMoveCommand.hh"
#include "command/entity/EntityMoveCommand.hh"
#include "step/command/CommandQueueStep.hh"

namespace octopus
{

EntityFlockMoveCommand::EntityFlockMoveCommand(std::list<Handle> const &handles_p, Vector const &finalPoint_p, bool attackMove_p, bool neverStop_p)
	: Command(0)
	, _handles(handles_p)
	, _finalPoint(finalPoint_p)
	, _attackMove(attackMove_p)
	, _neverStop(neverStop_p)
{}

void EntityFlockMoveCommand::registerCommand(Step &step_p, State const &state_p)
{
	// just store this command
	step_p.addSteppable(new CommandStorageStep(this));

	// create all move step command and add them
	for(Handle const handle_l : _handles)
	{
        const octopus::Entity * cur_l = state_p.getEntity(handle_l);
		bool isStatic_l = cur_l->_model._isStatic;

		if(isStatic_l)
        {
            continue;
        }

		if(_attackMove)
		{
			EntityAttackMoveCommand * cmd_l = new EntityAttackMoveCommand(handle_l, handle_l, _finalPoint, 0, {_finalPoint}, true, _neverStop);
			cmd_l->setFlockInformation(&_flockInfo);
			cmd_l->setQueued(isQueued());
			step_p.addSteppable(new CommandSpawnStep(cmd_l));
		}
		else
		{
			EntityMoveCommand * cmd_l = new EntityMoveCommand(handle_l, handle_l, _finalPoint, 0, {_finalPoint}, true, _neverStop);
			cmd_l->setFlockInformation(&_flockInfo);
			cmd_l->setQueued(isQueued());
			step_p.addSteppable(new CommandSpawnStep(cmd_l));
		}
	}
}

} // namespace octopus

