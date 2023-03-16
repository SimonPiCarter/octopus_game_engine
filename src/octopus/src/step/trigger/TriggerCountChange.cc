#include "TriggerCountChange.hh"

#include "controller/trigger/Listener.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void TriggerCountChange::apply(State &state_p, SteppableData *) const
{
	Logger::getDebug() << "TriggerCountChange :: apply " << _handleTrigger << ", "<< _handleListener << " " << _old << " -> "<< _new <<std::endl;
	ListenerData * data_l = state_p.getListenerData(_handleTrigger, _handleListener);

	data_l->_count = _new;
}

void TriggerCountChange::revert(State &state_p, SteppableData *) const
{
	Logger::getDebug() << "TriggerCountChange :: revert " << _handleTrigger << ", "<< _handleListener << " "  << _new << " -> "<< _old <<std::endl;
	ListenerData * data_l = state_p.getListenerData(_handleTrigger, _handleListener);

	data_l->_count = _old;
}

bool TriggerCountChange::isNoOp() const
{
	return _old == _new;
}


} // namespace octopus
