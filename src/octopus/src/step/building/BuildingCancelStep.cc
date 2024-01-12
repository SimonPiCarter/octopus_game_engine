#include "BuildingCancelStep.hh"

#include "state/State.hh"
#include "step/Step.hh"
#include "logger/Logger.hh"
#include "state/entity/Building.hh"

namespace octopus
{

void updateFromCancel(Building *ent_p, State &state_p)
{
	if(ent_p->isBlueprint())
	{
		return;
	}
	if(ent_p->_canceled)
	{
		Logger::getDebug() << "BuildingStep :: not alive " <<std::endl;
		if(ent_p->_alive)
		{
			updateGrid(state_p, ent_p, false);
			updateVisionGrid(state_p, ent_p, false);
			updateExplorationGrid(state_p, ent_p, false);
		}
		ent_p->_alive = false;
	}
	else
	{
		Logger::getDebug() << "BuildingStep :: alive " <<std::endl;
		if(!ent_p->_alive)
		{
			updateGrid(state_p, ent_p, true);
			updateVisionGrid(state_p, ent_p, true);
			updateExplorationGrid(state_p, ent_p, true);
		}
		ent_p->_alive = true;
	}
}

void BuildingCancelStep::consolidate(State const &state_p, Step const &step_p)
{
	Building const * building_l = static_cast<Building const *>(state_p.getEntity(this->_handle));
	_old = building_l->_canceled || step_p.isCanceled(this->_handle);
}

void BuildingCancelStep::apply(State &state_p) const
{
	Building * ent_l = static_cast<Building *>(state_p.getEntity(this->_handle));
	Logger::getDebug() << "BuildingCancelStep :: apply " << _handle <<std::endl;
	ent_l->_canceled = _new;
	updateFromCancel(ent_l, state_p);
	if(_new)
	{
		for(auto &&pair_l : _deltaResources)
		{
			state_p.getPlayer(ent_l->_player)->_resources[pair_l.first] += pair_l.second;
		}
	}
}

void BuildingCancelStep::revert(State &state_p, SteppableData const *) const
{
	Building * ent_l = static_cast<Building *>(state_p.getEntity(this->_handle));
	Logger::getDebug() << "BuildingCancelStep :: revert " << _handle <<std::endl;
	ent_l->_canceled = _old;
	updateFromCancel(ent_l, state_p);
	if(_new)
	{
		for(auto &&pair_l : _deltaResources)
		{
			state_p.getPlayer(ent_l->_player)->_resources[pair_l.first] -= pair_l.second;
		}
	}
}

} // namespace octopus

