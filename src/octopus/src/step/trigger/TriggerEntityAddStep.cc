#include "TriggerEntityAddStep.hh"

#include "controller/trigger/Listener.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void TriggerEntityAddStep::apply(State &state_p) const
{
	Logger::getDebug() << "TriggerEntityAddStep :: apply " << this->_handleTrigger << ", "<< this->_handleListener <<std::endl;
	ListenerEntityData * data_l = dynamic_cast<ListenerEntityData *>(state_p.getListenerData(_handleTrigger, _handleListener));

	data_l->_entities.push_back(_ent);
}

void TriggerEntityAddStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "TriggerEntityAddStep :: revert " << this->_handleTrigger << ", "<< this->_handleListener <<std::endl;
	ListenerEntityData * data_l = dynamic_cast<ListenerEntityData *>(state_p.getListenerData(_handleTrigger, _handleListener));

	data_l->_entities.pop_back();
}

} // namespace octopus
