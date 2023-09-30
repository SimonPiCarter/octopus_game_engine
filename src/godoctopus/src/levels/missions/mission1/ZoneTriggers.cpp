#include "ZoneTriggers.h"

#include "step/Step.hh"

#include "library/Library.hh"
#include "command/entity/EntityAttackCommand.hh"
#include "command/entity/EntityMoveCommand.hh"
#include "step/command/CommandQueueStep.hh"
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
///    RescueTrigger     ///
////////////////////////////

RescueTrigger::RescueTrigger(std::list<octopus::Listener *> const &listeners_p, octopus::Library const &lib_p,
		std::unordered_set<octopus::Handle> const &heroHandles_p, unsigned long nbPlayer_p, std::vector<octopus::Handle> const &enemies_p) :
	OneShotTrigger(listeners_p),
	_lib(lib_p),
	_heroHandles(heroHandles_p),
	_nbPlayer(nbPlayer_p),
	_enemies(enemies_p)
{}

void RescueTrigger::trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const
{
	// handles for the second zone check
	std::unordered_set<Handle> handles_l = _heroHandles;

	Vector spawn_l {32,13};
	Unit unit_l(spawn_l, false, _lib.getUnitModel("circle"));
	unit_l._player = 0;
	Handle rescue_l = getNextHandle(step_p, state_p);
	handles_l.insert(rescue_l);
	step_p.addSteppable(new UnitSpawnStep(rescue_l, unit_l));
	for(unsigned long i = 0 ; i < _nbPlayer ; ++ i)
	{
		step_p.addSteppable(new godot::CameraStep(to_int(spawn_l.x), to_int(spawn_l.y), i));
	}
	step_p.addSteppable(new godot::DialogStep("mission1_rescue"));

	// move step of rescue
	step_p.addSteppable(new CommandSpawnStep(new EntityMoveCommand(rescue_l, rescue_l, Vector(22, 12), 0, {Vector(22, 12)}, true, true)));

	for(Handle ent_l : _enemies)
	{
		// attack command for the ennemis
		step_p.addSteppable(new CommandSpawnStep(new EntityAttackCommand(ent_l, ent_l, rescue_l, false)));
	}

	// spawn next trigger
	step_p.addSteppable(new TriggerSpawn(
		new FirstAllyTrigger({new ListenerEntityInBox(handles_l, Vector(59,11), Vector(11,8))}, _lib, handles_l, _nbPlayer)));
}

////////////////////////////
///  FirstAllyTrigger    ///
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
			Unit unit_l(firstZoneSpawn_l+Vector(0,u), false, _lib.getUnitModel("circle"));
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
///  BaseDiscoverTrigger ///
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
