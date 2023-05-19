#include "ModelLoader.hh"

#include "library/Library.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/TempleModel.hh"

using namespace octopus;

void createWorker(Library &lib_p)
{
	// worker
	UnitModel unitModel_l { false, 0.5, 0.025, 50. };
	unitModel_l._isUnit = true;
	unitModel_l._isBuilder = true;
	unitModel_l._productionTime = 2000;
	unitModel_l._timeToHarvest = 100;
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
	unitModel_l._lineOfSight = 5;
	unitModel_l._fullReload = 500;
	unitModel_l._windup = 10;

	lib_p.registerUnitModel("worker", unitModel_l);
}

void createSquare(Library &lib_p)
{
	// square
	UnitModel unitModel_l { false, 0.5, 0.05, 120 };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 2300;
	unitModel_l._cost[ResourceType::Food] = 100;
	unitModel_l._cost[ResourceType::Steel] = 20;
	unitModel_l._damage = 9;
	unitModel_l._armor = 0;
	unitModel_l._range = 0.5;
	unitModel_l._lineOfSight = 10;
	unitModel_l._fullReload = 155;
	unitModel_l._windup = 20;

	unitModel_l._bonusDamage["triangle"] = 14;
	unitModel_l._bonusDamage["reverse_triangle"] = 14;
	unitModel_l._bonusDamage["long_square"] = 20;
	unitModel_l._bonusDamage["long_triuangle"] = 20;

	lib_p.registerUnitModel("square", unitModel_l);
}

void createDoubleSquare(Library &lib_p)
{
	// square
	UnitModel unitModel_l { false, 0.5, 0.045, 155 };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 1500;
	unitModel_l._cost[ResourceType::Food] = 100;
	unitModel_l._cost[ResourceType::Steel] = 20;
	unitModel_l._damage = 12;
	unitModel_l._armor = 4;
	unitModel_l._range = 0.5;
	unitModel_l._lineOfSight = 10;
	unitModel_l._fullReload = 117;
	unitModel_l._windup = 20;

	unitModel_l._bonusDamage["long_square"] = 20;
	unitModel_l._bonusDamage["long_triuangle"] = 20;

	lib_p.registerUnitModel("double_square", unitModel_l);
}

void createLongSquare(Library &lib_p)
{
	// square
	UnitModel unitModel_l { false, 0.5, 0.025, 140 };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 4000;
	unitModel_l._cost[ResourceType::Food] = 200;
	unitModel_l._cost[ResourceType::Steel] = 400;
	unitModel_l._damage = 36;
	unitModel_l._armor = 20;
	unitModel_l._range = 8;
	unitModel_l._lineOfSight = 10;
	unitModel_l._fullReload = 665;
	unitModel_l._windup = 20;
	unitModel_l._defaultAttackMod = AoEModifier(0.5, 2., false);

	unitModel_l._bonusDamage["triangle"] = 18;
	unitModel_l._bonusDamage["reverse_triangle"] = 18;

	lib_p.registerUnitModel("long_square", unitModel_l);
}

void createReverseTriangle(Library &lib_p)
{
	// square
	UnitModel unitModel_l { false, 0.5, 0.035, 80. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 2300;
	unitModel_l._cost[ResourceType::Food] = 80;
	unitModel_l._cost[ResourceType::Steel] = 40;
	unitModel_l._damage = 12;
	unitModel_l._armor = 0;
	unitModel_l._range = 8;
	unitModel_l._lineOfSight = 10;
	unitModel_l._fullReload = 192;
	unitModel_l._windup = 20;

	unitModel_l._bonusDamage["double_square"] = 11;

	lib_p.registerUnitModel("reverse_triangle", unitModel_l);
}

void createTriangle(Library &lib_p)
{
	// square
	UnitModel unitModel_l { false, 0.5, 0.045, 70. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 1500;
	unitModel_l._cost[ResourceType::Food] = 30;
	unitModel_l._cost[ResourceType::Steel] = 50;
	unitModel_l._damage = 5;
	unitModel_l._armor = 0;
	unitModel_l._range = 5;
	unitModel_l._lineOfSight = 10;
	unitModel_l._fullReload = 142;
	unitModel_l._windup = 20;

	unitModel_l._bonusDamage["circle"] = 5;

	lib_p.registerUnitModel("triangle", unitModel_l);
}

void createLongTriangle(Library &lib_p)
{
	// square
	UnitModel unitModel_l { false, 0.5, 0.025, 125 };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 3000;
	unitModel_l._cost[ResourceType::Food] = 250;
	unitModel_l._cost[ResourceType::Steel] = 250;
	unitModel_l._damage = 30;
	unitModel_l._armor = 20;
	unitModel_l._range = 10;
	unitModel_l._lineOfSight = 10;
	unitModel_l._fullReload = 395;
	unitModel_l._windup = 20;

	unitModel_l._bonusDamage["long_square"] = 70;
	unitModel_l._bonusDamage["long_triangle"] = 70;

	lib_p.registerUnitModel("long_triangle", unitModel_l);
}

void createCircle(Library &lib_p)
{
	// square
	UnitModel unitModel_l { false, 0.5, 0.05, 90. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 1500;
	unitModel_l._cost[ResourceType::Food] = 60;
	unitModel_l._cost[ResourceType::Steel] = 20;
	unitModel_l._damage = 8;
	unitModel_l._armor = 3;
	unitModel_l._range = 0.5;
	unitModel_l._lineOfSight = 10;
	unitModel_l._fullReload = 167;
	unitModel_l._windup = 20;

	unitModel_l._bonusDamage["square"] = 17;

	lib_p.registerUnitModel("circle", unitModel_l);
}

void createCommandCenter(Library &lib_p)
{
	BuildingModel buildingModel_l { true, 1.9, 5000. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._isStatic = true;
	buildingModel_l._productionOutput = {2, 0};
	buildingModel_l._deposit[ResourceType::Food] = true;
	buildingModel_l._deposit[ResourceType::Steel] = true;
	buildingModel_l._deposit[ResourceType::Ether] = true;
	buildingModel_l._deposit[ResourceType::Gas] = true;
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("worker"));
	buildingModel_l._buildingTime = 500;
	buildingModel_l._cost[ResourceType::Food] = 250;
	buildingModel_l._cost[ResourceType::Steel] = 550;
	lib_p.registerBuildingModel("command_center", buildingModel_l);
}

void createBarrackSquare(Library &lib_p)
{
	BuildingModel buildingModel_l { true, 0.9, 1000. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._isStatic = true;
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("square"));
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("long_square"));
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("double_square"));
	buildingModel_l._buildingTime = 1000;
	buildingModel_l._cost[ResourceType::Steel] = 150;
	lib_p.registerBuildingModel("barrack_square", buildingModel_l);
}

void createBarrackCircle(Library &lib_p)
{
	BuildingModel buildingModel_l { true, 0.9, 1000. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._isStatic = true;
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("circle"));
	buildingModel_l._buildingTime = 1000;
	buildingModel_l._cost[ResourceType::Steel] = 150;
	lib_p.registerBuildingModel("barrack_circle", buildingModel_l);
}

void createBarrackTriangle(Library &lib_p)
{
	BuildingModel buildingModel_l { true, 0.9, 1000. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._isStatic = true;
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("triangle"));
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("reverse_triangle"));
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("long_triangle"));
	buildingModel_l._buildingTime = 1000;
	buildingModel_l._cost[ResourceType::Steel] = 150;
	lib_p.registerBuildingModel("barrack_triangle", buildingModel_l);
}

