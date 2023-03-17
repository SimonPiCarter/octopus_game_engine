#include "EntityHitPointChangeStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

/// @brief clamp the hp of the entity between 0 and max life
/// @param ent_p
/// @return the correction made (old value + correction = new value)
double clampHp(Entity * ent_p)
{
	double correction_l = 0.;
	if(ent_p->_hp < 0)
	{
		correction_l = - ent_p->_hp;
		ent_p->_hp = 0;
	}
	if(ent_p->_hp > ent_p->getHpMax())
	{
		correction_l = ent_p->getHpMax() - ent_p->_hp;
		ent_p->_hp = ent_p->getHpMax();
	}
	return correction_l;
}

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
			updateGrid(state_p, ent_p, true);
		}
		ent_p->_alive = true;
	}
}

void EntityHitPointChangeStep::apply(State &state_p, SteppableData *data_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "EntityHitPointChangeStep :: " << ent_l->_hp << " + "<<this->_delta<<std::endl;
	ent_l->_hp += this->_delta;
	EntityHitPointChangeStepData * data_l = dynamic_cast<EntityHitPointChangeStepData *>(data_p);

	data_l->_correction = clampHp(ent_l);

	updateFromHp(state_p, ent_l);
}

void EntityHitPointChangeStep::revert(State &state_p, SteppableData *data_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "EntityHitPointChangeStep :: " << ent_l->_hp << " - "<<this->_delta<<std::endl;
	EntityHitPointChangeStepData * data_l = dynamic_cast<EntityHitPointChangeStepData *>(data_p);

	ent_l->_hp -= this->_delta + data_l->_correction;

	updateFromHp(state_p, ent_l);
}

bool EntityHitPointChangeStep::isNoOp() const
{
	return std::abs(_delta) < 1e-3;
}

} // namespace octopus

