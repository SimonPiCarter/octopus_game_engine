#include "ZoneTriggers.h"

#include "step/Step.hh"

#include "library/Library.hh"
#include "state/entity/Building.hh"
#include "state/entity/Unit.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/state/StateRemoveConstraintPositionStep.hh"
#include "step/trigger/TriggerSpawn.hh"
#include "controller/trigger/Listener.hh"

// godot
#include "controller/step/CameraStep.h"
#include "controller/step/DialogStep.h"

using namespace octopus;

////////////////////////////
///  First Zone Trigger  ///
////////////////////////////

FirstAllyTrigger::FirstAllyTrigger(std::list<octopus::Listener *> const &listeners_p, octopus::Library const &lib_p, std::unordered_set<Handle> const &heroHandles_p, unsigned long nbPlayer_p) :
	OneShotTrigger(listeners_p),
	_lib(lib_p),
	_heroHandles(heroHandles_p),
	_nbPlayer(nbPlayer_p)
{}

void FirstAllyTrigger::trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const
{
	// handles for the second zone check
	std::unordered_set<Handle> handles_l = _heroHandles;

	Vector firstZoneSpawn_l {82,17};
	for(unsigned long i = 0 ; i < _nbPlayer ; ++ i)
	{
		for(unsigned long u = 0 ; u < 3 ; ++ u)
		{
			Unit unit_l(firstZoneSpawn_l+Vector(0,u), false, _lib.getUnitModel("square"));
			unit_l._player = i;
			handles_l.insert(getNextHandle(step_p, state_p));
			step_p.addSteppable(new UnitSpawnStep(getNextHandle(step_p, state_p), unit_l));
		}

		step_p.addSteppable(new godot::CameraStep(to_int(firstZoneSpawn_l.x), to_int(firstZoneSpawn_l.y), i));
		step_p.addSteppable(new StateRemoveConstraintPositionStep(i, 26, 36, 56, true, false));
	}
	step_p.addSteppable(new godot::DialogStep("mission1_first_zone"));

	// spawn second zone trigger

	step_p.addSteppable(new TriggerSpawn(
		new BaseDiscoverTrigger({new ListenerEntityInBox(handles_l, Vector(58,32), Vector(5,9))}, _lib, _nbPlayer)));
}

////////////////////////////
///  Second Zone Trigger ///
////////////////////////////

BaseDiscoverTrigger::BaseDiscoverTrigger(std::list<octopus::Listener *> const &listeners_p, octopus::Library const &lib_p, unsigned long nbPlayer_p) :
	OneShotTrigger(listeners_p),
	_lib(lib_p),
	_nbPlayer(nbPlayer_p)
{}

void BaseDiscoverTrigger::trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const
{
	// handles for the second zone check

	Vector spawn_l {85,35};
	for(unsigned long i = 0 ; i < _nbPlayer ; ++ i)
	{
		Building building_l(spawn_l+Vector(0,3*i), false, _lib.getBuildingModel("command_center"));
		building_l._player = i;
		step_p.addSteppable(new BuildingSpawnStep(getNextHandle(step_p, state_p), building_l, true));

		step_p.addSteppable(new godot::CameraStep(to_int(spawn_l.x), to_int(spawn_l.y), i));
	}
	step_p.addSteppable(new godot::DialogStep("mission1_second_zone"));
}
