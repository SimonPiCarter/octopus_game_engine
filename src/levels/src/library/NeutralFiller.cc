#include "LibraryFillers.hh"

#include "library/Library.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/TempleModel.hh"
#include "state/model/entity/UnitModel.hh"

using namespace octopus;

void neutralFiller(Library &lib_p)
{
    {
        // worker
        UnitModel unitModel_l { false, 0.5, 0.025, 50. };
        unitModel_l._isUnit = true;
        unitModel_l._isBuilder = true;
        unitModel_l._productionTime = 2000;
        unitModel_l._timeToHarvest = 100;
        unitModel_l._cost["bloc"] = 50;
        unitModel_l._cost["ether"] = 0;
        unitModel_l._cost["steel"] = 0;
        unitModel_l._cost["idiem"] = 0;
        unitModel_l._maxQuantity["idiem"] = 10;
        unitModel_l._maxQuantity["bloc"] = 10;
        unitModel_l._maxQuantity["steel"] = 10;
        unitModel_l._maxQuantity["ether"] = 10;
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
        buildingModel_l._deposit["bloc"] = true;
        buildingModel_l._deposit["ether"] = true;
        buildingModel_l._deposit["idiem"] = true;
        buildingModel_l._deposit["steel"] = true;
        buildingModel_l._buildingTime = 18000;
        buildingModel_l._cost["ether"] = 450;
        buildingModel_l._cost["steel"] = 450;
        buildingModel_l._armor = 5;

        lib_p.registerBuildingModel("command_center", buildingModel_l);
    }
    {
        // deposit
        BuildingModel buildingModel_l { true, 0.9, 500. };
        buildingModel_l._isBuilding = true;
        buildingModel_l._isStatic = true;
        buildingModel_l._buildingTime = 2000;
        buildingModel_l._cost["ether"] = 50;
        buildingModel_l._deposit["bloc"] = true;
        buildingModel_l._deposit["ether"] = true;
        buildingModel_l._deposit["steel"] = true;
        buildingModel_l._armor = 5;

        lib_p.registerBuildingModel("deposit", buildingModel_l);
    }
    {
        // ether deposit
        BuildingModel buildingModel_l { true, 0.9, 500. };
        buildingModel_l._isBuilding = true;
        buildingModel_l._isStatic = true;
        buildingModel_l._buildingTime = 2000;
        buildingModel_l._cost["ether"] = 50;
        buildingModel_l._cost["steel"] = 50;
        buildingModel_l._deposit["idiem"] = true;
        buildingModel_l._armor = 5;

        lib_p.registerBuildingModel("ether_deposit", buildingModel_l);
    }
    {
        // temple
        TempleModel buildingModel_l { true, 0.9, 1000. };
        buildingModel_l._isBuilding = true;
        buildingModel_l._isStatic = true;
        buildingModel_l._buildingTime = 2000;
        buildingModel_l._cost["ether"] = 150;
        buildingModel_l._cost["steel"] = 50;
        buildingModel_l._armor = 5;

        lib_p.registerTempleModel("temple", buildingModel_l);
    }
    {
        // abandonned temple (should match size of temple)
        BuildingModel buildingModel_l { true, 0.9, 1000. };
        buildingModel_l._isStatic = true;
        buildingModel_l._isAbandonedTemple = true;
        buildingModel_l._buildingTime = 2000;
        buildingModel_l._armor = 5;

        lib_p.registerBuildingModel("abandonned_temple", buildingModel_l);
    }
}
