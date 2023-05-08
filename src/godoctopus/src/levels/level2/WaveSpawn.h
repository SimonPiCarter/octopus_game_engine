#ifndef __WaveSpawnLevel2__
#define __WaveSpawnLevel2__

#include <list>
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
	octopus::Vector spawnPoint;
	unsigned long stepWait;
	unsigned long number;
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

} // namespace level2
} // namespace godot


#endif
