#include "WaveSpawn.h"

// octopus
#include "controller/trigger/Listener.hh"
#include "library/Library.hh"
#include "state/entity/Unit.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/player/PlayerAddOptionStep.hh"
#include "step/player/PlayerBuffAllStep.hh"
#include "step/state/StateRemoveConstraintPositionStep.hh"
#include "step/trigger/TriggerSpawn.hh"
#include "step/state/StateWinStep.hh"

// fas
#include "library/model/AnchorTrigger.hh"
#include "library/model/wave_buff/WaveBuffGenerator.hh"

// godot
#include "controller/step/WaveStep.h"
#include "controller/step/DialogStep.h"

using namespace octopus;

namespace godot
{

std::vector<octopus::Steppable*> defaultGenerator() { return {new WaveStep()}; }

WaveSpawn::WaveSpawn(Listener * listener_p, WaveInfo const &currentWave_p, std::vector<octopus::Vector> const &currentSpawnPoint_p, bool earlyWave_p,
	Library const &lib_p, RandomGenerator &rand_p, std::list<WaveParam> const &param_p, unsigned long player_p,
    std::vector<unsigned long> players_p, std::function<std::vector<octopus::Steppable *>(void)> waveStepGenerator_p,
	std::vector<fas::SurvivalSpecialType> const &forbidden_p, unsigned long count_p, unsigned long nb_upgrades_p) :
		OneShotTrigger({listener_p}),
		_currentWave(currentWave_p),
		_currentSpawnPoints(currentSpawnPoint_p),
		_earlyWave(earlyWave_p),
		_lib(lib_p),
		_rand(rand_p),
		_params(param_p),
		_playerSpawn(player_p),
		_players(players_p),
		_waveStepGenerator(waveStepGenerator_p),
		_forbidden(forbidden_p),
		_count(count_p),
		_nb_upgrades(nb_upgrades_p)
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
				unit_l._player = _playerSpawn;
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
				_lib, _rand, nextParams_l, _playerSpawn, _players, _waveStepGenerator, _forbidden, _count+1);
			spawn_l->setEndless(_endless);
			step_p.addSteppable(new TriggerSpawn(spawn_l));
		}
	}
	else
	{
		// prepare main wave
		WaveSpawn *spawn_l = new WaveSpawn(new ListenerStepCount(_currentWave.mainWave.steps), _currentWave, _currentSpawnPoints, false,
			_lib, _rand, _params, _playerSpawn, _players, _waveStepGenerator, _forbidden, _count+1);
		spawn_l->setEndless(_endless);
		step_p.addSteppable(new TriggerSpawn(spawn_l));
	}
	// option generator
	step_p.addSteppable(new TriggerSpawn(new WaveClearTrigger(_playerSpawn, _players, _forbidden, handles_l, _lib, _rand, _count, _nb_upgrades)));
}

WinTrigger::WinTrigger(unsigned long winner_p, std::unordered_set<octopus::Handle> const &handles_p)
	: octopus::OneShotTrigger({new octopus::ListenerEntityDied(handles_p)})
	, _winner(winner_p)
{}

void WinTrigger::trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const
{
	step_p.addSteppable(new StateWinStep(state_p.isOver(), state_p.hasWinningTeam(), state_p.getWinningTeam(), _winner));
}

WaveClearTrigger::WaveClearTrigger(
	unsigned long playerSpawn_p,
	std::vector<unsigned long> const & players_p,
	std::vector<fas::SurvivalSpecialType> const &forbidden_p,
	std::unordered_set<octopus::Handle> const &handles_p,
	octopus::Library const &lib_p,
	octopus::RandomGenerator &rand_p,
	unsigned long count_p,
	unsigned long nb_upgrades_p
)
	: octopus::OneShotTrigger({new octopus::ListenerEntityDied(handles_p)})
	, _playerSpawn(playerSpawn_p)
	, _players(players_p)
	, _forbidden(forbidden_p)
	, _lib(lib_p)
	, _rand(rand_p)
	, _count(count_p)
	, _nb_upgrades(nb_upgrades_p)
{}

void WaveClearTrigger::trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const
{
	for(unsigned long player_l : _players)
	{
		unsigned long seed_l = _rand.roll(0,std::numeric_limits<int>::max());
		auto optionsGenerator_l = [seed_l, forbidden_l = _forbidden, player_l, count_l = _count](const octopus::State &state_p) {
			return generateOptions(
						std::string("wave.")+std::to_string(count_l),
						seed_l,
						forbidden_l,
						player_l,
						state_p);
		};

		step_p.addSteppable(new PlayerAddOptionStep(player_l, "0", new BuffGenerator("0", optionsGenerator_l, _lib)));
	}

	for(unsigned long up_l = 0 ; up_l < _nb_upgrades ; ++ up_l)
	{
		std::list<WaveBuffGenerator> waveBuffs_l = generateWaveBuffGenerators(_count+1, up_l);
		std::vector<WaveBuffGenerator> buffs_l = getWaveBuffGenerator(_rand, waveBuffs_l, waveBuffs_l.size());
		for(WaveBuffGenerator const &buffGen_l : buffs_l)
		{
			std::string model_l = buffGen_l.model;
			octopus::TimedBuff buff_l = buffGen_l.buff;
			step_p.addSteppable(new PlayerBuffAllStep(_playerSpawn, buff_l, model_l));
		}
	}
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
