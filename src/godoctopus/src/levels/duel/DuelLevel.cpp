#include "DuelLevel.h"

#include <iostream>
#include <fstream>
#include <random>

// fas
#include "library/model/AnchorDivinityTrigger.hh"
#include "library/model/ModelLoader.hh"
#include "library/model/TimerDamage.hh"

// octopus
#include "controller/trigger/Listener.hh"
#include "command/entity/EntityAttackMoveCommand.hh"
#include "command/spawner/AreaSpawnerCommand.hh"
#include "library/Library.hh"
#include "state/entity/Building.hh"
#include "state/entity/Unit.hh"
#include "state/entity/Resource.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/vision/PatternHandler.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/command/flying/FlyingCommandSpawnStep.hh"
#include "step/custom/CustomStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/player/PlayerAddBuildingModel.hh"
#include "step/player/PlayerBuffAllStep.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/state/StateTemplePositionAddStep.hh"
#include "step/state/StateWinStep.hh"
#include "step/team/TeamVisionStep.hh"
#include "step/trigger/TriggerSpawn.hh"

// godot
#include "controller/step/CameraStep.h"
#include "controller/step/DialogStep.h"

using namespace octopus;


namespace godot
{
namespace duellevel
{

class LoseStepTrigger : public octopus::OneShotTrigger
{
public:
	LoseStepTrigger(unsigned long stepWait_p) : octopus::OneShotTrigger({new ListenerStepCount(stepWait_p)}) {}

	virtual void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override
    {
        step_p.addSteppable(new StateWinStep(state_p.isOver(), state_p.hasWinningTeam(), state_p.getWinningTeam(), 1));
    }
};

void addBuildingPlayer(std::list<Steppable *> &spawners_p, unsigned long player_p, std::vector<fas::DivinityType> const &divinities_p, Library &lib_p)
{
	spawners_p.push_back(new PlayerAddBuildingModel(player_p, lib_p.getBuildingModel("barrack_square")));
	spawners_p.push_back(new PlayerAddBuildingModel(player_p, lib_p.getBuildingModel("barrack_circle")));
	spawners_p.push_back(new PlayerAddBuildingModel(player_p, lib_p.getBuildingModel("barrack_triangle")));
	spawners_p.push_back(new PlayerAddBuildingModel(player_p, lib_p.getBuildingModel("deposit")));
	spawners_p.push_back(new PlayerAddBuildingModel(player_p, lib_p.getBuildingModel("anchor")));

	for(fas::DivinityType div_l : divinities_p)
	{
		std::vector<Steppable *> steps_l = newPlayerBuilding(player_p, div_l, lib_p);
		for(Steppable * step_l : steps_l)
		{
			spawners_p.push_back(step_l);
		}
	}
}

std::list<Steppable *> LevelSteps(Library &lib_p, RandomGenerator &rand_p, int stepCount_p, bool buffProd_p,
	std::vector<fas::DivinityType> const &divinitiesPlayer1_p, std::vector<fas::DivinityType> const &divinitiesPlayer2_p)
{
	loadMinimalModels(lib_p);

	// load divinity library
	fas::loadLibrary(lib_p);

	Building buildingP0_l({20, 10}, true, lib_p.getBuildingModel("gate"));
	Unit unitP0_l({ 17, 10 }, false, lib_p.getUnitModel("worker"));

	Building buildingP1_l({LEVEL_DUEL_SIZE-20+1, LEVEL_DUEL_SIZE-10+1}, true, lib_p.getBuildingModel("gate"));
    buildingP1_l._player = 1;
	Unit unitP1_l({ LEVEL_DUEL_SIZE-17+1, LEVEL_DUEL_SIZE-10+1 }, false, lib_p.getUnitModel("worker"));
    unitP1_l._player = 1;

	long anchor_l = 420;
	std::map<std::string, Fixed> mapRes_l;
    if(stepCount_p>0)
	{
		mapRes_l["bloc"] = -20000;
		mapRes_l["ether"] = -20000;
		mapRes_l["Anchor"] = -stepCount_p;
	}
	else
	{
		mapRes_l["bloc"] = -200;
		mapRes_l["ether"] = -200;
		mapRes_l["Anchor"] = -anchor_l;
	}

	Trigger * triggerLoseP0_l = new LoseTrigger(new ListenerEntityModelDied(&lib_p.getBuildingModel("gate"), 0), 1);
	Trigger * triggerLoseP1_l = new LoseTrigger(new ListenerEntityModelDied(&lib_p.getBuildingModel("gate"), 1), 0);

	Handle handle_l(0);
	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
		new PlayerSpawnStep(2, 2),
		new PlayerSpendResourceStep(0, mapRes_l),
		new PlayerSpendResourceStep(1, mapRes_l),
		new BuildingSpawnStep(handle_l++, buildingP0_l, true),
		new BuildingSpawnStep(handle_l++, buildingP1_l, true),
		new TriggerSpawn(triggerLoseP1_l),
		new TriggerSpawn(triggerLoseP0_l),
		new TriggerSpawn(new AnchorDivinityTrigger(lib_p, rand_p, 0, divinitiesPlayer1_p, 180)),
		new TriggerSpawn(new AnchorDivinityTrigger(lib_p, rand_p, 1, divinitiesPlayer2_p, 180)),
		new FlyingCommandSpawnStep(new TimerDamage(Handle(0), 100, 0, 0, "Anchor", Handle(0))),
		new FlyingCommandSpawnStep(new TimerDamage(Handle(1), 100, 0, 1, "Anchor", Handle(1))),
		new godot::CameraStep(to_int(buildingP0_l._pos.x), to_int(buildingP0_l._pos.y), 0),
		new godot::CameraStep(to_int(buildingP1_l._pos.x), to_int(buildingP1_l._pos.y), 1),
	};

