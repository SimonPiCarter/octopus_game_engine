#ifndef __RecycleDivinityStepGenerator__
#define __RecycleDivinityStepGenerator__

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

namespace recycle_divinity
{

/// @brief contains all param for this divinity
struct RecycleDivinityParams
{
	/// @brief the number of unit dead required for the buff tier one to apply
	unsigned long _triggerNbUnitDeadBuffTierOne;
	/// @brief buff applied to tier one unit (on tier one)
	std::vector<octopus::TimedBuff> _buffTierOne;
	/// @brief the number of unit dead required for the buff tier three to apply
	unsigned long _triggerNbUnitDeadBuffTierThree;
	/// @brief buff applied to tier one unit (on tier three)
	std::vector<octopus::TimedBuff> _buffTierThree;
	/// @brief tier one unit model (unit with a buff when several units die)
	std::string _tierOneUnitModelId;
	/// @brief resource cash back on tier two
	std::map<std::string, octopus::Fixed> _resourceReturnedTierTwo;
	/// @brief resource cash back on tier three (upgrade)
	std::map<std::string, octopus::Fixed> _resourceReturnedTierThree;
	/// @brief resource harvest upgrade
	octopus::TimedBuff _resourceHarvestUpgrade;
};

std::vector<octopus::Steppable *> recycleTierOneGenertor(RecycleDivinityParams const &params_p, unsigned long player_p);
std::vector<octopus::Steppable *> recycleTierTwoGenertor(RecycleDivinityParams const &params_p, unsigned long player_p);
std::vector<octopus::Steppable *> recycleTierThreeGenertor(RecycleDivinityParams const &params_p, unsigned long player_p);

RecycleDivinityParams createDefaultParams();
void fillLibrary(RecycleDivinityParams const &params_p, octopus::Library &lib_p);

}

#endif
