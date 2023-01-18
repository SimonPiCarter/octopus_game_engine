#ifndef __SPAWN_MODEL_STEP__
#define __SPAWN_MODEL_STEP__

#include "state/entity/Entity.hh"
#include "state/State.hh"
#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
template<typename class_t>
class SpawnModelStep : public Steppable
{
public:
	SpawnModelStep(class_t const &model_p) : _model(model_p) {}

	virtual void apply(State &state_p) const override
	{
		// first time we apply this step we setup the handled
		// it must be the same for every state it is applied
		if(!_initialized)
		{
			_handle = state_p.addEntity(new class_t(_model));
			_initialized = true;
		}
		else
		{
			if(!state_p.hasEntity(_handle))
			{
				Handle check_l = state_p.addEntity(new class_t(_model));
				if(check_l != _handle)
				{
					throw std::logic_error("Spawn Step did not have the same handle acress multiple states");
				}
			}
		}
		Entity * ent_l = state_p.getEntity(this->_handle);
		ent_l->_alive = true;
		updateGrid(state_p, ent_l, true);
	}
	virtual void revert(State &state_p) const override
	{
		// unspawn but do not delete
		Entity * ent_l = state_p.getEntity(this->_handle);
		ent_l->_alive = false;
		updateGrid(state_p, ent_l, false);
	}

	virtual bool isNoOp() const override
	{
		return false;
	}
protected:
	/// @brief setup on first apply (that's why it must be mutable)
	mutable bool _initialized {false};
	mutable Handle _handle {0};

	class_t _model;
};


}

#endif
