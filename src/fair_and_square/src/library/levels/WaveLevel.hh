#ifndef __WaveLevel__
#define __WaveLevel__

#include <list>
#include "controller/trigger/Trigger.hh"

namespace octopus
{
	class Command;
	class Library;
	class State;
	class Step;
	class Steppable;
} // namespace octopus


std::list<octopus::Steppable *> WaveLevelSteps(octopus::Library &lib_p, unsigned long waveCount_p, unsigned long stepCount_p, unsigned long player_p);
std::list<octopus::Command *> WaveLevelCommands(octopus::Library &lib_p, unsigned long worldSize_p);

class WaveSpawn : public octopus::OneShotTrigger
{
public:
	WaveSpawn(octopus::Listener * listener_p, octopus::Library const &lib_p, unsigned long wave_p, unsigned long stepWait_p, unsigned long finalWave_p, unsigned long player_p);

	virtual void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long) const override;

private:
	octopus::Library const &_lib;
	unsigned long const _player;
	unsigned long const _wave;

	unsigned long const _stepWait;
	unsigned long const _finalWave;
};

class LoseTrigger : public octopus::OneShotTrigger
{
public:
	LoseTrigger(octopus::Listener * listener_p);

	virtual void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long) const override;
};


#endif
