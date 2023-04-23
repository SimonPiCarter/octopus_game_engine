#include "BuildingSetRallyPointStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"
#include "state/entity/Building.hh"

namespace octopus
{

void BuildingSetRallyPointStep::apply(State &state_p) const
{
	Building * ent_l = static_cast<Building *>(state_p.getEntity(this->_handle));
	Logger::getDebug() << "BuildingSetRallyPointStep :: apply " << _handle <<std::endl;

	ent_l->_rallyPointActive = true;
	ent_l->_rallyPoint = _rallyPoint;
	ent_l->_rallyPointEntityActive = _rallyPointEntityActive;
	ent_l->_rallyPointEntity = _rallyPointEntity;
}

void BuildingSetRallyPointStep::revert(State &state_p, SteppableData const *data_p) const
{
	Building * ent_l = static_cast<Building *>(state_p.getEntity(this->_handle));
	BuildingRallyPointData const * data_l = static_cast<BuildingRallyPointData const *>(data_p);
	Logger::getDebug() << "BuildingSetRallyPointStep :: revert " << _handle <<std::endl;

	ent_l->_rallyPointActive = data_l->_rallyPointActive;
	ent_l->_rallyPoint = data_l->_rallyPoint;
	ent_l->_rallyPointEntityActive = data_l->_rallyPointEntityActive;
	ent_l->_rallyPointEntity = data_l->_rallyPointEntity;
}

SteppableData * BuildingSetRallyPointStep::newData(State const &state_p) const
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

