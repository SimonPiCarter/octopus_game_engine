#ifndef __AnchorDivinityTrigger__
#define __AnchorDivinityTrigger__

#include "controller/trigger/Trigger.hh"
#include "controller/trigger/Listener.hh"
#include "utils/RandomGenerator.hh"
#include "divinity/DivinityGenerator.hh"
#include "divinity/DivinityModelLoader.hh"

namespace octopus
{
class Library;
} // namespace octopus


class AnchorDivinityTrigger : public octopus::OnEachTrigger
{
public:
	AnchorDivinityTrigger(octopus::Library const &lib_p, octopus::RandomGenerator &rand_p, unsigned long player_p, std::vector<fas::DivinityType> const &types_p, double bonus_p);

	virtual void trigger(octopus::State const &, octopus::CommandContext const &, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;

private:
	unsigned long const _player;

	octopus::RandomGenerator & _rand;

	std::vector<fas::DivinityType> const _types;

	double const _bonus;

	/// @brief count of trigger during the game to generate option key
	/// need to be mutable to be incremented when triggering
	mutable size_t _count {0};
};

#endif
