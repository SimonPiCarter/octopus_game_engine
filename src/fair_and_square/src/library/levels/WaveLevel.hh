#ifndef __WaveLevel__
#define __WaveLevel__

#include <list>
#include <vector>
#include <functional>
#include "controller/trigger/Trigger.hh"

namespace octopus
{
	class Command;
	class Library;
	class State;
	class Step;
	class Steppable;
} // namespace octopus

std::vector<octopus::Steppable*> defaultGenerator();

std::list<octopus::Steppable *> WaveLevelSteps(octopus::Library &lib_p, unsigned long waveCount_p, unsigned long stepCount_p, unsigned long player_p, unsigned long worldSize_p,
	std::function<std::vector<octopus::Steppable *>(void)> waveStepGenerator_p=defaultGenerator);
std::list<octopus::Command *> WaveLevelCommands(octopus::Library &lib_p, unsigned long worldSize_p);

class WaveSpawn : public octopus::OneShotTrigger
{
public:
	WaveSpawn(octopus::Listener * listener_p, octopus::Library const &lib_p, unsigned long wave_p, unsigned long stepWait_p, unsigned long finalWave_p, unsigned long player_p, unsigned long worldSize_p,
		std::function<std::vector<octopus::Steppable *>(void)> waveStepGenerator_p);

	virtual void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;

private:
	octopus::Library const &_lib;
	unsigned long const _player;
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

	virtual void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;
};


#endif
