#include "BuildingCancelStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"
#include "state/entity/Building.hh"

namespace octopus
{

void BuildingCancelStep::apply(State &state_p) const
{
	Building * ent_l = static_cast<Building *>(state_p.getEntity(this->_handle));
	Logger::getDebug() << "BuildingCancelStep :: apply " << _handle <<std::endl;
	ent_l->_canceled = _new;
}

void BuildingCancelStep::revert(State &state_p) const
{
	Building * ent_l = static_cast<Building *>(state_p.getEntity(this->_handle));
	Logger::getDebug() << "BuildingCancelStep :: revert " << _handle <<std::endl;
	ent_l->_canceled = _old;
}

} // namespace octopus

