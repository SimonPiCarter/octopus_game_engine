#include "UnitDataStep.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Unit.hh"
#include "state/entity/Resource.hh"
#include "state/player/Player.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void UnitDataStep::apply(State &state_p) const
{
	Logger::getNormal() << "UnitDataStep :: apply " << this->_handle <<std::endl;
	Unit * unit_l = dynamic_cast<Unit *>(state_p.getEntity(this->_handle));
	unit_l->_unitData->copyFrom(_data);
}

void UnitDataStep::revert(State &state_p, SteppableData const *data_p) const
{
	Logger::getDebug() << "UnitDataStep :: revert " << this->_handle <<std::endl;
	Unit * unit_l = dynamic_cast<Unit *>(state_p.getEntity(this->_handle));
	UnitDataStepData const *data_l = static_cast<UnitDataStepData const *>(data_p);
	unit_l->_unitData->copyFrom(data_l->_data);
}

bool UnitDataStep::isNoOp() const
{
	return false;
}

SteppableData * UnitDataStep::newData(State const &state_p) const
{
	Unit const * unit_l = dynamic_cast<Unit const *>(state_p.getEntity(this->_handle));
	return new UnitDataStepData(unit_l->_unitData);
}

} // namespace octopus
