#ifndef __LifestealDivinityStepGenerator__
#define __LifestealDivinityStepGenerator__

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

namespace lifesteal_divinity
{

/// @brief contains all param for this divinity
struct LifestealDivinityParams
{
	/// @brief models affected by general life steal upgrade
	std::vector<std::string> _modelsGeneralLifeSteal;
	/// @brief tier one upgrade of life steal
	octopus::Fixed _improvedLifeStealTierOne;
	/// @brief tier two upgrade of life steal for all units
	octopus::Fixed _lifeStealTierTwo;
	/// @brief tier three upgrade of life steal for all units
	octopus::Fixed _lifeStealTierThree;
	/// @brief tier one unit model (lifesteal unit)
	std::string _tierOneUnitModelId;

	octopus::TimedBuff _stimBuff;
	octopus::Fixed _stimHpCost;
};

std::vector<octopus::Steppable *> lifestealTierOneGenertor(LifestealDivinityParams const &param_p, unsigned long player_p);
std::vector<octopus::Steppable *> lifestealTierTwoGenertor(LifestealDivinityParams const &param_p, octopus::Library const &lib_p, unsigned long player_p);
std::vector<octopus::Steppable *> lifestealTierThreeGenertor(LifestealDivinityParams const &param_p, octopus::Library const &lib_p, unsigned long player_p);

LifestealDivinityParams createDefaultParams();
void fillLibrary(LifestealDivinityParams const &params_p, octopus::Library &lib_p);

} // namespace lifesteal_divinity


#endif
