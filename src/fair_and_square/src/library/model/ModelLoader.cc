#include "ModelLoader.hh"

#include "library/Library.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/model/entity/BuildingModel.hh"

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
    unitModel_l._fullReload = 500.;
    unitModel_l._windup = 10.;

    lib_p.registerUnitModel("worker", unitModel_l);
}

void createSquare(Library &lib_p)
{
    // square
    UnitModel unitModel_l { false, 0.5, 0.075, 120. };
    unitModel_l._isUnit = true;
    unitModel_l._productionTime = 2500;
    unitModel_l._cost[ResourceType::Food] = 25;
    unitModel_l._cost[ResourceType::Steel] = 25;
    unitModel_l._damage = 10;
    unitModel_l._armor = 0;
    unitModel_l._range = 0.5;
    unitModel_l._fullReload = 100.;
    unitModel_l._windup = 50;

    unitModel_l._bonusDamage["triangle"] = 5.;

    lib_p.registerUnitModel("square", unitModel_l);
}

void createTriangle(Library &lib_p)
{
    // square
    UnitModel unitModel_l { false, 0.5, 0.045, 50. };
    unitModel_l._isUnit = true;
    unitModel_l._productionTime = 2500;
    unitModel_l._cost[ResourceType::Food] = 10;
    unitModel_l._cost[ResourceType::Steel] = 40;
    unitModel_l._damage = 5;
    unitModel_l._armor = 0;
    unitModel_l._range = 5;
    unitModel_l._fullReload = 100.;
    unitModel_l._windup = 50;

    unitModel_l._bonusDamage["circle"] = 5.;

    lib_p.registerUnitModel("triangle", unitModel_l);
}

void createCircle(Library &lib_p)
{
    // square
    UnitModel unitModel_l { false, 0.5, 0.05, 100. };
    unitModel_l._isUnit = true;
    unitModel_l._productionTime = 2500;
    unitModel_l._cost[ResourceType::Food] = 40;
    unitModel_l._cost[ResourceType::Steel] = 10;
    unitModel_l._damage = 10;
    unitModel_l._armor = 3;
    unitModel_l._range = 0.1;
    unitModel_l._fullReload = 100.;
    unitModel_l._windup = 50;

    unitModel_l._bonusDamage["square"] = 10.;

    lib_p.registerUnitModel("circle", unitModel_l);
}

void createCommandCenter(Library &lib_p)
{
	BuildingModel buildingModel_l { true, 1.9, 5000. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._isStatic = true;
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

void createBarrack(Library &lib_p)
{
	BuildingModel buildingModel_l { true, 0.9, 1000. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._isStatic = true;
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("square"));
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("circle"));
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("triangle"));
	buildingModel_l._buildingTime = 200;
	buildingModel_l._cost[ResourceType::Steel] = 150;
	lib_p.registerBuildingModel("barrack", buildingModel_l);
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

void createResourceFood(Library &lib_p)
{
	EntityModel resModel_l { true, 0.9, 1., 10. };
	resModel_l._isResource = true;
	resModel_l._isStatic = true;
	lib_p.registerEntityModel("resource_food", resModel_l);
}

void createResourceSteel(Library &lib_p)
{
	EntityModel resModel_l { true, 1.8, 1., 10. };
	resModel_l._isResource = true;
	resModel_l._isStatic = true;
	lib_p.registerEntityModel("resource_steel", resModel_l);
}

void loadModels(octopus::Library &lib_p)
{
	createWorker(lib_p);
	createSquare(lib_p);
	createTriangle(lib_p);
	createCircle(lib_p);
	createCommandCenter(lib_p);
	createBarrack(lib_p);
    createDeposit(lib_p);

	createResourceFood(lib_p);
	createResourceSteel(lib_p);
}