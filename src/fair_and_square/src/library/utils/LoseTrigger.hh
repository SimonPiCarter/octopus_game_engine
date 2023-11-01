#ifndef __FASUtilsLoseTrigger__
#define __FASUtilsLoseTrigger__

#include "controller/trigger/Trigger.hh"

namespace octopus
{
	class State;
	class Step;
} // namespace octopus

namespace fas
{

class LoseTrigger : public octopus::OneShotTrigger
{
public:
	LoseTrigger(octopus::Listener * listener_p);

	virtual void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;
};

}

#endif
