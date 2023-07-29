#include "ChainingShowcase.hh"

#include <iostream>
#include <fstream>
#include <random>

#include "library/Library.hh"
#include "state/entity/attackModifier/modifiers/ChainingModifier.hh"
#include "state/entity/Unit.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/State.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"

using namespace octopus;

std::list<Steppable *> chainingShowcaseSteps(Library &lib_p)
{
    // square
{
    UnitModel unitModel_l { false, 0.5, 0.05, 120. };
    unitModel_l._isUnit = true;
    unitModel_l._damage = 10;
    unitModel_l._armor = 0;
    unitModel_l._range = 0.5;
    unitModel_l._lineOfSight = 30;
    unitModel_l._fullReload = 100;
    unitModel_l._windup = 20;

    lib_p.registerUnitModel("square", unitModel_l);
}
    // triangle
{
    UnitModel unitModel_l { false, 0.5, 0.045, 50. };
    unitModel_l._isUnit = true;
    unitModel_l._damage = 0;
    unitModel_l._armor = 0;
    unitModel_l._range = 0.5;
    unitModel_l._lineOfSight = 10;
    unitModel_l._fullReload = 30;
    unitModel_l._windup = 20;

    lib_p.registerUnitModel("triangle", unitModel_l);
}

	Unit square1_l({ 15, 20. }, false, lib_p.getUnitModel("square"));
	square1_l._player = 0;
    square1_l._attackMod = ChainingModifier(20, 5, 0.8, 2);

	Unit triangle2_l({ 35, 20. }, false, lib_p.getUnitModel("triangle"));
	triangle2_l._player = 1;

	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
	};

	unsigned long id_l = 0;
    spawners_l.push_back(new UnitSpawnStep(Handle(id_l++), square1_l));

	spawners_l.push_back(new UnitSpawnStep(Handle(id_l++), triangle2_l));
    triangle2_l._pos.x = 37;
	spawners_l.push_back(new UnitSpawnStep(Handle(id_l++), triangle2_l));
    triangle2_l._pos.y = 18;
	spawners_l.push_back(new UnitSpawnStep(Handle(id_l++), triangle2_l));
    triangle2_l._pos.x = 39;
	spawners_l.push_back(new UnitSpawnStep(Handle(id_l++), triangle2_l));
    triangle2_l._pos.y = 16;
	spawners_l.push_back(new UnitSpawnStep(Handle(id_l++), triangle2_l));
    triangle2_l._pos.x = 41;
	spawners_l.push_back(new UnitSpawnStep(Handle(id_l++), triangle2_l));
    triangle2_l._pos.y = 14;
	spawners_l.push_back(new UnitSpawnStep(Handle(id_l++), triangle2_l));

	return spawners_l;
}

std::list<Command *> chainingShowcaseCommands(Library &lib_p)
{
	return {};
}
