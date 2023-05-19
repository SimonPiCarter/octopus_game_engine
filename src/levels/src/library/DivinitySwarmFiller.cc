#include "LibraryFillers.hh"

#include "library/Library.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/DivinityType.hh"

using namespace octopus;

void divinitySwarmFiller(Library &lib_p)
{
    {
        // unit 1
        // cheap melee
        UnitModel unitModel_l { false, 0.35, 0.05, 35. };
        unitModel_l._isUnit = true;
        unitModel_l._productionTime = 1000;
        unitModel_l._cost["Food"] = 15;
        unitModel_l._cost["Steel"] = 0;
        unitModel_l._cost["Gas"] = 0;
        unitModel_l._cost["Ether"] = 25;
        unitModel_l._damage = 6;
        unitModel_l._armor = 0;
        unitModel_l._range = 0.1;
        unitModel_l._fullReload = 150.;
        unitModel_l._windup = 25.;

        unitModel_l._requirements._divLvl[DivinityType::Divinity_1] = 1;

        lib_p.registerUnitModel("div_swarm_1", unitModel_l);
    }
    {
        // unit 2
        // ranged moderate damage
        UnitModel unitModel_l { false, 0.5, 0.025, 50. };
        unitModel_l._isUnit = true;
        unitModel_l._productionTime = 2000;
        unitModel_l._cost["Food"] = 0;
        unitModel_l._cost["Steel"] = 0;
        unitModel_l._cost["Gas"] = 10;
        unitModel_l._cost["Ether"] = 40;
        unitModel_l._damage = 6;
        unitModel_l._armor = 0;
        unitModel_l._range = 4;
        unitModel_l._fullReload = 150.;
        unitModel_l._windup = 50.;

        unitModel_l._requirements._divLvl[DivinityType::Divinity_1] = 2;

        lib_p.registerUnitModel("div_swarm_2", unitModel_l);
    }
    {
        /// @todo
    }
    {
        // well
        BuildingModel buildingModel_l { true, 0.9, 1000. };
        buildingModel_l._isBuilding = true;
        buildingModel_l._isStatic = true;
        buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("div_swarm_1"));
        buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("div_swarm_2"));
        ///buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("div_swarm_3")); @todo
        buildingModel_l._buildingTime = 4500;
        buildingModel_l._cost["Steel"] = 100;
        buildingModel_l._cost["Ether"] = 100;
        buildingModel_l._armor = 5;

        buildingModel_l._requirements._divLvl[DivinityType::Divinity_1] = 1;

        lib_p.registerBuildingModel("div_swarm_well", buildingModel_l);
    }
}
