#ifndef __ProductionDivinityStepGenerator__
#define __ProductionDivinityStepGenerator__

#include <map>
#include <vector>
#include <string>
#include "utils/Fixed.hh"

namespace octopus
{
	class Steppable;
	class Library;
} // namespace octopus

namespace production_divinity
{

/// @brief contains all param for this divinity
struct ProductionDivinityParams
{
	/// @brief constraint for the number of buff production unit
	long _numberOfProductionBufferTierTwo = 4;
	/// @brief increased qty of production buffer after t3 upgrade
	long _increaseOfProductionBufferTierTwo = 4;
	/// @brief coef reduction for time production (positive)
	octopus::Fixed _productionTimeUpgradeCoef;
	/// @brief percent reduction for resource cost production for tier one unit model (positive)
	octopus::Fixed _prodTierOneUnitCostReductionCoef;
	/// @brief tier one unit model (cheap unit)
	std::string _tierOneUnitModelId;
	/// @brief tier one unit model respawn after t3 upgrade (same unit just different id to avoid infinite respawn)
	std::string _tierOneUnitRespawnModelId;
	/// @brief tier two unit model that buff production buildings
	std::string _productionBufferModelId;
};

std::vector<octopus::Steppable *> productionTierOneGenertor(ProductionDivinityParams const &param_p, unsigned long player_p);
std::vector<octopus::Steppable *> productionTierTwoGenertor(ProductionDivinityParams const &param_p, octopus::Library const &lib_p, unsigned long player_p);
std::vector<octopus::Steppable *> productionTierThreeGenertor(ProductionDivinityParams const &param_p, octopus::Library const &lib_p, unsigned long player_p);

ProductionDivinityParams createDefaultParams();
void fillLibrary(ProductionDivinityParams const &params_p, octopus::Library &lib_p);

} // namespace production_divinity


#endif
