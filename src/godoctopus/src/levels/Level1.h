#ifndef __WaveLevel1__
#define __WaveLevel1__

#include <list>
#include <vector>
#include <functional>
#include "controller/trigger/Trigger.hh"
#include "utils/RandomGenerator.hh"

#define LEVEL_ID_LEVEL_1 1000

namespace octopus
{
	class Command;
	class Library;
	class State;
	class Step;
	class Steppable;
} // namespace octopus

namespace godot
{
namespace level1
{

std::vector<octopus::Steppable*> defaultGenerator();

std::list<octopus::Steppable *> WaveLevelSteps(octopus::Library &lib_p, octopus::RandomGenerator &rand_p, unsigned long waveCount_p,
	unsigned long stepCount_p, unsigned long worldSize_p);
std::list<octopus::Command *> WaveLevelCommands(octopus::Library &lib_p, octopus::RandomGenerator &rand_p, unsigned long worldSize_p);

struct WaveLevelHeader
{
	// seed for random generation
	int seed;
	// number of wave before winning
	unsigned long waveCount;
	// number of step between waves
	unsigned long stepCount;
	// size of the world
	unsigned long worldSize;
};

/// @brief write header for classic arena level
void writeWaveLevelHeader(std::ofstream &file_p, WaveLevelHeader const &header_p);
/// @brief read header for classic arena level and return a pair of steppable and command (warning delete and reset rand_p using the written seed)
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readWaveLevelHeader(octopus::Library &lib_p, std::ifstream &file_p,
	octopus::RandomGenerator * &rand_p, WaveLevelHeader &header_r);

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
} // namespace godot

#endif
