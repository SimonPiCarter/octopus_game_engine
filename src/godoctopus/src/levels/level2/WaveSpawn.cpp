#include "WaveSpawn.h"

// octopus
#include "controller/trigger/Listener.hh"
#include "library/Library.hh"
#include "state/entity/Unit.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/state/StateRemoveConstraintPositionStep.hh"
#include "step/trigger/TriggerSpawn.hh"
#include "step/state/StateWinStep.hh"

// godot
#include "controller/step/WaveStep.h"
#include "UtilsLevel2.h"

using namespace octopus;

namespace godot
{
namespace level2
{

std::vector<octopus::Steppable*> defaultGenerator() { return {new WaveStep()}; }

WaveSpawn::WaveSpawn(Listener * listener_p, WaveInfo const &currentWave_p, bool earlyWave_p,
	Library const &lib_p, RandomGenerator &rand_p, std::list<WaveParam> const &param_p,
    std::function<std::vector<octopus::Steppable *>(void)> waveStepGenerator_p) :
		OneShotTrigger({listener_p}),
		_currentWave(currentWave_p),
		_earlyWave(earlyWave_p),
		_lib(lib_p),
		_rand(rand_p),
		_params(param_p),
		_waveStepGenerator(waveStepGenerator_p)
{}

void WaveSpawn::trigger(State const &state_p, Step &step_p, unsigned long, octopus::TriggerData const &) const
{
	WaveParam const & currentParams_l = _params.front();
	// load content based on early wave boolean
	WaveContentInfo const &content_l = _earlyWave?_currentWave.earlyWave:_currentWave.mainWave;
	// handles to be used if last to trigger win condition
	std::unordered_set<Handle> handles_l;
	for(WaveUnitCount const &unitCount_l : content_l.units)
	{
		std::string modelName_l = unitCount_l.model;

		// spawn the given unit count
		for(int i = 0 ; i < unitCount_l.count ; ++ i)
		{
			Unit unit_l({ currentParams_l.spawnPoint.x+_rand.roll(-5,5), currentParams_l.spawnPoint.y-_rand.roll(-5,5) }, false, _lib.getUnitModel(modelName_l));
			unit_l._player = 1;
			Handle handle_l = getNextHandle(step_p, state_p);
			step_p.addSteppable(new UnitSpawnStep(handle_l, unit_l));
			step_p.addSteppable(new CommandSpawnStep(new EntityAttackMoveCommand(handle_l, handle_l, currentParams_l.targetPoint, 0, {currentParams_l.targetPoint}, true, true )));
			handles_l.insert(handle_l);
		}
	}
	step_p.addSteppable(new StateRemoveConstraintPositionStep(0, currentParams_l.limitX, 0, currentParams_l.limitY, true, true));
	step_p.addSteppable(new StateRemoveConstraintPositionStep(0, currentParams_l.limitY, 0, currentParams_l.limitX, true, false));

	std::vector<octopus::Steppable *> stepsGenerated_l = _waveStepGenerator();
	for(octopus::Steppable *step_l : stepsGenerated_l)
	{
		step_p.addSteppable(step_l);
	}

	if(!_earlyWave)
	{
		// prepare next wave
		std::list<WaveParam> nextParams_l = _params;
		nextParams_l.pop_front();

		if(nextParams_l.empty())
		{
			step_p.addSteppable(new TriggerSpawn(new WinTrigger(0, handles_l)));
		}
		else
		{
			WaveInfo nextWave_l = rollWave(_rand, nextParams_l.front().wavePool);

			step_p.addSteppable(new TriggerSpawn(new WaveSpawn(new ListenerStepCount(nextWave_l.earlyWave.steps), nextWave_l, true, _lib, _rand, nextParams_l, _waveStepGenerator)));
		}
	}
	else
	{
		// prepare main wave
		step_p.addSteppable(new TriggerSpawn(new WaveSpawn(new ListenerStepCount(_currentWave.mainWave.steps), _currentWave, false, _lib, _rand, _params, _waveStepGenerator)));
	}
}

WinTrigger::WinTrigger(unsigned long winner_p, std::unordered_set<octopus::Handle> const &handles_p)
	: octopus::OneShotTrigger({new octopus::ListenerEntityDied(handles_p)})
	, _winner(winner_p)
{}

void WinTrigger::trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const
{
	step_p.addSteppable(new StateWinStep(state_p.isOver(), state_p.hasWinningTeam(), state_p.getWinningTeam(), _winner));
}

} // namespace level2
} // namespace godot
