#include "LifestealShowcase.hh"

#include <iostream>
#include <fstream>
#include <random>

#include "library/Library.hh"
#include "state/entity/attackModifier/modifiers/LifeStealModifier.hh"
#include "state/entity/Unit.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/State.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"

using namespace octopus;

std::list<Steppable *> lifestealShowcaseSteps(Library &lib_p, uint32_t number_p)
{
    // square
{
    UnitModel unitModel_l { false, 0.5, 0.05, 100. };
    unitModel_l._isUnit = true;
    unitModel_l._damage = 10;
    unitModel_l._armor = 0;
    unitModel_l._range = 0.5;
    unitModel_l._lineOfSight = 20;
    unitModel_l._fullReload = 141;
    unitModel_l._windup = 20;

    lib_p.registerUnitModel("square", unitModel_l);
}
    // triangle
{
    UnitModel unitModel_l { false, 0.5, 0.045, 100. };
    unitModel_l._isUnit = true;
    unitModel_l._damage = 10;
    unitModel_l._armor = 0;
    unitModel_l._range = 0.5;
    unitModel_l._lineOfSight = 10;
    unitModel_l._fullReload = 100;
    unitModel_l._windup = 20;

    lib_p.registerUnitModel("triangle", unitModel_l);
}

	Unit square1_l({ 15, 20. }, false, lib_p.getUnitModel("square"));
	square1_l._player = 0;
    square1_l._attackMod = LifeStealModifier(0.8);

	Unit triangle2_l({ 35, 20. }, false, lib_p.getUnitModel("triangle"));
	triangle2_l._player = 1;

	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
	};

	unsigned long id_l = 0;
    spawners_l.push_back(new UnitSpawnStep(Handle(id_l++), square1_l));
	for(uint32_t i = 0; i < number_p ; ++ i)
	{
		spawners_l.push_back(new UnitSpawnStep(Handle(id_l++), triangle2_l));
	}

	return spawners_l;
}

std::list<Command *> lifestealShowcaseCommands(Library &lib_p)
{
	return {};
}
