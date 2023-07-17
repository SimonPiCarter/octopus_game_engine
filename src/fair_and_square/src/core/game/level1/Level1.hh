#ifndef __WaveLevel1__
#define __WaveLevel1__

#include <list>
#include <vector>
#include <functional>
#include "controller/trigger/Trigger.hh"
#include "utils/RandomGenerator.hh"

namespace octopus
{
	class Command;
	class Library;
	class State;
	class Step;
	class Steppable;
} // namespace octopus

namespace cuttlefish
{
	class Window;
} // namespace cuttlefish

namespace fas
{
namespace level1
{

std::vector<octopus::Steppable*> defaultGenerator();

std::list<octopus::Steppable *> WaveLevelSteps(cuttlefish::Window &window_p, octopus::Library &lib_p, octopus::RandomGenerator &rand_p, unsigned long waveCount_p, unsigned long stepCount_p, unsigned long worldSize_p,
	std::function<std::vector<octopus::Steppable *>(void)> waveStepGenerator_p=defaultGenerator);
std::list<octopus::Command *> WaveLevelCommands(octopus::Library &lib_p, octopus::RandomGenerator &rand_p, unsigned long worldSize_p);

class WaveSpawn : public octopus::OneShotTrigger
{
public:
	WaveSpawn(octopus::Listener * listener_p, octopus::Library const &lib_p, octopus::RandomGenerator &rand_p, unsigned long wave_p,
		unsigned long stepWait_p, unsigned long finalWave_p, unsigned long worldSize_p,
		std::function<std::vector<octopus::Steppable *>(void)> waveStepGenerator_p);

	virtual void trigger(octopus::State const &state_p, CommandContext const &, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;

private:
	octopus::Library const &_lib;
	octopus::RandomGenerator &_rand;
	unsigned long const _wave;

	unsigned long const _stepWait;
	unsigned long const _finalWave;
	unsigned long const _worldSize;

	std::function<std::vector<octopus::Steppable *>(void)> _waveStepGenerator;
};

class LoseTrigger : public octopus::OneShotTrigger
{
public:
	LoseTrigger(octopus::Listener * listener_p);

	virtual void trigger(octopus::State const &state_p, CommandContext const &, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;
};

} // namespace level1
} // namespace fas

#endif
