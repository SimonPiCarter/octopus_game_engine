#include "ArenaLevel.hh"

#include <iostream>
#include <fstream>
#include <random>

#include "library/model/ModelLoader.hh"

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
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/state/StateWinStep.hh"
#include "step/trigger/TriggerSpawn.hh"
#include "step/team/TeamVisionStep.hh"

using namespace octopus;

std::list<Steppable *> ArenaLevelSteps(Library &lib_p, size_t number_p)
{
	loadModels(lib_p);

	Unit square1_l({ 15, 20. }, false, lib_p.getUnitModel("square"));
	square1_l._player = 0;
	Unit square2_l({ 35, 20. }, false, lib_p.getUnitModel("square"));
	square2_l._player = 1;

	Unit triangle1_l({ 15, 20. }, false, lib_p.getUnitModel("triangle"));
	triangle1_l._player = 0;
	Unit triangle2_l({ 35, 20. }, false, lib_p.getUnitModel("triangle"));
	triangle2_l._player = 1;

	Unit circle1_l({ 15, 20. }, false, lib_p.getUnitModel("circle"));
	circle1_l._player = 0;
	Unit circle2_l({ 35, 20. }, false, lib_p.getUnitModel("circle"));
	circle2_l._player = 1;

	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
	};

	unsigned long id_l = 0;
	for(size_t i = 0; i < number_p ; ++ i)
	{
		spawners_l.push_back(new UnitSpawnStep(id_l++, square1_l));
		spawners_l.push_back(new UnitSpawnStep(id_l++, circle2_l));
	}

	return spawners_l;
}

std::list<octopus::Steppable *> ArenaLevelSteps(octopus::Library &lib_p, std::vector<ArenaInfo> const &you_p, std::vector<ArenaInfo> const &them_p)
{
	loadModels(lib_p);
	PatternHandler patternHandler_l;

    octopus::PatternHandler handler_l;
	octopus::VisionPattern pattern_l = handler_l.getPattern(50);
	for(std::pair<long, long> &pair_l : pattern_l)
	{
		pair_l.first += 25;
		pair_l.second += 20;
	}


	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
		new TeamVisionStep(0, pattern_l, true, false),
		new TeamVisionStep(0, pattern_l, true, true),
	};

	unsigned long id_l = 0;

	for(size_t i = 0 ; i < you_p.size() ; ++ i )
	{
		Unit unit_l({ 15, 20. }, false, lib_p.getUnitModel(you_p.at(i).model));
		unit_l._player = 0;

		for(size_t j = 0; j < you_p.at(i).nb ; ++ j)
		{
			spawners_l.push_back(new UnitSpawnStep(id_l++, unit_l));
		}
	}

	for(size_t i = 0 ; i < them_p.size() ; ++ i )
	{
		Unit unit_l({ 35, 20. }, false, lib_p.getUnitModel(them_p.at(i).model));
		unit_l._player = 1;

		for(size_t j = 0; j < them_p.at(i).nb ; ++ j)
		{
			spawners_l.push_back(new UnitSpawnStep(id_l++, unit_l));
		}
	}

	return spawners_l;
}

std::list<octopus::Steppable *> ArenaKamikazeSteps(octopus::Library &lib_p, size_t numberYou_p, size_t numberThem_p, bool fast_p)
{
	PatternHandler patternHandler_l;

    octopus::PatternHandler handler_l;
	octopus::VisionPattern pattern_l = handler_l.getPattern(50);
	for(std::pair<long, long> &pair_l : pattern_l)
	{
		pair_l.first += 25;
		pair_l.second += 20;
	}

	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
		new TeamVisionStep(0, pattern_l, true, false),
		new TeamVisionStep(0, pattern_l, true, true),
	};

    // square (kamikaze)
{
    UnitModel unitModel_l { false, 0.5, 0.035, 30. };
	if (fast_p)
	{
		unitModel_l._stepSpeed = 0.0413;
	}
    unitModel_l._isUnit = true;
    unitModel_l._damage = 35;
    unitModel_l._armor = 0;
    unitModel_l._range = 0.5;
    unitModel_l._lineOfSight = 10;
    unitModel_l._fullReload = 30.;
    unitModel_l._windup = 20;

    lib_p.registerUnitModel("square", unitModel_l);
}
    // triangle
{
    UnitModel unitModel_l { false, 0.5, 0.0315, 55. };
    unitModel_l._isUnit = true;
    unitModel_l._damage = 6;
    unitModel_l._armor = 0;
    unitModel_l._range = 5;
    unitModel_l._lineOfSight = 10;
    unitModel_l._fullReload = 61.;
    unitModel_l._windup = 20;

    lib_p.registerUnitModel("triangle", unitModel_l);
}

	Unit triangle_l({ 15, 20. }, false, lib_p.getUnitModel("triangle"));
	triangle_l._player = 0;

	Unit square_l({ 35, 20. }, false, lib_p.getUnitModel("square"));
	square_l._player = 1;
	CompositeModifier mod_l;
	mod_l._modifiers.emplace_back(NoModifier());
	mod_l._modifiers.emplace_back(AoEModifier(1., 2.2));
	mod_l._modifiers.emplace_back(SelfDamageModifier(100,0.,0.,true));
    square_l._attackMod = mod_l;

	unsigned long id_l = 0;
	for(size_t i = 0; i < numberYou_p ; ++ i)
	{
		spawners_l.push_back(new UnitSpawnStep(id_l++, triangle_l));
	}
	for(size_t i = 0; i < numberThem_p ; ++ i)
	{
		spawners_l.push_back(new UnitSpawnStep(id_l, square_l));
		spawners_l.push_back(new CommandSpawnStep(new EntityAttackMoveCommand(id_l, id_l, { 15, 20. }, 0, {{ 15, 20. }}, true)));
		++id_l;
	}
	return spawners_l;
}

std::list<Command *> ArenaLevelCommands(Library &)
{
	return {};
}
