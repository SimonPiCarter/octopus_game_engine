#include <iostream>
#include <fstream>
#include <random>

#include "controller/trigger/Trigger.hh"
#include "controller/trigger/Listener.hh"
#include "command/building/BuildingUnitProductionCommand.hh"
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
#include "step/state/StateDrawStep.hh"
#include "step/state/StateTemplePositionAddStep.hh"
#include "step/state/StateWinStep.hh"

#include "library/LibraryFillers.hh"

using namespace octopus;


void createResource(Library &lib_p)
{
	ResourceModel resModel_l(1.8, "steel", 1000);
	lib_p.registerResourceModel("resource_steel", resModel_l);
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

void createResourceGas(Library &lib_p)
{
	ResourceModel resModel_l(1.8, "gas", 1000);
	lib_p.registerResourceModel("resource_gas", resModel_l);
}

class Case5DivinitiesOptionTrigger : public OnEachTrigger
{
public:
	Case5DivinitiesOptionTrigger(Listener * listener_p, unsigned long player_p) : OnEachTrigger(listener_p), _player(player_p), _gen(42) {}

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

class Case5WinTrigger : public OneShotTrigger
{
public:
	Case5WinTrigger(Listener * listener_p, unsigned long team_p) : OneShotTrigger({listener_p}), _team(team_p) {}

	virtual void trigger(State const &state_p, Step &step_p, unsigned long, TriggerData const &) const override
	{
		step_p.addSteppable(state_p, new StateWinStep(state_p.isOver(), state_p.hasWinningTeam(), state_p.getWinningTeam(), _team));
	}
private:
	unsigned long const _team;
};

class Case5DrawTrigger : public OneShotTrigger
{
public:
	Case5DrawTrigger(Listener * listener_p) : OneShotTrigger({listener_p}) {}

	virtual void trigger(State const &state_p, Step &step_p, unsigned long, TriggerData const &) const override
	{
		step_p.addSteppable(state_p, new StateDrawStep(state_p.isOver(), state_p.hasWinningTeam()));
	}
};

std::list<Steppable *> Case5(Library &lib_p)
{
	createResource(lib_p);
	createResourceFood(lib_p);
	createResourceEther(lib_p);
	createResourceGas(lib_p);

	divinitySwarmFiller(lib_p);
	divinityRaidFiller(lib_p);
	divinityArmorFiller(lib_p);
	divinityFireFiller(lib_p);
	neutralFiller(lib_p);

	// spawning
	Building building_l({14, 20}, true, lib_p.getBuildingModel("command_center"));
	Unit unit_l({ 18, 20. }, false, lib_p.getUnitModel("worker"));

	Resource res1_l({20,20}, true, lib_p.getResourceModel("resource_bloc"));
	res1_l._resource = 500.;

	Resource res2_l({21,17}, true, lib_p.getResourceModel("resource_bloc"));
	res2_l._resource = 500.;

	Resource res3_l({21,13}, true, lib_p.getResourceModel("resource_ether"));
	res3_l._resource = 500.;

	Resource res4_l({15,17}, true, lib_p.getResourceModel("resource_ether"));
	res4_l._resource = 500.;

	Resource res5_l({7,17}, true, lib_p.getResourceModel("resource_steel"));
	res5_l._resource = 500.;

	Building abandonnedTemple_l({23, 20}, true, lib_p.getBuildingModel("abandonned_temple"));


	std::map<std::string, Fixed> mapRes_l;
	mapRes_l["steel"] = -1000;
	mapRes_l["idiem"] = -1000;
	mapRes_l["bloc"] = -1000;
	mapRes_l["ether"] = -1000;

	Trigger * divTrigger_l = new Case5DivinitiesOptionTrigger(new ListenerEntityModelFinished(&lib_p.getBuildingModel("temple"), 0), 0);
	Trigger * winTrigger_l = new Case5WinTrigger(new ListenerEntityModelFinished(&lib_p.getBuildingModel("temple"), 0), 0);
	Trigger * loseTrigger_l = new Case5WinTrigger(new ListenerEntityModelFinished(&lib_p.getBuildingModel("ether_deposit"), 0), 1);
	Trigger * drawTrigger_l = new Case5DrawTrigger(new ListenerEntityModelFinished(&lib_p.getBuildingModel("deposit"), 0));

	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("command_center")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("deposit")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("ether_deposit")),
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
		new ResourceSpawnStep(Handle(5), res5_l),
		new UnitSpawnStep(Handle(6), unit_l),
		new UnitSpawnStep(Handle(7), unit_l),
		new UnitSpawnStep(Handle(8), unit_l),
		new UnitSpawnStep(Handle(9), unit_l),
		new UnitSpawnStep(Handle(10), unit_l),
		new TriggerSpawn(divTrigger_l),
		new TriggerSpawn(winTrigger_l),
		new TriggerSpawn(loseTrigger_l),
		new TriggerSpawn(drawTrigger_l),
		new BuildingSpawnStep(Handle(11), abandonnedTemple_l, true),
		new StateTemplePositionAddStep(Vector {23,20})
	};

	return spawners_l;
}
