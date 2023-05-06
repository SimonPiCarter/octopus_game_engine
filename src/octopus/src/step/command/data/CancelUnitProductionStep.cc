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

    for(CommandBundle const &bundle_l : ent_l.getQueue().getList())
    {
        octopus::UnitProductionData *data_l = dynamic_cast<octopus::UnitProductionData*>(bundle_l._cmd->getData());
        if(it_l->_id == _idx)
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

    for(CommandBundle const &bundle_l : ent_l.getQueue().getList())
    {
        octopus::UnitProductionData *data_l = dynamic_cast<octopus::UnitProductionData*>(bundle_l._cmd->getData());
        if(it_l->_id == _idx)
        {
            data_l->_canceled = false;
            break;
        }
        ++it_l;
    }
}

}