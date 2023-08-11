#include <iostream>
#include <fstream>
#include <random>

#include "controller/trigger/Trigger.hh"
#include "controller/trigger/Listener.hh"
#include "command/building/BuildingUnitProductionCommand.hh"
#include "command/entity/EntityMoveCommand.hh"
#include "library/Library.hh"
#include "state/entity/Building.hh"
#include "state/entity/Unit.hh"
#include "state/entity/Resource.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/ResourceModel.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/player/PlayerAddBuildingModel.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/trigger/TriggerSpawn.hh"

#include "library/LibraryFillers.hh"

using namespace octopus;

void createWorker(Library &lib_p)
{
	UnitModel unitModel_l { false, 0.5, 0.05, 10. };
	unitModel_l._isUnit = true;
	unitModel_l._isBuilder = true;
	unitModel_l._maxQuantity["bloc"] = 10;
	unitModel_l._maxQuantity["ether"] = 50;
	unitModel_l._maxQuantity["idiem"] = 50;
	unitModel_l._productionTime = 100;
	unitModel_l._cost["bloc"] = 50;
	lib_p.registerUnitModel("unit", unitModel_l);
}

void createSoldier(Library &lib_p)
{
	UnitModel unitModel_l { false, 0.5, 0.05, 10. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 200;
	unitModel_l._cost["bloc"] = 50;
	unitModel_l._cost["ether"] = 20;
	unitModel_l._hpMax = 5000.;
	unitModel_l._damage = 7.;
	lib_p.registerUnitModel("soldier", unitModel_l);
}

void createCommandCenter(Library &lib_p)
{
	BuildingModel buildingModel_l { true, 0.9, 10. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._isStatic = true;
	buildingModel_l._deposit["bloc"] = true;
	buildingModel_l._deposit["ether"] = true;
	buildingModel_l._deposit["idiem"] = true;
	buildingModel_l._deposit["steel"] = true;
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("unit"));
	buildingModel_l._buildingTime = 500;
	buildingModel_l._cost["ether"] = 350;
	lib_p.registerBuildingModel("building", buildingModel_l);
}

void createBarrack(Library &lib_p)
{
	BuildingModel buildingModel_l { true, 0.75, 10. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._isStatic = true;
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel("soldier"));
	buildingModel_l._buildingTime = 200;
	buildingModel_l._cost["ether"] = 150;
	lib_p.registerBuildingModel("barrack", buildingModel_l);
}

void createTemple(Library &lib_p)
{
	BuildingModel buildingModel_l { true, 0.75, 10. };
	buildingModel_l._isBuilding = true;
	buildingModel_l._isStatic = true;
	buildingModel_l._buildingTime = 200;
	buildingModel_l._cost["ether"] = 150;
	lib_p.registerBuildingModel("temple", buildingModel_l);
}

void createResource(Library &lib_p)
{
	ResourceModel resModel_l(1.8, "ether", 1000);
	lib_p.registerResourceModel("resource", resModel_l);
}

void createResourceFood(Library &lib_p)
{
	ResourceModel resModel_l(1.8, "bloc", 1000);
	lib_p.registerResourceModel("resource_bloc", resModel_l);
}

void createResourceEther(Library &lib_p)
{
	ResourceModel resModel_l(1.8, "ether", 1000);
	lib_p.registerResourceModel("resource_ether", resModel_l);
}

class Case4TriggerSpawn : public OneShotTrigger
{
public:
	Case4TriggerSpawn(Listener * listener_p, Library const &lib_p) : OneShotTrigger({listener_p}), _lib(lib_p) {}

	virtual void trigger(State const &state_p, Step &step_p, unsigned long, TriggerData const &) const override
	{
		{
			Unit unit_l({ 5, 35. }, false, _lib.getUnitModel("soldier"));
			unit_l._player = 1;
			Handle handle_l = getNextHandle(step_p, state_p);
			step_p.addSteppable(new UnitSpawnStep(handle_l, unit_l));
			step_p.addSteppable(new CommandSpawnStep(new EntityMoveCommand(handle_l, handle_l, {25., 35.}, 0, {{25., 35.}} )));
		}
		{
			Unit unit_l({ 6, 35. }, false, _lib.getUnitModel("soldier"));
			unit_l._player = 1;
			Handle handle_l = getNextHandle(step_p, state_p);
			step_p.addSteppable(new UnitSpawnStep(handle_l, unit_l));
			step_p.addSteppable(new CommandSpawnStep(new EntityMoveCommand(handle_l, handle_l, {25., 35.}, 0, {{25., 35.}} )));
		}
		{
			Unit unit_l({ 5, 36. }, false, _lib.getUnitModel("soldier"));
			unit_l._player = 1;
			Handle handle_l = getNextHandle(step_p, state_p);
			step_p.addSteppable(new UnitSpawnStep(handle_l, unit_l));
			step_p.addSteppable(new CommandSpawnStep(new EntityMoveCommand(handle_l, handle_l, {25., 35.}, 0, {{25., 35.}} )));
		}
		{
			Unit unit_l({ 6, 36. }, false, _lib.getUnitModel("soldier"));
			unit_l._player = 1;
			Handle handle_l = getNextHandle(step_p, state_p);
			step_p.addSteppable(new UnitSpawnStep(handle_l, unit_l));
			step_p.addSteppable(new CommandSpawnStep(new EntityMoveCommand(handle_l, handle_l, {25., 35.}, 0, {{25., 35.}} )));
		}

		step_p.addSteppable(new TriggerSpawn(new Case4TriggerSpawn(new ListenerStepCount(200), _lib)));
	}
private:
	Library const &_lib;
};

class Case4DivinitiesOptionTrigger : public OnEachTrigger
{
public:
	Case4DivinitiesOptionTrigger(Listener * listener_p, unsigned long player_p) : OnEachTrigger(listener_p), _player(player_p), _gen(42) {}

	virtual void trigger(State const &, Step &step_p, unsigned long, TriggerData const &) const override
	{
		std::vector<std::string> vector_l;
		vector_l.push_back("Divinity_1");
		vector_l.push_back("Divinity_3");
		vector_l.push_back("Divinity_4");
		vector_l.push_back("Divinity_5");

		while(vector_l.size() > 3)
		{
			// remove 1 from the possibilty
			std::uniform_int_distribution<> distrib_l(0, vector_l.size()-1);
			int idx_l = distrib_l(_gen);
			vector_l.erase(vector_l.begin() + idx_l);
		}
	}
private:
	unsigned long const _player;

	// required for random gen in const method
	mutable std::mt19937 _gen;
};

std::list<Steppable *> Case4(Library &lib_p)
{
	createWorker(lib_p);
	createSoldier(lib_p);
	createCommandCenter(lib_p);
	createBarrack(lib_p);
	createTemple(lib_p);
	createResource(lib_p);
	createResourceFood(lib_p);
	createResourceEther(lib_p);

	divinitySwarmFiller(lib_p);
	divinityRaidFiller(lib_p);
	divinityArmorFiller(lib_p);
	divinityFireFiller(lib_p);

	Building building_l({1, 20}, true, lib_p.getBuildingModel("building"));
	Unit unit_l({ 15, 20. }, false, lib_p.getUnitModel("unit"));

	Resource res1_l({20,20}, true, lib_p.getResourceModel("resource_bloc"));
	res1_l._resource = 500.;

	Resource res2_l({21,17}, true, lib_p.getResourceModel("resource_bloc"));
	res2_l._resource = 500.;

	Resource res3_l({21,13}, true, lib_p.getResourceModel("resource"));
	res3_l._resource = 500.;

	Resource res4_l({15,17}, true, lib_p.getResourceModel("resource_ether"));
	res4_l._resource = 500.;

	std::map<std::string, Fixed> mapRes_l;
	mapRes_l["bloc"] = -200;
	mapRes_l["ether"] = -200;

	Trigger * trigger_l = new Case4TriggerSpawn(new ListenerStepCount(200), lib_p);


	Trigger * divTrigger_l = new Case4DivinitiesOptionTrigger(new ListenerEntityModelFinished(&lib_p.getBuildingModel("temple"), 0), 0);


	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("building")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("barrack")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("temple")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("div_armor_well")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("div_fire_well")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("div_raid_well")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("div_swarm_well")),
		new PlayerSpendResourceStep(0, mapRes_l),
		new BuildingSpawnStep(Handle(0), building_l, true),
		new ResourceSpawnStep(Handle(1), res1_l),
		new ResourceSpawnStep(Handle(2), res2_l),
		new ResourceSpawnStep(Handle(3), res3_l),
		new ResourceSpawnStep(Handle(4), res4_l),
		new UnitSpawnStep(Handle(5), unit_l),
		new TriggerSpawn(trigger_l),
		new TriggerSpawn(divTrigger_l)
	};

	return spawners_l;
}
