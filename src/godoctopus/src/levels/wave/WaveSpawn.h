#ifndef __WaveSpawnLevel2__
#define __WaveSpawnLevel2__

#include <list>
#include <unordered_set>
#include <vector>
#include <functional>
#include "controller/trigger/Trigger.hh"
#include "utils/RandomGenerator.hh"
#include "utils/Vector.hh"

#include "library/model/bonus/BuffGenerators.hh"

#include "WaveUnitCount.h"

namespace octopus
{
	class Library;
	class State;
	class Step;
	class Steppable;
} // namespace octopus

namespace godot
{

std::vector<octopus::Steppable*> defaultGenerator();

struct WaveParam
{
	/// @brief points where the unit will spawn
	std::vector<octopus::Vector> spawnPoints;
	/// @brief point where the unit will attack
	octopus::Vector targetPoint;
	/// @brief limits to remove
	unsigned long limitX;
	unsigned long limitYStart;
	unsigned long limitYEnd;
	/// @brief wave pool info used to generate the wave
	WavePoolInfo wavePool;
	/// @brief number of spawn points to roll each wave
	unsigned long nSpawnPoints = 1;
};

class WaveSpawn : public octopus::OneShotTrigger
{
public:
	WaveSpawn(octopus::Listener * listener_p, WaveInfo const &currentWave_p, std::vector<octopus::Vector> const &currentSpawnPoint_p, bool earlyWave_p,
		octopus::Library const &lib_p, octopus::RandomGenerator &rand_p, std::list<WaveParam> const &param_p, unsigned long playerSpawn_p,
		std::vector<unsigned long> players_p, std::function<std::vector<octopus::Steppable *>(void)> waveStepGenerator_p,
		std::vector<fas::SurvivalSpecialType> const &forbidden_p, unsigned long count_p = 0, unsigned long nb_upgrades_p = 1);

	virtual void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;

	void setEndless(bool endless_p) { _endless = endless_p; }

private:
	/// @brief required because wave has already been picked from the current pool
	WaveInfo const _currentWave;
	/// @brief required because currentspawn point has been picked
	std::vector<octopus::Vector> const _currentSpawnPoints;
	/// @brief true if the ealy wave the the current wave is supposed to spawn
	bool const _earlyWave;
	octopus::Library const &_lib;
	octopus::RandomGenerator &_rand;

	std::list<WaveParam> _params;
	unsigned long _playerSpawn;
	std::vector<unsigned long> _players;

	/// @brief last wave will repeat endlessly
	bool _endless = false;
	// the wave index
	unsigned long _count = 0;
	// number of upgrade to spawn
	unsigned long _nb_upgrades = 1;

	std::function<std::vector<octopus::Steppable *>(void)> _waveStepGenerator;

	std::vector<fas::SurvivalSpecialType> _forbidden;
};

/// @brief trigger win for the given player when all given entities are dead
class WinTrigger : public octopus::OneShotTrigger
{
public:
	WinTrigger(unsigned long winner_p, std::unordered_set<octopus::Handle> const &handles_p);

	void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;

private:
	unsigned long _winner;
};

/// @brief trigger for option generator when wave is cleared
class WaveClearTrigger : public octopus::OneShotTrigger
{
public:
	WaveClearTrigger(
		unsigned long playerSpawn_p,
		std::vector<unsigned long> const & players_p,
		std::vector<fas::SurvivalSpecialType> const &forbidden_p,
		std::unordered_set<octopus::Handle> const &handles_p,
		octopus::Library const &lib_p,
		octopus::RandomGenerator &rand_p,
		unsigned long count_p,
		unsigned long nb_upgrades_p
	);

	void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;

private:
	unsigned long _playerSpawn;
	std::vector<unsigned long> _players;
	std::vector<fas::SurvivalSpecialType> _forbidden;
	octopus::Library const &_lib;
	octopus::RandomGenerator &_rand;
	// the wave index
	unsigned long _count = 0;
	// number of upgrade to spawn
	unsigned long _nb_upgrades = 1;
};

/// @brief Roll N spawn points from the candidates
std::vector<octopus::Vector> rollSpawnPoints(std::vector<octopus::Vector> const &candidates_p, unsigned long number_p, octopus::RandomGenerator &rand_p);

} // namespace godot


#endif
