#ifndef __UnitModelsId__
#define __UnitModelsId__

#include <string>
#include <vector>

namespace models
{
    const std::string AttackSpeedUnitModelTierOneId = "AttackSpeedUnitModelTierOneId";
    const std::string HealUnitModelTierOneId = "HealUnitModelTierOneId";
    const std::string LifestealUnitModelTierOneId = "LifestealUnitModelTierOneId";
    const std::string ProductionUnitModelTierOneId = "ProductionUnitModelTierOneId";
    const std::string ProductionUnitModelTierOneRespawnId = "ProductionUnitModelTierOneRespawnId";
    const std::string ProductionUnitModelTierTwoId = "ProductionUnitModelTierTwoId";
    const std::string RecycleUnitModelTierOneId = "RecycleUnitModelTierOneId";

    const std::string AttackSpeedBuildingId = "AttackSpeedBuildingId";
    const std::string EconomicBuildingId = "EconomicBuildingId";
    const std::string EconomicBuildingResourceProducerId = "EconomicBuildingResourceProducer";
    const std::string HealBuildingId = "HealBuildingId";
    const std::string LifestealBuildingId = "LifestealBuildingId";
    const std::string ProductionBuildingId = "ProductionBuildingId";
    const std::string RecycleBuildingId = "RecycleBuildingId";

    const std::vector<std::string> BasicUnitModels = {"square", "double_square", "circle", "triangle", "reverse_triangle",
        AttackSpeedUnitModelTierOneId,
        HealUnitModelTierOneId,
        LifestealUnitModelTierOneId,
        ProductionUnitModelTierOneId,
        ProductionUnitModelTierOneRespawnId,
        ProductionUnitModelTierTwoId,
        RecycleUnitModelTierOneId
    };

    const std::vector<std::string> BasicBuildingProducingModels = {"barrack_square", "barrack_triangle", "barrack_circle",
        AttackSpeedBuildingId,
        HealBuildingId,
        LifestealBuildingId,
        EconomicBuildingId,
        ProductionBuildingId,
        RecycleBuildingId
    };

    const std::vector<std::string> BasicBuildingDepositModels = {
    };
}

#endif