void createDeposit(Library &lib_p)
{
	BuildingModel buildingModel_l { true, 0.9, 500. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._isStatic = true;
	buildingModel_l._buildingTime = 100;
	buildingModel_l._cost[ResourceType::Steel] = 50;

	buildingModel_l._deposit[ResourceType::Food] = true;
	buildingModel_l._deposit[ResourceType::Steel] = true;

	lib_p.registerBuildingModel("deposit", buildingModel_l);
}

void createAnchor(Library &lib_p)
{
	// temple
	TempleModel buildingModel_l { true, 0.9, 1000. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._isStatic = true;
	buildingModel_l._buildingTime = 500;
	buildingModel_l._cost[ResourceType::Steel] = 150;
	buildingModel_l._cost[ResourceType::Food] = 50;
	buildingModel_l._armor = 5;

	lib_p.registerTempleModel("anchor", buildingModel_l);
}

void createAnchorSpot(Library &lib_p)
{
	// temple
	BuildingModel buildingModel_l { true, 0.9, 1000. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._isStatic = true;
	buildingModel_l._invulnerable = true;
	buildingModel_l._isAbandonedTemple = true;
	buildingModel_l._armor = 5;

	lib_p.registerBuildingModel("anchor_spot", buildingModel_l);
}

void createResourceFood(Library &lib_p)
{
	EntityModel resModel_l { true, 0.9, 1., 10. };
	resModel_l._isResource = true;
	resModel_l._isStatic = true;
	lib_p.registerEntityModel("resource_food", resModel_l);
}

void createResourceSteel(Library &lib_p)
{
	EntityModel resModel_l { true, 0.9, 1., 10. };
	resModel_l._isResource = true;
	resModel_l._isStatic = true;
	lib_p.registerEntityModel("resource_steel", resModel_l);
}

void createWater(Library &lib_p)
{
	BuildingModel buildingModel_l { true, 1., 500. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._isStatic = true;
	buildingModel_l._invulnerable = true;
	lib_p.registerBuildingModel("water_h", buildingModel_l);
	lib_p.registerBuildingModel("water_v", buildingModel_l);
	lib_p.registerBuildingModel("water_v_down", buildingModel_l);
	lib_p.registerBuildingModel("water_v_up", buildingModel_l);
	lib_p.registerBuildingModel("water_corner", buildingModel_l);
}

void loadModels(octopus::Library &lib_p)
{
	createWorker(lib_p);
	createSquare(lib_p);
	createDoubleSquare(lib_p);
	createLongSquare(lib_p);
	createReverseTriangle(lib_p);
	createLongTriangle(lib_p);
	createTriangle(lib_p);
	createCircle(lib_p);
	createCommandCenter(lib_p);
	createBarrackSquare(lib_p);
	createBarrackCircle(lib_p);
	createBarrackTriangle(lib_p);
	createDeposit(lib_p);
	createAnchor(lib_p);
	createAnchorSpot(lib_p);
	createWater(lib_p);

	createResourceFood(lib_p);
	createResourceSteel(lib_p);
}
