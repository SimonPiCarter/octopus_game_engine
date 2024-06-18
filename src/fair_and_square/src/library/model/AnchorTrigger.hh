#ifndef __AnchorTrigger__
#define __AnchorTrigger__

#include <vector>

#include "controller/trigger/Trigger.hh"
#include "controller/trigger/Listener.hh"
#include "utils/RandomGenerator.hh"

#include "library/model/bonus/BuffGenerators.hh"
#include "library/model/survival/SurvivalModelLoader.hh"

namespace octopus
{
class Library;
} // namespace octopus


class AnchorTrigger : public octopus::OnEachTrigger
{
public:
	AnchorTrigger(octopus::Library const &lib_p, octopus::RandomGenerator &rand_p, double bonus_p,
		unsigned long player_p=0, std::vector<fas::SurvivalSpecialType> const &forbidden_p={});
	virtual void trigger(octopus::State const &, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;

private:
	unsigned long const _player;
	octopus::Library const &_lib;

	octopus::RandomGenerator & _rand;

	double const _bonus;

	std::vector<fas::SurvivalSpecialType> const _forbidden;

	/// @brief count of trigger during the game to generate buff id
	/// need to be mutable to be incremented when triggering
	mutable uint32_t _count {0};
};

std::vector<Option> generateOpeningOptions(
	std::string const &id_p,
	unsigned long seed_p,
	std::vector<fas::SurvivalSpecialType> const &forbidden_p,
	unsigned long playerIdx_p,
	octopus::State const &state_p);

std::vector<Option> generateOptions(
	std::string const &id_p,
	unsigned long seed_p,
	std::vector<fas::SurvivalSpecialType> const &forbidden_p,
	unsigned long playerIdx_p,
	octopus::State const &state_p);

#endif
