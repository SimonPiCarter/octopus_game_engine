#include "ProductionProgressionStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"
#include "command/Commandable.hh"
#include "command/data/ProductionData.hh"

namespace octopus
{

void ProductionProgressionStep::apply(State &state_p) const
{
	Commandable * cmdable_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "ProductionProgressionStep :: apply " << this->_handle <<std::endl;
	ProductionData *data_l = dynamic_cast<ProductionData *>(getData(cmdable_l->getFrontQueue()._var));
	data_l->_progression += _delta;
}

void ProductionProgressionStep::revert(State &state_p, SteppableData const *) const
{
	Commandable * cmdable_l = state_p.getCommandable(this->_handle);
	Logger::getDebug() << "ProductionProgressionStep :: revert " << this->_handle <<std::endl;
	ProductionData *data_l = dynamic_cast<ProductionData *>(getData(cmdable_l->getFrontQueue()._var));
	data_l->_progression -= _delta;
}

bool ProductionProgressionStep::isNoOp() const
{
	return _delta == 0;
}

} // namespace octopus

