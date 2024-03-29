#ifndef __AnchorTrigger__
#define __AnchorTrigger__

#include "controller/trigger/Trigger.hh"
#include "controller/trigger/Listener.hh"
#include "utils/RandomGenerator.hh"

#include "library/model/divinity/DivinityModelLoader.hh"

namespace octopus
{
class Library;
} // namespace octopus


class AnchorTrigger : public octopus::OnEachTrigger
{
public:
	AnchorTrigger(octopus::Library const &lib_p, octopus::RandomGenerator &rand_p, double bonus_p,
		unsigned long player_p=0, std::vector<fas::DivinityType> const &forbidden_p={});
	virtual void trigger(octopus::State const &, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;

private:
	unsigned long const _player;
	octopus::Library const &_lib;

	octopus::RandomGenerator & _rand;

	double const _bonus;

	std::vector<fas::DivinityType> const _forbidden;

	/// @brief count of trigger during the game to generate buff id
	/// need to be mutable to be incremented when triggering
	mutable uint32_t _count {0};
};

#endif
