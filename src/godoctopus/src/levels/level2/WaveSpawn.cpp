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

WaveSpawn::WaveSpawn(Listener * listener_p, Library const &lib_p, RandomGenerator &rand_p, std::list<WaveParam> const &param_p,
    std::function<std::vector<octopus::Steppable *>(void)> waveStepGenerator_p) :
		OneShotTrigger({listener_p}),
		_lib(lib_p),
		_rand(rand_p),
		_params(param_p),
		_waveStepGenerator(waveStepGenerator_p)
{}

void WaveSpawn::trigger(State const &state_p, Step &step_p, unsigned long, octopus::TriggerData const &) const
{
	// handles to be used if last to trigger win condition
	std::unordered_set<Handle> handles_l;
	for(unsigned long i = 0 ; i < _params.front().number ; ++ i)
	{
		std::string modelName_l = genModelName(_rand);
		Unit unit_l({ _params.front().spawnPoint.x+_rand.roll(-5,5), _params.front().spawnPoint.y-_rand.roll(-5,5) }, false, _lib.getUnitModel(modelName_l));
		unit_l._player = 1;
		Handle handle_l = getNextHandle(step_p, state_p);
		step_p.addSteppable(new UnitSpawnStep(handle_l, unit_l));
		step_p.addSteppable(new CommandSpawnStep(new EntityAttackMoveCommand(handle_l, handle_l, _params.front().targetPoint, 0, {_params.front().targetPoint}, true, true )));
		handles_l.insert(handle_l);
	}
	step_p.addSteppable(new StateRemoveConstraintPositionStep(0, _params.front().limitX, 0, _params.front().limitY, true, true));
	step_p.addSteppable(new StateRemoveConstraintPositionStep(0, _params.front().limitY, 0, _params.front().limitX, true, false));

	std::vector<octopus::Steppable *> stepsGenerated_l = _waveStepGenerator();
	for(octopus::Steppable *step_l : stepsGenerated_l)
	{
		step_p.addSteppable(step_l);
	}
	std::list<WaveParam> nextParams_l = _params;
    nextParams_l.pop_front();

    if(nextParams_l.empty())
    {
        step_p.addSteppable(new TriggerSpawn(new WinTrigger(0, handles_l)));
    }
    else
    {
	    step_p.addSteppable(new TriggerSpawn(new WaveSpawn(new ListenerStepCount(nextParams_l.front().stepWait), _lib, _rand, nextParams_l, _waveStepGenerator)));
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
