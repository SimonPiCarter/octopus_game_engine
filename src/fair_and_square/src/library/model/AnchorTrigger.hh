#ifndef __AnchorTrigger__
#define __AnchorTrigger__

#include "controller/trigger/Trigger.hh"
#include "controller/trigger/Listener.hh"
#include "utils/RandomGenerator.hh"

namespace octopus
{
class Library;
} // namespace octopus


class AnchorTrigger : public octopus::OnEachTrigger
{
public:
	AnchorTrigger(octopus::Library const &lib_p, octopus::RandomGenerator &rand_p);
	virtual void trigger(octopus::State const &, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;

private:
	unsigned long const _player;

	octopus::RandomGenerator & _rand;
};

#endif
