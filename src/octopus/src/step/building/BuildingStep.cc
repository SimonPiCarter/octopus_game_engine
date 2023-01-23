#include "BuildingStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"
#include "state/entity/Building.hh"

namespace octopus
{

void updateFromProgress(State &state_p, Building *ent_p)
{
	if(ent_p->_buildingProgress < 1e-3)
	{
		ent_p->_alive = false;
	}
	else
	{
		ent_p->_alive = true;
	}
}

void BuildingStep::apply(State &state_p) const
{
	Building * ent_l = static_cast<Building *>(state_p.getEntity(this->_handle));
	Logger::getDebug() << "BuildingStep :: apply " << _handle <<std::endl;
	ent_l->_buildingProgress += this->_delta;
	updateFromProgress(state_p, ent_l);
}

void BuildingStep::revert(State &state_p) const
{
	Building * ent_l = static_cast<Building *>(state_p.getEntity(this->_handle));
	Logger::getDebug() << "BuildingStep :: revert " << _handle <<std::endl;
	ent_l->_buildingProgress -= this->_delta;
	updateFromProgress(state_p, ent_l);
}

bool BuildingStep::isNoOp() const
{
	return std::abs(_delta) < 1e-3;
}

} // namespace octopus

