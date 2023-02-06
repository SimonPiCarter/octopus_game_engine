#include "TriggerCountChange.hh"

#include "controller/trigger/Listener.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void TriggerCountChange::apply(State &state_p) const
{
	Logger::getDebug() << "TriggerCountChange :: apply " << this->_handleTrigger << ", "<< this->_handleListener <<std::endl;
	ListenerData * data_l = state_p.getListenerData(_handleTrigger, _handleListener);

	data_l->_count = _new;
}

void TriggerCountChange::revert(State &state_p) const
{
	Logger::getDebug() << "TriggerCountChange :: revert " << this->_handleTrigger << ", "<< this->_handleListener <<std::endl;
	ListenerData * data_l = state_p.getListenerData(_handleTrigger, _handleListener);

	data_l->_count = _old;
}

bool TriggerCountChange::isNoOp() const
{
	return _old == _new;
}


} // namespace octopus
