#include "AttackModifierStep.hh"

#include <stdexcept>
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void AttackModifierStep::apply(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_target);
	Logger::getDebug() << "AttackModifierStep :: apply " << this->_target <<std::endl;

    ent_l->_attackMod = _mod;
}

void AttackModifierStep::revert(State &state_p, SteppableData const *data_p) const
{
	Entity * ent_l = state_p.getEntity(this->_target);
	Logger::getDebug() << "AttackModifierStep :: revert " << this->_target<<std::endl;
	AttackModifierStepData const * data_l = dynamic_cast<AttackModifierStepData const *>(data_p);
    ent_l->_attackMod = data_l->_old;
}

SteppableData * AttackModifierStep::newData(State const &state_p) const
{
	Entity const * ent_l = state_p.getEntity(_target);
	return new AttackModifierStepData(ent_l->_attackMod);
}

} // namespace octopus

