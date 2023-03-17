#include "ProductionPaidStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"
#include "command/Commandable.hh"
#include "command/data/ProductionData.hh"

namespace octopus
{

void ProductionPaidStep::apply(State &state_p, SteppableData *) const
{
	Commandable * cmdable_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "ProductionPaidStep :: apply " << this->_handle <<std::endl;
	ProductionData *data_l = dynamic_cast<ProductionData *>(cmdable_l->getFrontQueue()._data);
	data_l->_paid = true;
}

void ProductionPaidStep::revert(State &state_p, SteppableData *) const
{
	Commandable * cmdable_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "ProductionPaidStep :: revert " << this->_handle <<std::endl;
	ProductionData *data_l = dynamic_cast<ProductionData *>(cmdable_l->getFrontQueue()._data);
	data_l->_paid = false;
}

} // namespace octopus

