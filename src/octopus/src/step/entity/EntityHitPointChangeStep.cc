#include "EntityHitPointChangeStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void EntityHitPointChangeStep::apply(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "EntityHitPointChangeStep :: " << ent_l->_hp << " + "<<this->_delta<<std::endl;
	ent_l->_hp += this->_delta;
	if(ent_l->_hp < 1e-3)
	{
		ent_l->_alive = false;
	}
	else
	{
		ent_l->_alive = true;
	}
}

void EntityHitPointChangeStep::revert(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "EntityHitPointChangeStep :: " << ent_l->_hp << " - "<<this->_delta<<std::endl;
	ent_l->_hp -= this->_delta;
	if(ent_l->_hp < 1e-3)
	{
		ent_l->_alive = false;
	}
	else
	{
		ent_l->_alive = true;
	}
}

bool EntityHitPointChangeStep::isNoOp() const
{
	return std::abs(_delta) < 1e-3;
}

} // namespace octopus

