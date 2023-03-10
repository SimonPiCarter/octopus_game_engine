#include "TriggerSpawn.hh"

#include "controller/trigger/Listener.hh"
#include "controller/trigger/Trigger.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void TriggerSpawn::apply(State &state_p) const
{
	Logger::getDebug() << "TriggerSpawn :: apply " << std::endl;

	std::vector<Trigger const *> & triggers_l = state_p.getTriggers();
	std::vector<TriggerData *> & triggersData_l = state_p.getTriggersData();

	// Create trigger data and register trigger
	triggers_l.push_back(_trigger);
	triggersData_l.push_back(_trigger->newTriggerData(triggers_l.size()-1));
}

void TriggerSpawn::revert(State &state_p) const
{
	Logger::getDebug() << "TriggerSpawn :: revert " << std::endl;

	std::vector<Trigger const *> & triggers_l = state_p.getTriggers();
	std::vector<TriggerData *> & triggersData_l = state_p.getTriggersData();

	// safety check
	if(triggers_l.back() != _trigger)
	{
		throw std::logic_error("Error when reverting trigger spawn");
	}

	// un register trigger
	triggers_l.pop_back();

	// delete trigger data
	delete triggersData_l.back();
	triggersData_l.pop_back();
}

bool TriggerSpawn::isNoOp() const
{
	return _trigger == nullptr;
}

} // namespace octopus
