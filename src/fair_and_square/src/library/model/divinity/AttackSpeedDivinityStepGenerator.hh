#ifndef __AttackspeedDivinityStepGenerator__
#define __AttackspeedDivinityStepGenerator__

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

namespace attackspeed_divinity
{

/// @brief contains all param for this divinity
struct AttackSpeedDivinityParams
{
	/// @brief tier two upgrade of damage
	octopus::Fixed _tierTwoDamageIncrease;
	/// @brief models affected by general attack damage upgrade (t2)
	std::vector<std::string> _modelsGeneralDamage;

	/// @brief tier three attack speed buff when full life
	octopus::Fixed _tierThreeAttackSpeedIfFullLife;
	/// @brief models affected by general attack speed upgrade (t3)
	std::vector<std::string> _modelsGeneralAttackspeed;

	/// @brief tier three upgrade buffing damage of tier one unit when full life
	octopus::Fixed _damageUpgradeForTierOneUnit;
	/// @brief tier one unit model (attackspeed unit)
	std::string _tierOneUnitModelId;
};

std::vector<octopus::Steppable *> attackspeedTierOneGenertor(AttackSpeedDivinityParams const &param_p, unsigned long player_p);
std::vector<octopus::Steppable *> attackspeedTierTwoGenertor(AttackSpeedDivinityParams const &param_p, octopus::Library const &lib_p, unsigned long player_p);
std::vector<octopus::Steppable *> attackspeedTierThreeGenertor(AttackSpeedDivinityParams const &param_p, octopus::Library const &lib_p, unsigned long player_p);

AttackSpeedDivinityParams createDefaultParams();
void fillLibrary(AttackSpeedDivinityParams const &params_p, octopus::Library &lib_p);

} // namespace attackspeed_divinity


#endif
