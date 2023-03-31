#ifndef __AnchorTrigger__
#define __AnchorTrigger__

#include <random>

#include "controller/trigger/Trigger.hh"
#include "controller/trigger/Listener.hh"

namespace octopus
{
class Library;
} // namespace octopus


class AnchorTrigger : public octopus::OnEachTrigger
{
public:
	AnchorTrigger(octopus::Library const &lib_p);
	virtual void trigger(octopus::State const &, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;

private:
	unsigned long const _player;

	// required for random gen in const method
	mutable std::mt19937 _gen;
};

#endif
