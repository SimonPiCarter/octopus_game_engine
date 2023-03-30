#include "EntityFrozenStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void EntityFrozenStep::apply(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "EntityFrozenStep :: apply " << _old << " -> "<< _new <<std::endl;
	ent_l->_frozen = _new;
}

void EntityFrozenStep::revert(State &state_p, SteppableData const *) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "EntityFrozenStep :: revert " << _new << " -> "<< _old <<std::endl;
	ent_l->_frozen = _old;
}

bool EntityFrozenStep::isNoOp() const
{
	return _old == _new;
}

} // namespace octopus

