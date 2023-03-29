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

    auto it_l = ent_l->getQueue().getCurrentCommand();
    while(it_l != ent_l->getQueue().getEnd())
    {
        octopus::UnitProductionData *data_l = dynamic_cast<octopus::UnitProductionData*>(it_l->_data);
        if(data_l == _data)
        {
            data_l->_canceled = true;
            break;
        }
        ++it_l;
    }
}

void CancelUnitProductionStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "CancelUnitProductionStep :: revert " << this->_handle <<std::endl;
    Entity *ent_l = state_p.getEntity(this->_handle);

    auto it_l = ent_l->getQueue().getCurrentCommand();
    while(it_l != ent_l->getQueue().getEnd())
    {
        octopus::UnitProductionData *data_l = dynamic_cast<octopus::UnitProductionData*>(it_l->_data);
        if(data_l == _data)
        {
            data_l->_canceled = false;
            break;
        }
        ++it_l;
    }
}

}