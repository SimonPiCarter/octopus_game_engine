#include "ResourceSlotStep.hh"
#include "state/entity/Resource.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void ResourceSlotStep::apply(State &state_p) const
{
	Logger::getDebug() << "ResourceSlotStep :: apply " << this->_res <<std::endl;
	Resource * res_l = dynamic_cast<Resource*>(state_p.getEntity(this->_res));

	// update resource slot
	res_l->_harvestPoints.at(_idx).free = _free;
	res_l->_harvestPoints.at(_idx).harvester = _harvester;
}

void ResourceSlotStep::revert(State &state_p, SteppableData const *data_p) const
{
	Logger::getDebug() << "ResourceSlotStep :: revert " << this->_res <<std::endl;
	Resource * res_l = dynamic_cast<Resource*>(state_p.getEntity(this->_res));

	ResourceSlotStepData const *data_l = dynamic_cast<ResourceSlotStepData const *>(data_p);
	res_l->_harvestPoints.at(_idx) = data_l->old;
}

bool ResourceSlotStep::isNoOp() const
{
	return false;
}

SteppableData * ResourceSlotStep::newData(State const &state_p) const
{
	Resource const * res_l = dynamic_cast<Resource const *>(state_p.getEntity(this->_res));

	return new ResourceSlotStepData(res_l->_harvestPoints.at(_idx));
}


} // namespace octopus
