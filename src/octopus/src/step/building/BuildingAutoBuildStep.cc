#include "BuildingAutoBuildStep.hh"

#include "state/entity/Building.hh"
#include "state/entity/Entity.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void BuildingAutoBuildStep::apply(State &state_p) const
{
	Building * ent_l = dynamic_cast<Building *>(state_p.getEntity(this->_handle));
	Logger::getDebug() << "BuildingAutoBuildStep :: apply " << this->_handle <<std::endl;
	ent_l->_autoBuild = _new;
}

void BuildingAutoBuildStep::revert(State &state_p, SteppableData const *) const
{
	Building * ent_l = dynamic_cast<Building *>(state_p.getEntity(this->_handle));
	Logger::getDebug() << "BuildingAutoBuildStep :: revert " << this->_handle<<std::endl;
	ent_l->_autoBuild = _old;
}

} // namespace octopus

