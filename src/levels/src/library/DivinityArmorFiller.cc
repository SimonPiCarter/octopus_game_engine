#include "LibraryFillers.hh"

#include "library/Library.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/DivinityType.hh"
#include "state/ResourceType.hh"

using namespace octopus;

void divinityArmorFiller(Library &lib_p)
{
    {
        // unit 1
        // tanky melee (armor)
        UnitModel unitModel_l { false, 0.5, 0.025, 100. };
        unitModel_l._isUnit = true;
        unitModel_l._productionTime = 2500;
        unitModel_l._cost[ResourceType::Food] = 30;
        unitModel_l._cost[ResourceType::Steel] = 0;
        unitModel_l._cost[ResourceType::Gas] = 0;
        unitModel_l._cost[ResourceType::Ether] = 80;
        unitModel_l._damage = 10;
        unitModel_l._armor = 3;
        unitModel_l._range = 0.1;
        unitModel_l._fullReload = 400.;
        unitModel_l._windup = 50.;

        unitModel_l._requirements._divLvl[DivinityType::Divinity_4] = 1;

        lib_p.registerUnitModel("div_armor_1", unitModel_l);
    }
    {
        /// @todo
    }
    {
        // unit 3
        // fat tanky
        UnitModel unitModel_l { false, 0.75, 0.02, 300. };
        unitModel_l._isUnit = true;
        unitModel_l._productionTime = 4500;
        unitModel_l._cost[ResourceType::Food] = 10;
        unitModel_l._cost[ResourceType::Steel] = 0;
        unitModel_l._cost[ResourceType::Gas] = 120;
        unitModel_l._cost[ResourceType::Ether] = 150;
        unitModel_l._damage = 35;
        unitModel_l._armor = 2;
        unitModel_l._range = 3;
        unitModel_l._fullReload = 400.;
        unitModel_l._windup = 100.;

        unitModel_l._requirements._divLvl[DivinityType::Divinity_4] = 3;

        lib_p.registerUnitModel("div_armor_3", unitModel_l);
    }
    {
        // well
        BuildingModel buildingModel_l { true, 0.9, 1000. };
        buildingModel_l._isBuilding = true;
        buildingModel_l._isStatic = true;
        buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("div_armor_1"));
        ///buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("div_armor_2")); @todo
        buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("div_armor_3"));
        buildingModel_l._buildingTime = 4500;
        buildingModel_l._cost[ResourceType::Steel] = 100;
        buildingModel_l._cost[ResourceType::Ether] = 100;
        buildingModel_l._armor = 5;

        buildingModel_l._requirements._divLvl[DivinityType::Divinity_4] = 1;

        lib_p.registerBuildingModel("div_armor_well", buildingModel_l);
    }
}
