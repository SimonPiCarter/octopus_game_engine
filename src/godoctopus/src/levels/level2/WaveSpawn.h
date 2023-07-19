#ifndef __WaveSpawnLevel2__
#define __WaveSpawnLevel2__

#include <list>
#include <unordered_set>
#include <vector>
#include <functional>
#include "controller/trigger/Trigger.hh"
#include "utils/RandomGenerator.hh"
#include "utils/Vector.hh"

namespace octopus
{
	class Library;
	class State;
	class Step;
	class Steppable;
} // namespace octopus

namespace godot
{
namespace level2
{

std::vector<octopus::Steppable*> defaultGenerator();

struct WaveParam
{
	/// @brief point where the unit will spawn
	octopus::Vector spawnPoint;
	/// @brief number of step to wait unit wave spawn
	unsigned long stepWait;
	/// @brief number of unit spawning
	unsigned long number;
	/// @brief limits to remove
	unsigned long limitX;
	unsigned long limitY;
};

class WaveSpawn : public octopus::OneShotTrigger
{
public:
	WaveSpawn(octopus::Listener * listener_p, octopus::Library const &lib_p, octopus::RandomGenerator &rand_p, std::list<WaveParam> const &param_p,
		std::function<std::vector<octopus::Steppable *>(void)> waveStepGenerator_p);

	virtual void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;

private:
	octopus::Library const &_lib;
	octopus::RandomGenerator &_rand;

	std::list<WaveParam> _params;

	std::function<std::vector<octopus::Steppable *>(void)> _waveStepGenerator;
};

/// @brief trigger win for the given player when all given entities are dead
class WinTrigger : public octopus::OneShotTrigger
{
public:
	WinTrigger(unsigned long winner_p, std::unordered_set<octopus::Handle> const &handles_p);

	virtual void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;

private:
	unsigned long _winner;
};

} // namespace level2
} // namespace godot


#endif
