#include "LibraryFillers.hh"

#include "library/Library.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/model/entity/BuildingModel.hh"

using namespace octopus;

void divinityFireFiller(Library &lib_p)
{
    {
        // unit 1
        // ranged rapid fire low dmg
        UnitModel unitModel_l { false, 0.5, 0.025, 50. };
        unitModel_l._isUnit = true;
        unitModel_l._productionTime = 1000;
        unitModel_l._cost["bloc"] = 15;
        unitModel_l._cost["ether"] = 0;
        unitModel_l._cost["steel"] = 15;
        unitModel_l._cost["idiem"] = 25;
        unitModel_l._damage = 3;
        unitModel_l._armor = 0;
        unitModel_l._range = 4;
        unitModel_l._fullReload = 50.;
        unitModel_l._windup = 25.;

        unitModel_l._requirements._upgradeLvl["Divinity_5"] = 1;

        lib_p.registerUnitModel("div_fire_1", unitModel_l);
    }
    {
        // unit 2
        // buffing damage
        UnitModel unitModel_l { false, 0.5, 0.025, 100. };
        unitModel_l._isUnit = true;
        unitModel_l._productionTime = 3000;
        unitModel_l._cost["bloc"] = 10;
        unitModel_l._cost["ether"] = 0;
        unitModel_l._cost["steel"] = 75;
        unitModel_l._cost["idiem"] = 75;
        unitModel_l._damage = 0;
        unitModel_l._armor = 2;
        unitModel_l._range = 0.1;
        unitModel_l._fullReload = 400.;
        unitModel_l._windup = 50.;

        // should buff damage + 3
        unitModel_l._buffer._buff._id = "damage";
        unitModel_l._buffer._buff._type = TyppedBuff::Type::Damage;
        unitModel_l._buffer._buff._duration = 3000;
        unitModel_l._buffer._buff._offset = 3;
        unitModel_l._buffer._buff._coef = 0;
        unitModel_l._buffer._range = 5.;
        unitModel_l._buffer._reload = 500;
        unitModel_l._buffer._active = true;

        unitModel_l._requirements._upgradeLvl["Divinity_5"] = 2;

        lib_p.registerUnitModel("div_fire_2", unitModel_l);
    }
    {
        // unit 3
        // fast armored
        UnitModel unitModel_l { false, 0.5, 0.05, 180. };
        unitModel_l._isUnit = true;
        unitModel_l._productionTime = 4000;
        unitModel_l._cost["bloc"] = 10;
        unitModel_l._cost["ether"] = 0;
        unitModel_l._cost["steel"] = 150;
        unitModel_l._cost["idiem"] = 150;
        unitModel_l._damage = 25;
        unitModel_l._armor = 3;
        unitModel_l._range = 0.1;
        unitModel_l._fullReload = 200.;
        unitModel_l._windup = 50.;

        unitModel_l._requirements._upgradeLvl["Divinity_5"] = 3;

        lib_p.registerUnitModel("div_fire_3", unitModel_l);
    }
    {
        // well
        BuildingModel buildingModel_l { true, 0.9, 1000. };
        buildingModel_l._isBuilding = true;
        buildingModel_l._isStatic = true;
        buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("div_fire_1"));
        buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("div_fire_2"));
        buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("div_fire_3"));
        buildingModel_l._buildingTime = 4500;
        buildingModel_l._cost["ether"] = 100;
        buildingModel_l._cost["idiem"] = 100;
        buildingModel_l._armor = 5;

        buildingModel_l._requirements._upgradeLvl["Divinity_5"] = 1;

        lib_p.registerBuildingModel("div_fire_well", buildingModel_l);
    }
}
