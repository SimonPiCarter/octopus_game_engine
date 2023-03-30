#include "EntityBuffStep.hh"

#include <stdexcept>
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void EntityBuffStep::apply(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_target);
	Logger::getDebug() << "EntityBuffStep :: apply " << this->_target <<" ("<<_buff._id<<")"<<std::endl;

	// If buff already existed we just reset it to one (and not to 0)
	// 0 should only be set when first registering the buff
	// we keep the min in case the buff was applied twice in a step
	if(ent_l->_timeSinceBuff.find(_buff._id) != ent_l->_timeSinceBuff.end())
	{
		Logger::getDebug() << "EntityBuffStep :: update time since buff"<<std::endl;
		if(ent_l->_timeSinceBuff[_buff._id] >= _buff._duration)
		{
			ent_l->_timeSinceBuff[_buff._id] = 0;
		}
		else
		{
			ent_l->_timeSinceBuff[_buff._id] = std::min(ent_l->_timeSinceBuff[_buff._id], 1ul);
		}
	}
	else
	{
		Logger::getDebug() << "EntityBuffStep :: add buff"<<std::endl;
		ent_l->_timeSinceBuff[_buff._id] = 0;
		ent_l->_registeredBuff[_buff._id] = _buff;
	}
}

void EntityBuffStep::revert(State &state_p, SteppableData const *data_p) const
{
	Entity * ent_l = state_p.getEntity(this->_target);
	Logger::getDebug() << "EntityBuffStep :: revert " << this->_target<<std::endl;
	EntityBuffStepData const * data_l = dynamic_cast<EntityBuffStepData const *>(data_p);

	if(data_l->_new)
	{
		ent_l->_timeSinceBuff.erase(_buff._id);
	}
	else
	{
		ent_l->_timeSinceBuff[_buff._id] = data_l->_oldTime;
	}
}

bool EntityBuffStep::isNoOp() const
{
	return std::abs(_buff._offset) < 1e-3 && std::abs(_buff._coef) < 1e-3;
}

SteppableData * EntityBuffStep::newData(State const &state_p) const
{
	Entity const * ent_l = state_p.getEntity(_target);
	auto &&it_l = ent_l->_timeSinceBuff.find(_buff._id);
	bool new_l = it_l == ent_l->_timeSinceBuff.cend();
	unsigned long time_l = 0;
	if(!new_l)
	{
		time_l = it_l->second;
	}

	EntityBuffStepData * data_l = new EntityBuffStepData(time_l, new_l);

	return data_l;
}

} // namespace octopus

