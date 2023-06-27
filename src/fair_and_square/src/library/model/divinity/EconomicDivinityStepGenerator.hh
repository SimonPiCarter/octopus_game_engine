#ifndef __EconomicDivinityStepGenerator__
#define __EconomicDivinityStepGenerator__

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

namespace economic_divinity
{

/// @brief contains all param for this divinity
struct EconomicDivinityParams
{
	/// @brief reduced step time for worker on tier one
	octopus::Fixed _workerReducedTime;

	/// @brief tier two building model (resource production)
	std::string _tierTwoBuildingModelId;

	/// @brief hp buff for every unit
	octopus::Fixed _hpBuffTierThree;
	/// @brief cost coef increase for every unit
	octopus::Fixed _costDebuffTierThree;

	/// @brief resource offset buff
	octopus::Fixed _resourceProductionBuffTierThree;
};

std::vector<octopus::Steppable *> economicTierOneGenertor(EconomicDivinityParams const &params_p, unsigned long player_p);
std::vector<octopus::Steppable *> economicTierTwoGenertor(EconomicDivinityParams const &params_p, unsigned long player_p);
std::vector<octopus::Steppable *> economicTierThreeGenertor(EconomicDivinityParams const &params_p, unsigned long player_p);

EconomicDivinityParams createDefaultParams();
void fillLibrary(EconomicDivinityParams const &params_p, octopus::Library &lib_p);

}

#endif