	fas::addBuildingPlayer(spawners_l, 0, divinitiesPlayer1_p, lib_p);
	fas::addBuildingPlayer(spawners_l, 1, divinitiesPlayer2_p, lib_p);

    for(size_t i = 0 ; i < 10 ; ++i)
    {
        spawners_l.push_back(new UnitSpawnStep(handle_l++, unitP0_l));
        spawners_l.push_back(new UnitSpawnStep(handle_l++, unitP1_l));
    }

    if(stepCount_p>0)
    {
        spawners_l.push_back(new TriggerSpawn(new LoseStepTrigger(stepCount_p)));
    }

	if(buffProd_p)
	{
		octopus::TimedBuff buff_l;
		buff_l._id = "model_loading_buff_prod";
		buff_l._type = octopus::TyppedBuff::Type::Production;
		buff_l._coef = 9.;
		spawners_l.push_back(new octopus::PlayerBuffAllStep(0, buff_l, ""));
		spawners_l.push_back(new octopus::PlayerBuffAllStep(1, buff_l, ""));
	}

	return spawners_l;
}

Option mirrorPosition(Option const &option_p)
{
	return Option{LEVEL_DUEL_SIZE - option_p.x - 1, LEVEL_DUEL_SIZE - option_p.y - 1};
}

AreaSpawnerCommand * createArenaSpawnCommmand(Library &lib_p, RandomGenerator &rand_p, unsigned long x, unsigned long y, unsigned long width, unsigned long height,
	size_t nbRes_p, size_t nbAnchorSpot_p, size_t qtyIrium_p, int minTrees_p)
{
	std::list<AreaSpawn> spawners_l;

	Resource res2_l({0,0}, true, lib_p.getResourceModel("resource_bloc"));
	res2_l._resource = 2000.;
	res2_l._player = 2;

	Resource res3_l({0,0}, true, lib_p.getResourceModel("resource_ether"));
	res3_l._resource = 2000.;
	res3_l._player = 2;

	Resource res4_l({0,0}, true, lib_p.getResourceModel("resource_irium"));
	res4_l._resource = qtyIrium_p;
	res4_l._player = 2;

	Building anchorSpot_l({0,0}, true, lib_p.getBuildingModel("anchor_spot"));
	anchorSpot_l._player = 2;

	Building tree_l({0,0}, true, lib_p.getBuildingModel("water_v_up"));
	tree_l._player = 2;

	{
		AreaSpawn area_l;
		area_l.width = width;
		area_l.height = height;
		area_l.x = x;
		area_l.y = y;
		area_l.entities.emplace_back(new Resource(res3_l), nbRes_p);
		area_l.entities.emplace_back(new Resource(res2_l), nbRes_p);
		int nbTrees_l = rand_p.roll(minTrees_p,minTrees_p+3);
		for(int i = 0 ; i < nbTrees_l ; ++ i)
		{
			area_l.entities.emplace_back(new Building(tree_l), nbRes_p);
		}
        if(qtyIrium_p>0)
        {
		    area_l.entities.emplace_back(new Resource(res4_l), 1);
        }
		if(nbAnchorSpot_p>0)
		{
			area_l.entities.emplace_back(new Building(anchorSpot_l), nbAnchorSpot_p);
		}
		spawners_l.push_back(area_l);
	}

	AreaSpawnerCommand * cmd_l = new AreaSpawnerCommand(rand_p, spawners_l);
	cmd_l->setReplicationOption(mirrorPosition);

    return cmd_l;
}

std::list<Command *> LevelCommands(Library &lib_p, RandomGenerator &rand_p)
{
	std::list<Command *> commands_l {
		createArenaSpawnCommmand(lib_p, rand_p, 20, 10, 10, 10, 1, 0, 0, 2),
		createArenaSpawnCommmand(lib_p, rand_p, 0, 0, 50, 50, 1, 1, 500, 5),
		createArenaSpawnCommmand(lib_p, rand_p, 50, 0, 75, 50, 1, 1, 500, 5),
		createArenaSpawnCommmand(lib_p, rand_p, 0, 50, 125, 50, 1, 1, 500, 5),
		createArenaSpawnCommmand(lib_p, rand_p, 0, 100, 125, 50, 1, 1, 500, 5),
		createArenaSpawnCommmand(lib_p, rand_p, 0, 150, 125, 50, 1, 1, 500, 5),
		createArenaSpawnCommmand(lib_p, rand_p, 0, 200, 125, 50, 1, 1, 500, 5),
	};

	return commands_l;
}

namespace
{

void writeDivList(std::ofstream &file_p, std::vector<fas::DivinityType> const &types_p)
{
	size_t size_l = types_p.size();
	file_p.write((char*)&size_l, sizeof(size_l));

	for(size_t i = 0 ; i < size_l ; ++i)
	{
		file_p.write((char*)&types_p.at(i), sizeof(types_p.at(i)));
	}
}

std::vector<fas::DivinityType> readDivList(std::ifstream &file_p)
{
	size_t size_l = 0;
	file_p.read((char*)&size_l, sizeof(size_l));

	std::vector<fas::DivinityType> types_l;

	for(size_t i = 0 ; i < size_l ; ++i)
	{
		fas::DivinityType type_l;
		file_p.read((char*)&type_l, sizeof(type_l));
		types_l.push_back(type_l);
	}

	return types_l;
}

} // namespace


/// @brief write header for classic duel level
void writeLevelHeader(std::ofstream &file_p, DuelLevelHeader const &header_p)
{
    file_p.write((char*)&header_p.seed, sizeof(header_p.seed));
    file_p.write((char*)&header_p.step_count, sizeof(header_p.step_count));
    file_p.write((char*)&header_p.buff_prod, sizeof(header_p.buff_prod));

	writeDivList(file_p, header_p.div_player_1);
	writeDivList(file_p, header_p.div_player_2);
}

/// @brief read header for classic duel level and return a pair of steppable and command
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readLevelHeader(octopus::Library &lib_p, std::ifstream &file_p,
	octopus::RandomGenerator * &rand_p, DuelLevelHeader &header_r)
{
    file_p.read((char*)&header_r.seed, sizeof(header_r.seed));
    file_p.read((char*)&header_r.step_count, sizeof(header_r.step_count));
    file_p.read((char*)&header_r.buff_prod, sizeof(header_r.buff_prod));

	header_r.div_player_1 = readDivList(file_p);
	header_r.div_player_2 = readDivList(file_p);

	delete rand_p;
	rand_p = new octopus::RandomGenerator(header_r.seed);

	std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > pair_l;
	pair_l.first = LevelSteps(lib_p, *rand_p, header_r.step_count, header_r.buff_prod, header_r.div_player_1, header_r.div_player_2);
	pair_l.second = LevelCommands(lib_p, *rand_p);
	return pair_l;
}

/////////////////////////////////////////////
/////////////////////////////////////////////
/// 				Triggers			  ///
/////////////////////////////////////////////
/////////////////////////////////////////////

LoseTrigger::LoseTrigger(Listener * listener_p, unsigned long player_p) : OneShotTrigger({listener_p}), _player(player_p) {}

void LoseTrigger::trigger(State const &state_p, Step &step_p, unsigned long, octopus::TriggerData const &) const
{
	step_p.addSteppable(new StateWinStep(state_p.isOver(), state_p.hasWinningTeam(), state_p.getWinningTeam(), _player));
}

} // namespace duellevel
} // namespace godot
