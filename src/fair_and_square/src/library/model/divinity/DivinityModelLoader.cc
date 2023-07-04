#include "DivinityModelLoader.hh"

#include "library/Library.hh"
#include "step/player/PlayerSpawnStep.hh"

// fas
#include "library/model/divinity/generators/AttackSpeedDivinityStepGenerator.hh"
#include "library/model/divinity/generators/EconomicDivinityStepGenerator.hh"
#include "library/model/divinity/generators/HealDivinityStepGenerator.hh"
#include "library/model/divinity/generators/LifestealDivinityStepGenerator.hh"
#include "library/model/divinity/generators/ProductionDivinityStepGenerator.hh"
#include "library/model/divinity/generators/RecycleDivinityStepGenerator.hh"
#include "library/model/divinity/generators/UnitModelIds.hh"

namespace fas
{

void loadLibrary(octopus::Library &lib_p)
{
	// load divinity library
	attackspeed_divinity::fillLibrary(attackspeed_divinity::createDefaultParams(), lib_p);
	economic_divinity::fillLibrary(economic_divinity::createDefaultParams(), lib_p);
	heal_divinity::fillLibrary(heal_divinity::createDefaultParams(), lib_p);
	lifesteal_divinity::fillLibrary(lifesteal_divinity::createDefaultParams(), lib_p);
	production_divinity::fillLibrary(production_divinity::createDefaultParams(), lib_p);
	recycle_divinity::fillLibrary(recycle_divinity::createDefaultParams(), lib_p);
}

std::vector<int> divinityTypeToInt(std::vector<DivinityType> const &types_p)
{
    std::vector<int> result_l;
    for(DivinityType const &type_l : types_p)
    {
        result_l.push_back(static_cast<int>(type_l));
    }
    return result_l;
}

std::vector<DivinityType> intToDivinityType(std::vector<int> const &types_p)
{
    std::vector<DivinityType> result_l;
    for(int const &type_l : types_p)
    {
        result_l.push_back(static_cast<DivinityType>(type_l));
    }
    return result_l;
}

std::vector<octopus::Steppable *> newPlayerBuilding(unsigned long player_p, DivinityType type_l, octopus::Library const &lib_p)
{
    switch(type_l)
    {
        case DivinityType::AttackSpeed:
            return {new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel(models::AttackSpeedBuildingId))};
        case DivinityType::Economic:
            return {
                new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel(models::EconomicBuildingId)),
                new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel(models::EconomicBuildingResourceProducerId))
            };
        case DivinityType::Heal:
            return {new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel(models::HealBuildingId))};
        case DivinityType::Lifesteal:
            return {new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel(models::LifestealBuildingId))};
        case DivinityType::Production:
            return {new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel(models::ProductionBuildingId))};
        case DivinityType::Recycle:
            return {new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel(models::RecycleBuildingId))};
    }
    return {};
}

} // namespace fas

