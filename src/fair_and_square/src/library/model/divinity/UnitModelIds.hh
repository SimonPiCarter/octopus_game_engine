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

    const std::vector<std::string> BasicModels = {"square", "double_square", "circle", "triangle", "reverse_triangle",
        AttackSpeedUnitModelTierOneId,
        HealUnitModelTierOneId,
        LifestealUnitModelTierOneId,
        ProductionUnitModelTierOneId,
        ProductionUnitModelTierOneRespawnId,
        ProductionUnitModelTierTwoId,
        RecycleUnitModelTierOneId
    };
}

#endif
