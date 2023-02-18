#include "TriggerEntityResetStep.hh"

#include "controller/trigger/Listener.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void TriggerEntityResetStep::apply(State &state_p) const
{
	Logger::getDebug() << "TriggerEntityResetStep :: apply " << this->_handleTrigger << ", "<< this->_handleListener <<std::endl;
	ListenerEntityData * data_l = dynamic_cast<ListenerEntityData *>(state_p.getListenerData(_handleTrigger, _handleListener));

	data_l->_entities.clear();
}

void TriggerEntityResetStep::revert(State &state_p) const
{
	Logger::getDebug() << "TriggerEntityResetStep :: revert " << this->_handleTrigger << ", "<< this->_handleListener <<std::endl;
	ListenerEntityData * data_l = dynamic_cast<ListenerEntityData *>(state_p.getListenerData(_handleTrigger, _handleListener));

	data_l->_entities = _ent;
}

} // namespace octopus
