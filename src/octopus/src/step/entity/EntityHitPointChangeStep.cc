#include "EntityHitPointChangeStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

/// @brief clamp the hp of the entity between 0 and max life
/// @param ent_p
/// @param lethal_p if set to false the delta will only bring the hp to 1
/// @return the new delta to be applied
Fixed getDelta(Fixed hp_p, Fixed delta_p, Fixed hpMax_p, bool lethal_p)
{
	Fixed curHp_l = hp_p+delta_p;
	Fixed newDelta_l = delta_p;
	if(curHp_l <= 0)
	{
		if(lethal_p)
		{
			newDelta_l = -hp_p;
		}
		else
		{
			newDelta_l = 1-hp_p;
		}
	}
	if(curHp_l > hpMax_p)
	{
		newDelta_l = hpMax_p - hp_p;
	}
	return newDelta_l;
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

EntityHitPointChangeStep::EntityHitPointChangeStep(Handle const &handle_p, Fixed delta_p, bool lethal_p)
	: _handle(handle_p), _delta(delta_p), _lethal(lethal_p)
{}

void EntityHitPointChangeStep::apply(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "EntityHitPointChangeStep :: " << ent_l->_hp << " + "<<this->_delta<<std::endl;
	ent_l->_hp += getDelta(ent_l->_hp, _delta, ent_l->getHpMax(), _lethal);

	updateFromHp(state_p, ent_l);
}

void EntityHitPointChangeStep::revert(State &state_p, SteppableData const *data_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	HitPointChangeData const * data_l = dynamic_cast<HitPointChangeData const *>(data_p);
	Logger::getDebug() << "EntityHitPointChangeStep :: " << ent_l->_hp << " - "<<this->_delta<<std::endl;

	ent_l->_hp -= data_l->_delta;

	updateFromHp(state_p, ent_l);
}

bool EntityHitPointChangeStep::isNoOp() const
{
	return ::is_zero(_delta);
}

SteppableData * EntityHitPointChangeStep::newData(State const &state_p) const
{
	if(!state_p.isEntityAlive(_handle))
	{
		return new HitPointChangeData { 0 };
	}
	Entity const * ent_l = state_p.getEntity(_handle);
	return new HitPointChangeData { getDelta(ent_l->_hp, _delta, ent_l->getHpMax(), _lethal) };
}

} // namespace octopus

