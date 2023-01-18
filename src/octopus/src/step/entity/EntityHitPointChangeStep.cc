#include "EntityHitPointChangeStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void updateFromHp(State &state_p, Entity *ent_p)
{
	if(ent_p->_hp < 1e-3)
	{
		if(ent_p->_alive)
		{
			updateGrid(state_p, ent_p, false);
		}
		ent_p->_alive = false;
	}
	else
	{
		if(!ent_p->_alive)
		{
			updateGrid(state_p, ent_p, false);
		}
		ent_p->_alive = true;
	}
}

void EntityHitPointChangeStep::apply(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "EntityHitPointChangeStep :: " << ent_l->_hp << " + "<<this->_delta<<std::endl;
	ent_l->_hp += this->_delta;
	updateFromHp(state_p, ent_l);
}

void EntityHitPointChangeStep::revert(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "EntityHitPointChangeStep :: " << ent_l->_hp << " - "<<this->_delta<<std::endl;
	ent_l->_hp -= this->_delta;
	updateFromHp(state_p, ent_l);
}

bool EntityHitPointChangeStep::isNoOp() const
{
	return std::abs(_delta) < 1e-3;
}

} // namespace octopus

