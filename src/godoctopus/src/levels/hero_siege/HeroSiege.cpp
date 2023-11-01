#include "HeroSiege.h"

#include <iostream>
#include <fstream>
#include <random>

// fas
#include "library/utils/Randomizer.hh"
#include "library/utils/LoseTrigger.hh"
#include "library/model/AnchorTrigger.hh"
#include "library/model/ModelLoader.hh"
#include "library/model/TimerDamage.hh"
#include "library/model/divinity/DivinityModelLoader.hh"

// octopus
#include "controller/trigger/Listener.hh"
#include "command/entity/EntityAttackCommand.hh"
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
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/state/StateTemplePositionAddStep.hh"
#include "step/state/StateWinStep.hh"
#include "step/team/TeamVisionStep.hh"
#include "step/trigger/TriggerSpawn.hh"
#include "utils/Binary.hh"

// godot
#include "controller/step/CameraStep.h"
#include "controller/step/DialogStep.h"

using namespace octopus;
using namespace fas;

namespace godot
{
namespace hero_siege
{

//////////////////////////////////////////
/// Triggers
//////////////////////////////////////////

class WaveSpawn : public octopus::OneShotTrigger
{
public:
	WaveSpawn(octopus::Listener * listener_p, octopus::Library const &lib_p, unsigned long player_p, unsigned long count_p) :
		OneShotTrigger({listener_p}),
		_lib(lib_p),
		_player(player_p),
		_count(count_p)
	{}

	void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const final
	{
		// spawn wave
		for(unsigned long idx_l = 0 ; idx_l < _count ; ++idx_l)
		{
			Unit unit_l({ 25, 50 }, false, _lib.getUnitModel("mob"));
			unit_l._player = _player;

			Handle handle_l = getNextHandle(step_p, state_p);
			step_p.addSteppable(new UnitSpawnStep(handle_l, unit_l));
			step_p.addSteppable(new CommandSpawnStep(new EntityAttackCommand(handle_l, handle_l, Handle(0,0), false)));
		}

		// next wave
		step_p.addSteppable(new TriggerSpawn(new WaveSpawn(new ListenerStepCount(3000), _lib, _player, _count*2)));
	}

private:
	octopus::Library const &_lib;
	unsigned long const _player;
	unsigned long const _count;
};

class ScoreTrigger : public OnEachTrigger
{
public:
	ScoreTrigger(unsigned long player_p, unsigned long playerRes_p, std::string const &res_p, Fixed const &qty_p) :
		OnEachTrigger(new ListenerEntityModelDied(nullptr, player_p)),
		_player(playerRes_p),
		_res(res_p),
		_qty(qty_p)
	{}

	virtual void trigger(State const &state_p, Step &step_p, unsigned long, TriggerData const &) const override
	{
		// add resource for the player
		std::map<std::string, Fixed> map_l;
		map_l[_res] = -_qty;
		step_p.addSteppable(new PlayerSpendResourceStep(_player, map_l));
	}

private:
	unsigned long const _player;
	std::string const _res;
	Fixed const _qty;
};

//////////////////////////////////////////
/// Step generator
//////////////////////////////////////////

std::list<Steppable *> HeroSiegeLevelSteps(Library &lib_p, RandomGenerator &rand_p, unsigned long playerCount_p)
{
	// hero
	UnitModel unitModel_l { false, 0.5, 0.045, 500. };
	unitModel_l._projectile = true;
	unitModel_l._isUnit = true;
	unitModel_l._damage = 50;
	unitModel_l._armor = 0;
	unitModel_l._range = 8;
	unitModel_l._lineOfSight = 20;
	unitModel_l._fullReload = 100;
	unitModel_l._windup = 1;
	unitModel_l._defaultAttackMod = AoEModifier(0.5, 2., false);

	lib_p.registerUnitModel("hero", unitModel_l);

	// mobs
	UnitModel mobModel_l { false, 0.5, 0.03, 40. };
	mobModel_l._isUnit = true;
	mobModel_l._damage = 50;
	mobModel_l._armor = 0;
	mobModel_l._range = 0.5;
	mobModel_l._lineOfSight = 10;
	mobModel_l._fullReload = 142;
	mobModel_l._windup = 20;

	lib_p.registerUnitModel("mob", mobModel_l);

	std::list<Steppable *> spawners_l;

	Unit hero_l({ 17, 50 }, false, lib_p.getUnitModel("hero"));
	Handle handle_l(0);

	for(unsigned long player_l = 0 ; player_l < playerCount_p ; ++player_l)
	{
		spawners_l.push_back(new PlayerSpawnStep(player_l, 0));
		spawners_l.push_back(new UnitSpawnStep(handle_l, hero_l));
		spawners_l.push_back(new godot::CameraStep(to_int(hero_l._pos.x), to_int(hero_l._pos.y), player_l));
		spawners_l.push_back(new TriggerSpawn(new LoseTrigger(new ListenerEntityDied({handle_l}))));
		spawners_l.push_back(new TriggerSpawn(new ScoreTrigger(playerCount_p, player_l, "bloc", 1)));
		handle_l++;
	}

	spawners_l.push_back(new PlayerSpawnStep(playerCount_p, 1));
	spawners_l.push_back(new TriggerSpawn(new WaveSpawn(new ListenerStepCount(500), lib_p, playerCount_p, 5)));

	return spawners_l;
}

//////////////////////////////////////////
/// Command generator
//////////////////////////////////////////

std::list<Command *> HeroSiegeLevelCommands(Library &lib_p, RandomGenerator &rand_p, unsigned long playerCount_p)
{
	std::list<Command *> commands_l;
	return commands_l;
}

/////////////////////////////////////////////
/////////////////////////////////////////////
/// 		read/write for Header		  ///
/////////////////////////////////////////////
/////////////////////////////////////////////

/// @brief write header for classic arena level
void writeHeroSiegeLevelHeader(std::ofstream &file_p, HeroSiegeLevelHeader const &header_p)
{
	file_p.write((char*)&header_p.seed, sizeof(header_p.seed));
	file_p.write((char*)&header_p.playerCount, sizeof(header_p.playerCount));
}

void readHeroSiegeLevelHeader(std::ifstream &file_p, HeroSiegeLevelHeader &header_r)
{
	file_p.read((char*)&header_r.seed, sizeof(header_r.seed));
	file_p.read((char*)&header_r.playerCount, sizeof(header_r.playerCount));
}

std::pair<std::list<Steppable *>, std::list<Command *> > readHeroSiegeLevelHeader(Library &lib_p, std::ifstream &file_p,
	RandomGenerator * &rand_p, HeroSiegeLevelHeader &header_r)
{
	readHeroSiegeLevelHeader(file_p, header_r);

	delete rand_p;
	rand_p = new RandomGenerator(header_r.seed);

	std::pair<std::list<Steppable *>, std::list<Command *> > pair_l;
	pair_l.first = HeroSiegeLevelSteps(lib_p, *rand_p, header_r.playerCount);
	pair_l.second = HeroSiegeLevelCommands(lib_p, *rand_p, header_r.playerCount);
	return pair_l;
}

} // namespace hero_siege
} // namespace godot
