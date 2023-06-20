
#include "EntityAbilityCommand.hh"

#include "logger/Logger.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/EntityUpdateReloadAbilityStep.hh"

namespace octopus
{

EntityAbilityCommand::EntityAbilityCommand(Handle const &commandHandle_p, Handle const &target_p, Vector const &pointTarget_p, std::string const &id_p
        , std::string const &reloadKey_p)
	: Command(commandHandle_p)
    , _target(target_p)
    , _pointTarget(pointTarget_p)
    , _id(id_p)
    , _reloadKey(reloadKey_p)
{}

void EntityAbilityCommand::registerCommand(Step & step_p, State const &state_p)
{
	Entity const * ent_l = state_p.getEntity(_handleCommand);

    // check reload time
    unsigned long reload_l = getReloadAbilityTime(*ent_l, _reloadKey, getMinReloadTime(ent_l->_model, _reloadKey));
    if(reload_l >= getMinReloadTime(ent_l->_model, _reloadKey) && step_p.checkAbilityNotAlreadyRegistered(_handleCommand, _reloadKey))
    {
        // reset reload time
        step_p.addSteppable(new EntityUpdateReloadAbilityStep(_handleCommand, _reloadKey, reload_l, 0));
        // spaw command
	    step_p.addSteppable(new CommandSpawnStep(this));
    }
}

bool EntityAbilityCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *, PathManager &) const
{
	Logger::getDebug() << "EntityAbilityCommand:: apply Command "<<_target <<std::endl;

	Entity const * ent_l = state_p.getEntity(_handleCommand);
    getAbility(ent_l->_model, _id)(step_p, state_p, _target, _pointTarget);

    return true;
}

} // namespace octopus

