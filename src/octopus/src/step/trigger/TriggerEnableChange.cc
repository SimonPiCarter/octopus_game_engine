#include "TriggerEnableChange.hh"

#include "controller/trigger/Listener.hh"
#include "controller/trigger/Trigger.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void TriggerEnableChange::apply(State &state_p, SteppableData *) const
{
	Logger::getDebug() << "TriggerEnableChange :: apply " << this->_handleTrigger  <<std::endl;
	TriggerData * trigger_l = state_p.getTriggerData(_handleTrigger);

	trigger_l->_isEnabled = _new;
}

void TriggerEnableChange::revert(State &state_p, SteppableData *) const
{
	Logger::getDebug() << "TriggerEnableChange :: revert " << this->_handleTrigger  <<std::endl;
	TriggerData * trigger_l = state_p.getTriggerData(_handleTrigger);

	trigger_l->_isEnabled = _old;
}

bool TriggerEnableChange::isNoOp() const
{
	return _old == _new;
}

} // namespace octopus
