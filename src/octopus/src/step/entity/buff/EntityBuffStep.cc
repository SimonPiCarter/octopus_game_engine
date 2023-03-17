#include "EntityBuffStep.hh"

#include <stdexcept>
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void EntityBuffStep::apply(State &state_p, SteppableData *) const
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
		if(!_new)
		{
			throw std::logic_error("Could not create buff because step was not declared as new");
		}
		Logger::getDebug() << "EntityBuffStep :: add buff"<<std::endl;
		ent_l->_timeSinceBuff[_buff._id] = 0;
		ent_l->_registeredBuff[_buff._id] = _buff;
	}
}

void EntityBuffStep::revert(State &state_p, SteppableData *) const
{
	Entity * ent_l = state_p.getEntity(this->_target);
	Logger::getDebug() << "EntityBuffStep :: revert " << this->_target<<std::endl;

	if(_new)
	{
		ent_l->_timeSinceBuff.erase(_buff._id);
	}
	else
	{
		ent_l->_timeSinceBuff[_buff._id] = _oldTime;
	}
}

bool EntityBuffStep::isNoOp() const
{
	return std::abs(_buff._offset) < 1e-3 && std::abs(_buff._coef) < 1e-3;
}

} // namespace octopus

