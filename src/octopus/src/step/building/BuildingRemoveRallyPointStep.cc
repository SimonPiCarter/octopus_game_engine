#include "BuildingRemoveRallyPointStep.hh"
#include "BuildingSetRallyPointStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"
#include "state/entity/Building.hh"

namespace octopus
{

void BuildingRemoveRallyPointStep::apply(State &state_p) const
{
	Building * ent_l = static_cast<Building *>(state_p.getEntity(this->_handle));
	Logger::getDebug() << "BuildingRemoveRallyPointStep :: apply " << _handle <<std::endl;

	ent_l->_rallyPointActive = false;
	ent_l->_rallyPoint = {0,0};
	ent_l->_rallyPointEntityActive = false;
	ent_l->_rallyPointEntity = 0;
}

void BuildingRemoveRallyPointStep::revert(State &state_p, SteppableData const *data_p) const
{
	Building * ent_l = static_cast<Building *>(state_p.getEntity(this->_handle));
	BuildingRallyPointData const * data_l = static_cast<BuildingRallyPointData const *>(data_p);
	Logger::getDebug() << "BuildingRemoveRallyPointStep :: revert " << _handle <<std::endl;

	ent_l->_rallyPointActive = data_l->_rallyPointActive;
	ent_l->_rallyPoint = data_l->_rallyPoint;
	ent_l->_rallyPointEntityActive = data_l->_rallyPointEntityActive;
	ent_l->_rallyPointEntity = data_l->_rallyPointEntity;
}

SteppableData * BuildingRemoveRallyPointStep::newData(State const &state_p) const
{
	Building const * ent_l = static_cast<Building const *>(state_p.getEntity(this->_handle));

	BuildingRallyPointData * data_l = new BuildingRallyPointData();
	data_l->_rallyPointActive = ent_l->_rallyPointActive;
	data_l->_rallyPoint = ent_l->_rallyPoint;
	data_l->_rallyPointEntityActive = ent_l->_rallyPointEntityActive;
	data_l->_rallyPointEntity = ent_l->_rallyPointEntity;

	return data_l;
}

} // namespace octopus

