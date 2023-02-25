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
	SpawnModelStep(Handle const &handle_p, class_t const &model_p, bool forceAlive_p=false) : _handle(handle_p), _model(model_p), _forceAlive(forceAlive_p) {}

	virtual void apply(State &state_p) const override
	{
		if(!state_p.hasEntity(_handle))
		{
			Handle check_l = state_p.addEntity(new class_t(_model));
			if(check_l != _handle)
			{
				throw std::logic_error("Spawn Step did not have the same handle acress multiple states");
			}
		}

		Entity * ent_l = state_p.getEntity(this->_handle);
		if(!ent_l->_model._isBuilding
		|| _forceAlive)
		{
			ent_l->_alive = true;
		}
		else
		{
			ent_l->_alive = false;
		}
		if(ent_l->_alive)
		{
			updateGrid(state_p, ent_l, true);
			updateExplorationGrid(state_p, ent_l, true);
		}
	}
	virtual void revert(State &state_p) const override
	{
		// unspawn but do not delete
		Entity * ent_l = state_p.getEntity(this->_handle);
		if(ent_l->_alive)
		{
			updateExplorationGrid(state_p, ent_l, false);
			updateGrid(state_p, ent_l, false);
		}
		ent_l->_alive = false;
	}

	virtual bool isNoOp() const override
	{
		return false;
	}

	Handle const &getHandle() const { return _handle; }
	class_t const &getModel() const { return _model; }
protected:
	Handle const _handle {0};
	bool const _forceAlive;

	class_t _model;
};


}

#endif
