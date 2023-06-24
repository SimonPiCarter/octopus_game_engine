#ifndef __ProductionDivinityStepGenerator__
#define __ProductionDivinityStepGenerator__

#include "state/model/entity/UnitModel.hh"

#include <vector>

namespace octopus
{
	class Steppable;
	class Library;
} // namespace octopus

/// @brief contains all param for this divinity
struct ProductionDivinityParams
{
	/// @brief constraint for the number of buff production unit
	long _numberOfProductionBufferTierTwo = 4;
	/// @brief increased qty of production buffer after t3 upgrade
	long _increaseOfProductionBufferTierTwo = 4;
	/// @brief identifier of the resource used to limit the number of buffer
	std::string _productionBufferResourceId = "ProductionBufferResource";
	/// @brief coef reduction for time production (positive)
	octopus::Fixed _productionTimeUpgradeCoef;
	/// @brief percent reduction for resource cost production for tier one unit model (positive)
	std::map<std::string, octopus::Fixed> _prodTierOneUnitCostReductionCoef;
	/// @brief tier one unit model (cheap unit)
	octopus::UnitModel const * _tierOneUnitModel = nullptr;
	/// @brief tier one unit model respawn after t3 upgrade (same unit just different id to avoid infinite respawn)
	octopus::UnitModel const * _tierOneUnitRespawnModel = nullptr;
	/// @brief tier two unit model that buff production buildings
	octopus::UnitModel const * _productionBufferModel = nullptr;
};

std::vector<octopus::Steppable *> productionTierOneGenertor(ProductionDivinityParams const &param_p, unsigned long player_p);
std::vector<octopus::Steppable *> productionTierTwoGenertor(ProductionDivinityParams const &param_p, octopus::Library const &lib_p, unsigned long player_p);
std::vector<octopus::Steppable *> productionTierThreeGenertor(ProductionDivinityParams const &param_p, octopus::Library const &lib_p, unsigned long player_p);

ProductionDivinityParams createDefaultParams(octopus::Library const &lib_p);

#endif
