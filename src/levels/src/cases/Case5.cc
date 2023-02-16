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

#include "library/LibraryFillers.hh"

using namespace octopus;


void createResource(Library &lib_p)
{
	EntityModel resModel_l { true, 1.8, 1., 10. };
	resModel_l._isResource = true;
	resModel_l._isStatic = true;
	lib_p.registerEntityModel("resource_steel", resModel_l);
}

void createResourceFood(Library &lib_p)
{
	EntityModel resModel_l { true, 1.8, 1., 10. };
	resModel_l._isResource = true;
	resModel_l._isStatic = true;
	lib_p.registerEntityModel("resource_food", resModel_l);
}

void createResourceEther(Library &lib_p)
{
	EntityModel resModel_l { true, 1.8, 1., 10. };
	resModel_l._isResource = true;
	resModel_l._isStatic = true;
	lib_p.registerEntityModel("resource_ether", resModel_l);
}

void createResourceGas(Library &lib_p)
{
	EntityModel resModel_l { true, 1.8, 1., 10. };
	resModel_l._isResource = true;
	resModel_l._isStatic = true;
	lib_p.registerEntityModel("resource_gas", resModel_l);
}

class Case5DivinitiesOptionTrigger : public OnEachTrigger
{
public:
	Case5DivinitiesOptionTrigger(Listener * listener_p, unsigned long player_p) : OnEachTrigger(listener_p), _player(player_p), _gen(42) {}

	virtual void trigger(Step &step_p) const override
	{
		std::vector<DivinityType> vector_l;
		vector_l.push_back(DivinityType::Divinity_1);
		vector_l.push_back(DivinityType::Divinity_3);
		vector_l.push_back(DivinityType::Divinity_4);
		vector_l.push_back(DivinityType::Divinity_5);

		while(vector_l.size() > 3)
		{
			// remove 1 from the possibilty
			std::uniform_int_distribution<> distrib_l(0, vector_l.size()-1);
			int idx_l = distrib_l(_gen);
			vector_l.erase(vector_l.begin() + idx_l);
		}

		std::set<DivinityType> set_l(vector_l.begin(), vector_l.end());


		step_p.addSteppable(new PlayerAddOptionDivinityStep(_player, set_l));
	}
private:
	unsigned long const _player;

	// required for random gen in const method
	mutable std::mt19937 _gen;
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

	Resource res1_l({20,20}, true, lib_p.getEntityModel("resource_food"));
	res1_l._type = ResourceType::Food;
	res1_l._resource = 500.;

	Resource res2_l({21,17}, true, lib_p.getEntityModel("resource_food"));
	res2_l._type = ResourceType::Food;
	res2_l._resource = 500.;

	Resource res3_l({21,13}, true, lib_p.getEntityModel("resource_steel"));
	res3_l._type = ResourceType::Steel;
	res3_l._resource = 500.;

	Resource res4_l({15,17}, true, lib_p.getEntityModel("resource_ether"));
	res4_l._type = ResourceType::Ether;
	res4_l._resource = 500.;

	Resource res5_l({7,17}, true, lib_p.getEntityModel("resource_gas"));
	res5_l._type = ResourceType::Gas;
	res5_l._resource = 500.;

	std::map<ResourceType, double> mapRes_l;
	mapRes_l[octopus::ResourceType::Food] = -200;
	mapRes_l[octopus::ResourceType::Steel] = -200;

	Trigger * divTrigger_l = new Case5DivinitiesOptionTrigger(new ListenerEntityModelFinished(&lib_p.getBuildingModel("temple"), 0), 0);

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
		new BuildingSpawnStep(building_l, true),
		new ResourceSpawnStep(res1_l),
		new ResourceSpawnStep(res2_l),
		new ResourceSpawnStep(res3_l),
		new ResourceSpawnStep(res4_l),
		new ResourceSpawnStep(res5_l),
		new UnitSpawnStep(unit_l),
		new UnitSpawnStep(unit_l),
		new UnitSpawnStep(unit_l),
		new UnitSpawnStep(unit_l),
		new UnitSpawnStep(unit_l),
		new TriggerSpawn(divTrigger_l)
	};

	return spawners_l;
}
