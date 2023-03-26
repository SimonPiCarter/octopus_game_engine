#include "BuildingStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"
#include "state/entity/Building.hh"

namespace octopus
{

void updateFromProgress(Building *ent_p, State &state_p)
{
	if(ent_p->_buildingProgress < 1e-3)
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

void BuildingStep::apply(State &state_p, SteppableData *) const
{
	Building * ent_l = static_cast<Building *>(state_p.getEntity(this->_handle));
	Logger::getDebug() << "BuildingStep :: apply " << _handle <<std::endl;
	ent_l->_buildingProgress += this->_delta;
	updateFromProgress(ent_l, state_p);
}

void BuildingStep::revert(State &state_p, SteppableData *) const
{
	Building * ent_l = static_cast<Building *>(state_p.getEntity(this->_handle));
	Logger::getDebug() << "BuildingStep :: revert " << _handle <<std::endl;
	ent_l->_buildingProgress -= this->_delta;
	updateFromProgress(ent_l, state_p);
}

bool BuildingStep::isNoOp() const
{
	return std::abs(_delta) < 1e-3;
}

} // namespace octopus

