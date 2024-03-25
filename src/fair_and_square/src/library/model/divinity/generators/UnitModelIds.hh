#ifndef __UnitModelsId__
#define __UnitModelsId__

#include <functional>
#include <string>
#include <vector>

#include "state/model/entity/BuildingModel.hh"
#include "state/player/upgrade/StepUpgradeGenerator.hh"
#include "state/player/upgrade/Upgrade.hh"
#include "library/Library.hh"

namespace models
{
    // units
    const std::string AttackSpeedUnitModelTierOneId = "AttackSpeedUnitModelTierOneId";
    const std::string HealUnitModelTierOneId = "HealUnitModelTierOneId";
    const std::string LifestealUnitModelTierOneId = "LifestealUnitModelTierOneId";
    const std::string ProductionUnitModelTierOneId = "ProductionUnitModelTierOneId";
    const std::string ProductionUnitModelTierOneRespawnId = "ProductionUnitModelTierOneRespawnId";
    const std::string ProductionUnitModelTierTwoId = "ProductionUnitModelTierTwoId";
    const std::string RecycleUnitModelTierOneId = "RecycleUnitModelTierOneId";

    // specific resources names
    const std::string RecycleDeathResourceId = "RecycleDeathBuffTrigger";
    const std::string ProductionBufferResourceId = "ProductionBufferResource";

    // buildings
    const std::string AttackSpeedBuildingId = "AttackSpeedBuildingId";
    const std::string EconomicBuildingId = "EconomicBuildingId";
    const std::string EconomicBuildingResourceProducerId = "EconomicBuildingResourceProducerId";
    const std::string HealBuildingId = "HealBuildingId";
    const std::string LifestealBuildingId = "LifestealBuildingId";
    const std::string ProductionBuildingId = "ProductionBuildingId";
    const std::string RecycleBuildingId = "RecycleBuildingId";

	// rune upgrade
    const std::string AttackSpeedDivId = "AttackSpeedDivId";
    const std::string EconomicDivId = "EconomicDivId";
    const std::string HealDivId = "HealDivId";
    const std::string LifestealDivId = "LifestealDivId";
    const std::string ProductionDivId = "ProductionDivId";
    const std::string RecycleDivId = "RecycleDivId";

	// suffix
    const std::string tierOneSuffix = "_UpgradeTierOne";
    const std::string tierTwoSuffix = "_UpgradeTierTwo";
    const std::string tierThreeSuffix = "_UpgradeTierThree";

    const std::vector<std::string> BasicUnitModels = {"square", "double_square", "circle", "triangle", "reverse_triangle",
        AttackSpeedUnitModelTierOneId,
        HealUnitModelTierOneId,
        LifestealUnitModelTierOneId,
        ProductionUnitModelTierOneId,
        ProductionUnitModelTierOneRespawnId,
        ProductionUnitModelTierTwoId,
        RecycleUnitModelTierOneId
    };

    const std::vector<std::string> AllUnitModels = {"square", "double_square", "circle", "triangle", "reverse_triangle", "long_square", "long_triangle",
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

    template<typename param_t>
    class TierUpgrade : public octopus::StepUpgradeGenerator
    {
    public:
        TierUpgrade(param_t const &param_p, std::function<std::vector<octopus::Steppable *>(param_t, unsigned long)> const &fn_p) : _param(param_p), _fn(fn_p) {}

        virtual TierUpgrade* newCopy() const override
        {
            return new TierUpgrade(_param, _fn);
        }

        virtual std::vector<octopus::Steppable *> getSteppables(unsigned long player_p, unsigned long) const
        {
            return _fn(_param, player_p);
        }
    private:
        param_t const _param;
        std::function<std::vector<octopus::Steppable *>(param_t, unsigned long)> const _fn;
    };

    template<typename param_t>
    void fillTierUpgrade(octopus::Library &lib_p, param_t const &params_p, std::string const &id_p,
        std::function<std::vector<octopus::Steppable *>(param_t, unsigned long)> const &t1_p,
        std::function<std::vector<octopus::Steppable *>(param_t, unsigned long)> const &t2_p,
        std::function<std::vector<octopus::Steppable *>(param_t, unsigned long)> const &t3_p,
        octopus::BuildingModel &model_p)
    {
        octopus::Upgrade * upT1_l = new octopus::Upgrade(id_p+models::tierOneSuffix, new models::TierUpgrade<param_t>(params_p, t1_p));
        upT1_l->_cost["ether"] = 200;
        upT1_l->_cost["bloc"] = 100;
        upT1_l->_productionTime = 1500;
        upT1_l->_requirements._upgradeLvl[id_p] = 1;
        lib_p.registerUpgrade(upT1_l->_id, upT1_l);

        octopus::Upgrade * upT2_l = new octopus::Upgrade(id_p+models::tierTwoSuffix, new models::TierUpgrade<param_t>(params_p, t2_p));
        upT2_l->_cost["ether"] = 600;
        upT2_l->_cost["irium"] = 300;
        upT2_l->_productionTime = 3000;
        upT2_l->_requirements._upgradeLvl[id_p] = 2;
        upT2_l->_requirements._upgradeLvl[upT1_l->_id] = 1;
        lib_p.registerUpgrade(upT2_l->_id, upT2_l);

        octopus::Upgrade * upT3_l = new octopus::Upgrade(id_p+models::tierThreeSuffix, new models::TierUpgrade<param_t>(params_p, t3_p));
        upT3_l->_cost["bloc"] = 1200;
        upT3_l->_cost["irium"] = 600;
        upT3_l->_productionTime = 6000;
        upT3_l->_requirements._upgradeLvl[id_p] = 3;
        upT3_l->_requirements._upgradeLvl[upT2_l->_id] = 1;
        lib_p.registerUpgrade(upT3_l->_id, upT3_l);

        model_p._upgrades.push_back(upT1_l);
        model_p._upgrades.push_back(upT2_l);
        model_p._upgrades.push_back(upT3_l);
    }
}

#endif
