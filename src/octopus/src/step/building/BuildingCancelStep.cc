#include "BuildingCancelStep.hh"

#include "state/State.hh"
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

void BuildingCancelStep::apply(State &state_p) const
{
	Building * ent_l = static_cast<Building *>(state_p.getEntity(this->_handle));
	Logger::getDebug() << "BuildingCancelStep :: apply " << _handle <<std::endl;
	ent_l->_canceled = _new;
	updateFromCancel(ent_l, state_p);
}

void BuildingCancelStep::revert(State &state_p, SteppableData const *) const
{
	Building * ent_l = static_cast<Building *>(state_p.getEntity(this->_handle));
	Logger::getDebug() << "BuildingCancelStep :: revert " << _handle <<std::endl;
	ent_l->_canceled = _old;
	updateFromCancel(ent_l, state_p);
}

} // namespace octopus

