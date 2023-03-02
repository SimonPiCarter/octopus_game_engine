#include "EntityUpdateWaitingStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void EntityUpdateWaitingStep::apply(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	//Logger::getDebug() << "EntityUpdateWaitingStep :: apply " << _old << " -> "<< _new <<std::endl;
	ent_l->_waiting = _new;
}

void EntityUpdateWaitingStep::revert(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	//Logger::getDebug() << "EntityUpdateWaitingStep :: revert " << _new << " -> "<< _old <<std::endl;
	ent_l->_waiting = _old;
}

bool EntityUpdateWaitingStep::isNoOp() const
{
	return _old == _new;
}

} // namespace octopus

