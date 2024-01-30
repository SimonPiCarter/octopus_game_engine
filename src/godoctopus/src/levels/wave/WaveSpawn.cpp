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
#include "controller/step/DialogStep.h"

using namespace octopus;

namespace godot
{

std::vector<octopus::Steppable*> defaultGenerator() { return {new WaveStep()}; }

WaveSpawn::WaveSpawn(Listener * listener_p, WaveInfo const &currentWave_p, std::vector<octopus::Vector> const &currentSpawnPoint_p, bool earlyWave_p,
	Library const &lib_p, RandomGenerator &rand_p, std::list<WaveParam> const &param_p, unsigned long player_p,
    std::function<std::vector<octopus::Steppable *>(void)> waveStepGenerator_p) :
		OneShotTrigger({listener_p}),
		_currentWave(currentWave_p),
		_currentSpawnPoints(currentSpawnPoint_p),
		_earlyWave(earlyWave_p),
		_lib(lib_p),
		_rand(rand_p),
		_params(param_p),
		_player(player_p),
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
		for(octopus::Vector const &spawnPoint_l : _currentSpawnPoints)
		{
			// spawn the given unit count
			for(int i = 0 ; i < unitCount_l.count ; ++ i)
			{
				Unit unit_l(spawnPoint_l, false, _lib.getUnitModel(modelName_l));
				unit_l._pos.x += _rand.roll(-5,5);
				unit_l._pos.y += _rand.roll(-5,5);
				unit_l._player = _player;
				Handle handle_l = getNextHandle(step_p, state_p);
				step_p.addSteppable(new UnitSpawnStep(handle_l, unit_l));
				step_p.addSteppable(new CommandSpawnStep(new EntityAttackMoveCommand(handle_l, handle_l, currentParams_l.targetPoint, 0, {currentParams_l.targetPoint}, true, true )));
				handles_l.insert(handle_l);
			}
		}
	}


	if(!_earlyWave)
	{
		step_p.addSteppable(new StateRemoveConstraintPositionStep(0, currentParams_l.limitX, currentParams_l.limitYStart, currentParams_l.limitYEnd, true, true));
	}

	if(!_endless)
	{
		step_p.addSteppable(new WaveSpawPointStep(_currentSpawnPoints));
		step_p.addSteppable(new godot::DialogStep(_earlyWave?"early_wave":(_params.size()==1?"last_wave":"big_wave")));
	}

	std::vector<octopus::Steppable *> stepsGenerated_l = _waveStepGenerator();
	for(octopus::Steppable *step_l : stepsGenerated_l)
	{
		step_p.addSteppable(step_l);
	}

	if(!_earlyWave)
	{
		// prepare next wave
		std::list<WaveParam> nextParams_l = _params;
		// if size > 1 or not endless we pop an element
		if(nextParams_l.size() > 1 || !_endless)
		{
			nextParams_l.pop_front();
		}

		if(nextParams_l.empty())
		{
			step_p.addSteppable(new TriggerSpawn(new WinTrigger(0, handles_l)));
		}
		else
		{
			WaveParam const &param_l = nextParams_l.front();
			WaveInfo nextWave_l = rollWave(_rand, param_l.wavePool);
			std::vector<octopus::Vector> rolledSpawns_l = rollSpawnPoints(param_l.spawnPoints, param_l.nSpawnPoints, _rand);

			WaveSpawn *spawn_l = new WaveSpawn(new ListenerStepCount(nextWave_l.earlyWave.steps), nextWave_l, rolledSpawns_l, true,
				_lib, _rand, nextParams_l, _player, _waveStepGenerator);
			spawn_l->setEndless(_endless);
			step_p.addSteppable(new TriggerSpawn(spawn_l));
		}
	}
	else
	{
		// prepare main wave
		WaveSpawn *spawn_l = new WaveSpawn(new ListenerStepCount(_currentWave.mainWave.steps), _currentWave, _currentSpawnPoints, false,
			_lib, _rand, _params, _player, _waveStepGenerator);
		spawn_l->setEndless(_endless);
		step_p.addSteppable(new TriggerSpawn(spawn_l));
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

std::vector<octopus::Vector> rollSpawnPoints(std::vector<octopus::Vector> const &candidates_p, unsigned long number_p, octopus::RandomGenerator &rand_p)
{
	std::vector<octopus::Vector> candidateSpawns_l = candidates_p;
	std::vector<octopus::Vector> rolledSpawns_l;
	unsigned long size_l = std::max<unsigned long>(1, std::min<unsigned long>(candidateSpawns_l.size(), number_p));
	for(unsigned long i = 0 ; i < size_l ; ++i)
	{
		int roll_l = rand_p.roll(0, candidateSpawns_l.size()-1);
		rolledSpawns_l.push_back(candidateSpawns_l[roll_l]);
		candidateSpawns_l.erase(std::next(candidateSpawns_l.begin(), roll_l));
	}
	return rolledSpawns_l;
}

} // namespace godot
