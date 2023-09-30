#ifndef __Mission_SpawnTrigger__
#define __Mission_SpawnTrigger__

#include "controller/trigger/Trigger.hh"
#include "step/Step.hh"

#include "controller/step/DialogStep.h"

class DialogTrigger : public octopus::OneShotTrigger
{
public:
	DialogTrigger(std::list<octopus::Listener *> const &listeners_p, std::string const &dialog_p) :
		OneShotTrigger(listeners_p),
		_dialog(dialog_p)
	{}

	/// @brief trigger by adding the given steps to the step
	/// @param step_p the step to fill with steps from the trigger
	/// @param count_p the current count of the trigger in the same step (this can be used to access data for this occurence of trigger)
	void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override
	{
		step_p.addSteppable(new godot::DialogStep(_dialog));
	}

private:
	std::string const _dialog;
};

#endif
