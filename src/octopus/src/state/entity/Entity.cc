#include "Entity.hh"

#include "logger/Logger.hh"

#include "command/entity/EntityAttackCommand.hh"
#include "state/State.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/EntityUpdateWaitingStep.hh"
#include "step/Step.hh"

namespace octopus
{

Entity::Entity(Vector const &pos_p, bool frozen_p, EntityModel const &model_p)
	: Commandable()
	, _pos(pos_p)
	, _frozen(frozen_p)
	, _hp(model_p._hpMax)
	, _reload(model_p._fullReload)
	, _model(model_p)
{}

void Entity::runCommands(Step & step_p, State const &state_p)
{
	// If no command we check for target if we have damage
	if(!getQueue().hasCurrentCommand() && _model._damage > 0)
	{
		Logger::getDebug() << " Entity::runCommands :: no command"<< std::endl;
		if(_waiting >= 50)
		{
			// reset waiting time
			step_p.addSteppable(new EntityUpdateWaitingStep(_handle, _waiting, 0));
			Entity const * target_l = lookUpNewTarget(state_p, _handle);
			if(target_l)
			{
				Logger::getDebug() << " Entity::runCommands :: add attack command" << _handle << " -> " << target_l->_handle << std::endl;
				step_p.addSteppable(new CommandSpawnStep(new EntityAttackCommand(_commandableHandle, _handle, target_l->_handle)));
			}
		}
		else
		{
			step_p.addSteppable(new EntityUpdateWaitingStep(_handle, _waiting, _waiting+1));
		}
	}
	Commandable::runCommands(step_p, state_p);
}

double Entity::getStepSpeed() const
{
	return ( _model._stepSpeed + _buffSpeed._offset )* ( 1. + _buffSpeed._coef );
}

double Entity::getFullReload() const
{
	return ( _model._fullReload + _buffFullReload._offset )* ( 1. + _buffFullReload._coef );
}

double Entity::getDamage() const
{
	return ( _model._damage + _buffDamage._offset )* ( 1. + _buffDamage._coef );
}
double Entity::getArmor() const
{
	return ( _model._armor + _buffArmor._offset )* ( 1. + _buffArmor._coef );
}

double Entity::getProduction() const
{
	return ( 1. + _buffProduction._offset )* ( 1. + _buffProduction._coef );
}
double Entity::getHarvest() const
{
	return ( 1. + _buffHarvest._offset )* ( 1. + _buffHarvest._coef );
}

bool Entity::isFrozen() const
{
	return _frozen || _model._isStatic;
}

} // namespace octopus


std::ostream &operator<<(std::ostream &os_p, octopus::Entity const &ent_p)
{
	return os_p<<"Entity[pos="<<ent_p._pos<<", ray="<<ent_p._model._ray<<", frozen="<<ent_p._frozen<<"]";
}

std::ostream &streamCsvPos(std::ostream &os_p, octopus::Entity const &ent_p)
{
	return os_p<<ent_p._pos.x<<","<<ent_p._pos.y<<","<<ent_p._model._ray<<","<<ent_p._hp<<","<<ent_p._model._isStatic;
}

std::ostream &streamCsvEntity(std::ostream &os_p, std::vector<octopus::Entity *> const &vec_p)
{
	os_p<<"x,y,r,hp,building"<<std::endl;
	for(octopus::Entity const *ent_l : vec_p)
	{
		if(ent_l->_alive)
		{
			streamCsvPos(os_p, *ent_l) << std::endl;
		}
	}
	return os_p;
}
