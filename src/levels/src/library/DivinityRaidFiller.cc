#include "LibraryFillers.hh"

#include "library/Library.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/model/entity/BuildingModel.hh"

using namespace octopus;

void divinityRaidFiller(Library &lib_p)
{
    {
        // unit 1
        // light fast unit
        UnitModel unitModel_l { false, 0.5, 0.75, 90. };
        unitModel_l._isUnit = true;
        unitModel_l._productionTime = 2000;
        unitModel_l._cost["Food"] = 25;
        unitModel_l._cost["Steel"] = 0;
        unitModel_l._cost["Gas"] = 0;
        unitModel_l._cost["Ether"] = 75;
        unitModel_l._damage = 20;
        unitModel_l._armor = 0;
        unitModel_l._range = 0.1;
        unitModel_l._fullReload = 200.;
        unitModel_l._windup = 50.;

        unitModel_l._requirements._divLvl["Divinity_3"] = 1;

        lib_p.registerUnitModel("div_raid_1", unitModel_l);
    }
    {
        // unit 2
        // tanky melee
        UnitModel unitModel_l { false, 0.75, 0.025, 220. };
        unitModel_l._isUnit = true;
        unitModel_l._productionTime = 3500;
        unitModel_l._cost["Food"] = 15;
        unitModel_l._cost["Steel"] = 0;
        unitModel_l._cost["Gas"] = 25;
        unitModel_l._cost["Ether"] = 100;
        unitModel_l._damage = 20;
        unitModel_l._armor = 1;
        unitModel_l._range = 0.1;
        unitModel_l._fullReload = 300.;
        unitModel_l._windup = 100.;

        unitModel_l._requirements._divLvl["Divinity_3"] = 2;

        lib_p.registerUnitModel("div_raid_2", unitModel_l);
    }
    {
        // unit 3
        // high range and damage
        UnitModel unitModel_l { false, 0.5, 0.025, 75. };
        unitModel_l._isUnit = true;
        unitModel_l._productionTime = 4000;
        unitModel_l._cost["Food"] = 0;
        unitModel_l._cost["Steel"] = 0;
        unitModel_l._cost["Gas"] = 100;
        unitModel_l._cost["Ether"] = 100;
        unitModel_l._damage = 50;
        unitModel_l._armor = 1;
        unitModel_l._range = 6.;
        unitModel_l._fullReload = 500.;
        unitModel_l._windup = 50.;

        unitModel_l._requirements._divLvl["Divinity_3"] = 3;

        lib_p.registerUnitModel("div_raid_3", unitModel_l);
    }
    {
        // well
        BuildingModel buildingModel_l { true, 0.9, 1000. };
        buildingModel_l._isBuilding = true;
        buildingModel_l._isStatic = true;
        buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("div_raid_1"));
        buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("div_raid_2"));
        buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("div_raid_3"));
        buildingModel_l._buildingTime = 4500;
        buildingModel_l._cost["Steel"] = 100;
        buildingModel_l._cost["Ether"] = 100;
        buildingModel_l._armor = 5;

        buildingModel_l._requirements._divLvl["Divinity_3"] = 1;

        lib_p.registerBuildingModel("div_raid_well", buildingModel_l);
    }
}
