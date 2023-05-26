#include "CancelUnitProductionStep.hh"

#include "logger/Logger.hh"

#include "command/data/ProductionData.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"

namespace octopus
{

void CancelUnitProductionStep::apply(State &state_p) const
{
	Logger::getDebug() << "CancelUnitProductionStep :: apply " << this->_handle <<std::endl;
    Entity *ent_l = state_p.getEntity(this->_handle);

    CommandBundle &bundle_l = *ent_l->getQueue().getBundle(_idx);
    octopus::ProductionData *data_l = dynamic_cast<octopus::ProductionData*>(getData(bundle_l._var));
    data_l->_canceled = true;
}

void CancelUnitProductionStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "CancelUnitProductionStep :: revert " << this->_handle <<std::endl;
    Entity *ent_l = state_p.getEntity(this->_handle);

    CommandBundle &bundle_l = *ent_l->getQueue().getBundle(_idx);
    octopus::ProductionData *data_l = dynamic_cast<octopus::ProductionData*>(getData(bundle_l._var));
    data_l->_canceled = false;
}

}
