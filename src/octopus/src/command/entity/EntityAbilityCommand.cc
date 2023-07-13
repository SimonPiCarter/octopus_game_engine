
#include "EntityAbilityCommand.hh"

#include "logger/Logger.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/EntityUpdateReloadAbilityStep.hh"

namespace octopus
{

EntityAbilityCommand::EntityAbilityCommand(Handle const &commandHandle_p, Handle const &target_p, Vector const &pointTarget_p, std::string const &id_p)
	: Command(commandHandle_p)
    , _target(target_p)
    , _pointTarget(pointTarget_p)
    , _id(id_p)
{}

void EntityAbilityCommand::registerCommand(Step & step_p, State const &state_p)
{
	Entity const * ent_l = state_p.getEntity(_handleCommand);

    Ability const & ability_l = getAbility(ent_l->_model, _id);
    // check reload time
    unsigned long reload_l = getReloadAbilityTime(*ent_l, ability_l._reloadKey, getMinReloadTime(ent_l->_model, ability_l._reloadKey));
    if(reload_l >= getMinReloadTime(ent_l->_model, ability_l._reloadKey)
    && ability_l._checker(step_p, state_p, _handleCommand, _target, _pointTarget)
    && step_p.checkAbilityNotAlreadyRegistered(_handleCommand, ability_l._reloadKey))
    {
        // reset reload time
        step_p.addSteppable(new EntityUpdateReloadAbilityStep(_handleCommand, ability_l._reloadKey, reload_l, 0));
        // spaw command
	    step_p.addSteppable(new CommandSpawnStep(this));
    }
}

bool EntityAbilityCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *, CommandContext &) const
{
	Logger::getDebug() << "EntityAbilityCommand:: apply Command "<<_target <<std::endl;

	Entity const * ent_l = state_p.getEntity(_handleCommand);
    getAbility(ent_l->_model, _id)._runnable(step_p, state_p, _handleCommand, _target, _pointTarget);

    return true;
}

} // namespace octopus

