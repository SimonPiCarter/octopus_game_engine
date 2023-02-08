#include <iostream>
#include <fstream>

#include "controller/trigger/Trigger.hh"
#include "controller/trigger/Listener.hh"
#include "command/building/BuildingUnitProductionCommand.hh"
#include "library/Library.hh"
#include "state/entity/Building.hh"
#include "state/entity/Unit.hh"
#include "state/entity/Resource.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/player/PlayerAddOptionDivinityStep.hh"
#include "step/trigger/TriggerSpawn.hh"

using namespace octopus;

void createWorker(Library &lib_p)
{
	UnitModel unitModel_l { false, 0.5, 0.05, 10. };
	unitModel_l._isUnit = true;
	unitModel_l._maxQuantity[ResourceType::Food] = 10;
	unitModel_l._maxQuantity[ResourceType::Steel] = 50;
	unitModel_l._productionTime = 100;
	unitModel_l._cost[ResourceType::Food] = 50;
	lib_p.registerUnitModel("unit", unitModel_l);
}

void createSoldier(Library &lib_p)
{
	UnitModel unitModel_l { false, 0.5, 0.05, 10. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 200;
	unitModel_l._cost[ResourceType::Food] = 50;
	unitModel_l._cost[ResourceType::Steel] = 20;
	unitModel_l._hpMax = 5000.;
	unitModel_l._damage = 7.;
	lib_p.registerUnitModel("soldier", unitModel_l);
}

void createCommandCenter(Library &lib_p)
{
	BuildingModel buildingModel_l { true, 0.9, 10. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._isStatic = true;
	buildingModel_l._deposit[ResourceType::Food] = true;
	buildingModel_l._deposit[ResourceType::Steel] = true;
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("unit"));
	buildingModel_l._buildingTime = 500;
	buildingModel_l._cost[ResourceType::Steel] = 350;
	lib_p.registerBuildingModel("building", buildingModel_l);
}

void createBarrack(Library &lib_p)
{
	BuildingModel buildingModel_l { true, 0.75, 10. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._isStatic = true;
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("soldier"));
	buildingModel_l._buildingTime = 200;
	buildingModel_l._cost[ResourceType::Steel] = 150;
	lib_p.registerBuildingModel("barrack", buildingModel_l);
}

void createTemple(Library &lib_p)
{
	BuildingModel buildingModel_l { true, 0.75, 10. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._isStatic = true;
	buildingModel_l._buildingTime = 200;
	buildingModel_l._cost[ResourceType::Steel] = 150;
	lib_p.registerBuildingModel("temple", buildingModel_l);
}

void createResource(Library &lib_p)
{
	EntityModel resModel_l { true, 1.8, 1., 10. };
	resModel_l._isResource = true;
	resModel_l._isStatic = true;
	lib_p.registerEntityModel("resource", resModel_l);
}

class Case4TriggerSpawn : public OneShotTrigger
{
public:
	Case4TriggerSpawn(Listener * listener_p, Library const &lib_p) : OneShotTrigger({listener_p}), _lib(lib_p) {}

	virtual void trigger(Step &step_p) const override
	{
		{
			Unit unit_l({ 5, 35. }, false, _lib.getUnitModel("soldier"));
			unit_l._player = 1;
			step_p.addSteppable(new UnitSpawnStep(unit_l));
		}
		{
			Unit unit_l({ 6, 35. }, false, _lib.getUnitModel("soldier"));
			unit_l._player = 1;
			step_p.addSteppable(new UnitSpawnStep(unit_l));
		}
		{
			Unit unit_l({ 5, 36. }, false, _lib.getUnitModel("soldier"));
			unit_l._player = 1;
			step_p.addSteppable(new UnitSpawnStep(unit_l));
		}
		{
			Unit unit_l({ 6, 36. }, false, _lib.getUnitModel("soldier"));
			unit_l._player = 1;
			step_p.addSteppable(new UnitSpawnStep(unit_l));
		}

		step_p.addSteppable(new TriggerSpawn(new Case4TriggerSpawn(new ListenerStepCount(1000), _lib)));
	}
private:
	Library const &_lib;
};

class Case4DivinitiesOptionTrigger : public OnEachTrigger
{
public:
	Case4DivinitiesOptionTrigger(Listener * listener_p, unsigned long player_p) : OnEachTrigger(listener_p), _player(player_p) {}

	virtual void trigger(Step &step_p) const override
	{
		std::set<DivinityType> set_l;
		set_l.insert(DivinityType::Divinity_1);
		set_l.insert(DivinityType::Divinity_2);
		set_l.insert(DivinityType::Divinity_3);
		step_p.addSteppable(new PlayerAddOptionDivinityStep(_player, set_l));
	}
private:
	unsigned long const _player;
};

std::list<Steppable *> Case4(Library &lib_p)
{
	createWorker(lib_p);
	createSoldier(lib_p);
	createCommandCenter(lib_p);
	createBarrack(lib_p);
	createTemple(lib_p);
	createResource(lib_p);

	Building building_l({1, 20}, true, lib_p.getBuildingModel("building"));
	Unit unit_l({ 15, 20. }, false, lib_p.getUnitModel("unit"));

	Resource res1_l({20,20}, true, lib_p.getEntityModel("resource"));
	res1_l._type = ResourceType::Food;
	res1_l._resource = 500.;

	Resource res2_l({21,17}, true, lib_p.getEntityModel("resource"));
	res2_l._type = ResourceType::Food;
	res2_l._resource = 500.;

	Resource res3_l({21,13}, true, lib_p.getEntityModel("resource"));
	res3_l._type = ResourceType::Steel;
	res3_l._resource = 500.;

	std::map<ResourceType, double> mapRes_l;
	mapRes_l[octopus::ResourceType::Food] = -200;
	mapRes_l[octopus::ResourceType::Steel] = -200;

	Trigger * trigger_l = new Case4TriggerSpawn(new ListenerStepCount(1000), lib_p);

	Trigger * divTrigger_l = new Case4DivinitiesOptionTrigger(new ListenerEntityModelFinished(&lib_p.getBuildingModel("building"), 0), 0);

	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("building")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("barrack")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("temple")),
		new PlayerSpendResourceStep(0, mapRes_l),
		new BuildingSpawnStep(building_l, true),
		new ResourceSpawnStep(res1_l),
		new ResourceSpawnStep(res2_l),
		new ResourceSpawnStep(res3_l),
		new UnitSpawnStep(unit_l),
		new TriggerSpawn(trigger_l),
		new TriggerSpawn(divTrigger_l)
	};

	return spawners_l;
}
