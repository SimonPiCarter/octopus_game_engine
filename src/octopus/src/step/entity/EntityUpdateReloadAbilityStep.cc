#include "EntityUpdateReloadAbilityStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void EntityUpdateReloadAbilityStep::apply(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "EntityUpdateReloadAbilityStep :: apply " << _old << " -> "<< _new <<" for "<<_key<<std::endl;
	ent_l->_reloadAbilities[_key] = _new;
}

void EntityUpdateReloadAbilityStep::revert(State &state_p, SteppableData const *) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "EntityUpdateReloadAbilityStep :: revert " << _new << " -> "<< _old <<" for "<<_key<<std::endl;
	ent_l->_reloadAbilities[_key] = _old;
}

bool EntityUpdateReloadAbilityStep::isNoOp() const
{
	return _old == _new && _new != 0;
}

} // namespace octopus

