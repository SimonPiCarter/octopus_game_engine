#include "LibraryFillers.hh"

#include "library/Library.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/DivinityType.hh"
#include "state/ResourceType.hh"

using namespace octopus;

void divinityFireFiller(Library &lib_p)
{
    {
        // unit 1
        // ranged rapid fire low dmg
        UnitModel unitModel_l { false, 0.5, 0.0125, 50. };
        unitModel_l._isUnit = true;
        unitModel_l._productionTime = 1000;
        unitModel_l._cost[ResourceType::Food] = 15;
        unitModel_l._cost[ResourceType::Steel] = 0;
        unitModel_l._cost[ResourceType::Gas] = 15;
        unitModel_l._cost[ResourceType::Ether] = 25;
        unitModel_l._damage = 3;
        unitModel_l._armor = 0;
        unitModel_l._range = 4;
        unitModel_l._fullReload = 50.;
        unitModel_l._windup = 25.;

        unitModel_l._requirements._divLvl[DivinityType::Divinity_5] = 1;

        lib_p.registerUnitModel("div_fire_1", unitModel_l);
    }
    {
        /// @todo
    }
    {
        // unit 3
        // fast armored
        UnitModel unitModel_l { false, 0.5, 0.025, 180. };
        unitModel_l._isUnit = true;
        unitModel_l._productionTime = 4000;
        unitModel_l._cost[ResourceType::Food] = 10;
        unitModel_l._cost[ResourceType::Steel] = 0;
        unitModel_l._cost[ResourceType::Gas] = 150;
        unitModel_l._cost[ResourceType::Ether] = 150;
        unitModel_l._damage = 25;
        unitModel_l._armor = 3;
        unitModel_l._range = 0.1;
        unitModel_l._fullReload = 200.;
        unitModel_l._windup = 50.;

        unitModel_l._requirements._divLvl[DivinityType::Divinity_5] = 3;

        lib_p.registerUnitModel("div_fire_3", unitModel_l);
    }
    {
        // well
        // high range and damage
        BuildingModel buildingModel_l { true, 0.9, 1000. };
        buildingModel_l._isBuilding = true;
        buildingModel_l._isStatic = true;
        buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("div_fire_1"));
        ///buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("div_fire_2")); @todo
        buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("div_fire_3"));
        buildingModel_l._buildingTime = 4500;
        buildingModel_l._cost[ResourceType::Steel] = 100;
        buildingModel_l._cost[ResourceType::Ether] = 100;
        buildingModel_l._armor = 5;

        buildingModel_l._requirements._divLvl[DivinityType::Divinity_5] = 1;

        lib_p.registerBuildingModel("div_fire_well", buildingModel_l);
    }
}
