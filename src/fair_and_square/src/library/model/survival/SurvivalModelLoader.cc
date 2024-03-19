#include "SurvivalModelLoader.hh"

#include "library/Library.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/ResourceModel.hh"

using namespace octopus;

void createSurvivalHealer(Library &lib_p)
{
	UnitModel unitModel_l { false, 0.5, 0.04, 90 };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 5500;
	unitModel_l._cost["bloc"] = 100;
	unitModel_l._cost["ether"] = 150;
	unitModel_l._damage = 0;
	unitModel_l._heal = 10;
	unitModel_l._armor = 0;
	unitModel_l._range = 5.;
	unitModel_l._lineOfSight = 10;
	unitModel_l._fullReload = 100;
	unitModel_l._windup = 20;

	lib_p.registerUnitModel("survival_healer", unitModel_l);
}

void createSurvivalAttackSpeed(Library &lib_p)
{
	UnitModel unitModel_l { false, 0.5, 0.05, 50 };
	unitModel_l._projectile = true;
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 3000;
	unitModel_l._cost["bloc"] = 25;
	unitModel_l._cost["ether"] = 75;
	unitModel_l._damage = 2;
	unitModel_l._armor = 0;
	unitModel_l._range = 4;
	unitModel_l._lineOfSight = 10;
	unitModel_l._fullReload = 45;
	unitModel_l._windup = 5;
	unitModel_l._damageReturn = 0.5;

	lib_p.registerUnitModel("survival_attack_speed", unitModel_l);
}

void createSurvivalTank(Library &lib_p)
{
	UnitModel unitModel_l { false, 0.5, 0.04, 250 };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 5500;
	unitModel_l._cost["bloc"] = 150;
	unitModel_l._cost["ether"] = 150;
	unitModel_l._damage = 15;
	unitModel_l._armor = 2;
	unitModel_l._damageReturn = 0.1;
	unitModel_l._range = 0.5;
	unitModel_l._lineOfSight = 10;
	unitModel_l._fullReload = 100;
	unitModel_l._windup = 20;

	lib_p.registerUnitModel("survival_tank", unitModel_l);
}

void createSurvivalAoe(Library &lib_p)
{
	UnitModel unitModel_l { false, 0.5, 0.025, 75 };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 4000;
	unitModel_l._cost["bloc"] = 50;
	unitModel_l._cost["ether"] = 100;
	unitModel_l._damage = 12;
	unitModel_l._armor = 0;
	unitModel_l._range = 3;
	unitModel_l._lineOfSight = 10;
	unitModel_l._fullReload = 100;
	unitModel_l._windup = 20;
	unitModel_l._defaultAttackMod = AoEModifier(0.5, 2., false);

	lib_p.registerUnitModel("survival_aoe", unitModel_l);
}

void loadSurvivalModels(Library &lib_p)
{
	createSurvivalHealer(lib_p);
	createSurvivalAttackSpeed(lib_p);
	createSurvivalTank(lib_p);
	createSurvivalAoe(lib_p);
}
