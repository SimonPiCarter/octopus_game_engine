#include "LibraryFillers.hh"

#include "library/Library.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/DivinityType.hh"
#include "state/ResourceType.hh"

using namespace octopus;

void neutralFiller(Library &lib_p)
{
    {
        // worker
        UnitModel unitModel_l { false, 0.5, 0.025, 50. };
        unitModel_l._isUnit = true;
        unitModel_l._isBuilder = true;
        unitModel_l._productionTime = 2000;
        unitModel_l._gatherRate = 0.01;
        unitModel_l._cost[ResourceType::Food] = 50;
        unitModel_l._cost[ResourceType::Steel] = 0;
        unitModel_l._cost[ResourceType::Gas] = 0;
        unitModel_l._cost[ResourceType::Ether] = 0;
        unitModel_l._maxQuantity[ResourceType::Ether] = 10;
        unitModel_l._maxQuantity[ResourceType::Food] = 10;
        unitModel_l._maxQuantity[ResourceType::Gas] = 10;
        unitModel_l._maxQuantity[ResourceType::Steel] = 10;
        unitModel_l._damage = 5;
        unitModel_l._armor = 0;
        unitModel_l._range = 3.;
        unitModel_l._fullReload = 500.;
        unitModel_l._windup = 100.;

        lib_p.registerUnitModel("worker", unitModel_l);
    }
    {
        // command center
        BuildingModel buildingModel_l { true, 1.9, 5000. };
        buildingModel_l._isBuilding = true;
        buildingModel_l._isStatic = true;
        buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("worker"));
        buildingModel_l._deposit[ResourceType::Food] = true;
        buildingModel_l._deposit[ResourceType::Steel] = true;
        buildingModel_l._deposit[ResourceType::Ether] = true;
        buildingModel_l._deposit[ResourceType::Gas] = true;
        buildingModel_l._buildingTime = 18000;
        buildingModel_l._cost[ResourceType::Steel] = 450;
        buildingModel_l._cost[ResourceType::Gas] = 450;
        buildingModel_l._armor = 5;

        lib_p.registerBuildingModel("command_center", buildingModel_l);
    }
    {
        // deposit
        BuildingModel buildingModel_l { true, 0.9, 500. };
        buildingModel_l._isBuilding = true;
        buildingModel_l._isStatic = true;
        buildingModel_l._buildingTime = 2000;
        buildingModel_l._cost[ResourceType::Steel] = 50;
        buildingModel_l._deposit[ResourceType::Food] = true;
        buildingModel_l._deposit[ResourceType::Steel] = true;
        buildingModel_l._deposit[ResourceType::Gas] = true;
        buildingModel_l._armor = 5;

        lib_p.registerBuildingModel("deposit", buildingModel_l);
    }
    {
        // ether deposit
        BuildingModel buildingModel_l { true, 0.9, 500. };
        buildingModel_l._isBuilding = true;
        buildingModel_l._isStatic = true;
        buildingModel_l._buildingTime = 2000;
        buildingModel_l._cost[ResourceType::Steel] = 50;
        buildingModel_l._cost[ResourceType::Gas] = 50;
        buildingModel_l._deposit[ResourceType::Ether] = true;
        buildingModel_l._armor = 5;

        lib_p.registerBuildingModel("ether_deposit", buildingModel_l);
    }
    {
        // temple
        BuildingModel buildingModel_l { true, 0.9, 1000. };
        buildingModel_l._isBuilding = true;
        buildingModel_l._isStatic = true;
        buildingModel_l._buildingTime = 2000;
        buildingModel_l._cost[ResourceType::Steel] = 150;
        buildingModel_l._cost[ResourceType::Gas] = 50;
        buildingModel_l._armor = 5;

        lib_p.registerBuildingModel("temple", buildingModel_l);
    }
}
