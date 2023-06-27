#ifndef __HealDivinityStepGenerator__
#define __HealDivinityStepGenerator__

#include <map>
#include <vector>
#include <string>
#include "utils/Fixed.hh"
#include "state/entity/buff/TimedBuff.hh"

namespace octopus
{
	class Steppable;
	class Library;
} // namespace octopus

namespace heal_divinity
{

/// @brief contains all param for this divinity
struct HealDivinityParams
{
	/// @brief hp buff of tier one unit from upgrage tier one
	octopus::Fixed _buffHpTierOne;
	/// @brief regen for all units from tier two
	octopus::Fixed _tierTwoRegen;
	/// @brief heal buff of tier one unit from upgrade tier two
	octopus::Fixed _buffHealTierTwo;

	/// @brief heal amount from tier three
	octopus::Fixed _healTierThree;
	/// @brief heal range from tier three
	octopus::Fixed _rangeHealTierThree;

	/// @brief hp buff of tier one unit from upgrage tier three
	octopus::Fixed _buffHpTierThree;
	/// @brief heal buff of tier one unit from upgrade tier three
	octopus::Fixed _buffHealTierThree;

	/// @brief tier one unit model (heal unit)
	std::string _tierOneUnitModelId;
};

std::vector<octopus::Steppable *> healTierOneGenertor(HealDivinityParams const &param_p, unsigned long player_p);
std::vector<octopus::Steppable *> healTierTwoGenertor(HealDivinityParams const &param_p, octopus::Library const &lib_p, unsigned long player_p);
std::vector<octopus::Steppable *> healTierThreeGenertor(HealDivinityParams const &param_p, octopus::Library const &lib_p, unsigned long player_p);

HealDivinityParams createDefaultParams();
void fillLibrary(HealDivinityParams const &params_p, octopus::Library &lib_p);

} // namespace heal_divinity


#endif
