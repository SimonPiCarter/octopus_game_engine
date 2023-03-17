#include "EntityAttackStep.hh"

#include "state/entity/Entity.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void EntityAttackStep::apply(State &state_p, SteppableData *) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "EntityAttackStep :: apply " << this->_handle <<std::endl;
	ent_l->_reload = 0;
}

void EntityAttackStep::revert(State &state_p, SteppableData *) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "EntityAttackStep :: revert " << this->_handle<<std::endl;
	ent_l->_reload = _timeSinceLastAttack;
}

bool EntityAttackStep::isNoOp() const
{
	return false;
}

} // namespace octopus

