#include "TriggerStepCountChange.hh"

#include "controller/trigger/Listener.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void TriggerStepCountChange::apply(State &state_p) const
{
	Logger::getDebug() << "TriggerStepCountChange :: apply " << this->_handleTrigger << ", "<< this->_handleListener <<std::endl;
	ListenerStepCountData * data_l = dynamic_cast<ListenerStepCountData *>(state_p.getListenerData(_handleTrigger, _handleListener));

	data_l->_elapsedStep = _new;
}

void TriggerStepCountChange::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "TriggerStepCountChange :: revert " << this->_handleTrigger << ", "<< this->_handleListener <<std::endl;
	ListenerStepCountData * data_l = dynamic_cast<ListenerStepCountData *>(state_p.getListenerData(_handleTrigger, _handleListener));

	data_l->_elapsedStep = _old;
}

bool TriggerStepCountChange::isNoOp() const
{
	return _old == _new;
}


} // namespace octopus
