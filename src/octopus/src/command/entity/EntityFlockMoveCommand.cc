
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
	: Command(handles_p.empty()?Handle():*handles_p.begin())
	, _handles(handles_p)
	, _finalPoint(finalPoint_p)
	, _attackMove(attackMove_p)
	, _neverStop(neverStop_p)
{}

void EntityFlockMoveCommand::setRayTolerance(Fixed const &rayTolerance_p)
{
	_rayTolerance = rayTolerance_p;
}

Fixed const & EntityFlockMoveCommand::getRayTolerance() const
{
	return _rayTolerance;
}

void EntityFlockMoveCommand::registerCommand(Step &step_p, State const &state_p)
{
	// just store this command
	step_p.addSteppable(state_p, new CommandStorageStep(this));

	unsigned long player_l = state_p.getEntity(_handleCommand)->_player;

	// create all move step command and add them
	for(Handle const handle_l : _handles)
	{
        const octopus::Entity * cur_l = state_p.getEntity(handle_l);
		bool isStatic_l = cur_l->_model._isStatic;

		if(isStatic_l)
        {
            continue;
        }

		// skip entity if not same player than first one
		if(player_l != cur_l->_player)
		{
			continue;
		}

		if(_attackMove)
		{
			EntityAttackMoveCommand * cmd_l = new EntityAttackMoveCommand(handle_l, handle_l, _finalPoint, 0, {_finalPoint}, true, _neverStop);
			cmd_l->setFlockInformation(&_flockInfo);
			cmd_l->setQueued(isQueued());
			cmd_l->setStepOfRegristration(getStepOfRegistration());
			step_p.addSteppable(state_p, new CommandSpawnStep(cmd_l));
		}
		else
		{
			EntityMoveCommand * cmd_l = new EntityMoveCommand(handle_l, handle_l, _finalPoint, 0, {_finalPoint}, true, _neverStop);
			cmd_l->setRayTolerance(_rayTolerance);
			cmd_l->setFlockInformation(&_flockInfo);
			cmd_l->setQueued(isQueued());
			cmd_l->setStepOfRegristration(getStepOfRegistration());
			step_p.addSteppable(state_p, new CommandSpawnStep(cmd_l));
		}
	}
}

} // namespace octopus

