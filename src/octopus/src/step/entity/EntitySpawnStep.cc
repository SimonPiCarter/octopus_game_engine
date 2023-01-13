#include "EntitySpawnStep.hh"

#include "state/State.hh"

namespace octopus
{
void EntitySpawnStep::apply(State &state_p) const
{
	// first time we apply this step we setup the handled
	// it must be the same for every state it is applied
	if(!_initialized)
	{
		_handle = state_p.addEntity(new Entity(_model));
		_initialized = true;
	}
	else
	{
		if(!state_p.hasEntity(_handle))
		{
			Handle check_l = state_p.addEntity(new Entity(_model));
			if(check_l != _handle)
			{
				throw std::logic_error("Spawn Step did not have the same handle acress multiple states");
			}
		}
	}
	state_p.getEntity(this->_handle)->_alive = true;
}

void EntitySpawnStep::revert(State &state_p) const
{
	// unspawn but not delete
	state_p.getEntity(this->_handle)->_alive = false;
}

bool EntitySpawnStep::isNoOp() const
{
	return false;
}

} // namespace octopus

