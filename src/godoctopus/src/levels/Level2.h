#ifndef __WaveLevel2__
#define __WaveLevel2__

#include <list>
#include <vector>
#include <functional>
#include <fstream>
#include "controller/trigger/Trigger.hh"
#include "utils/RandomGenerator.hh"

#include "level2/wave/WaveUnitCount.h"

// THIS IS THE DEMO LEVEL
#define LEVEL_ID_LEVEL_2 1001

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
namespace level2
{

std::list<octopus::Steppable *> WaveLevelSteps(octopus::Library &lib_p, octopus::RandomGenerator &rand_p, std::vector<WavePoolInfo> const &waveInfo_p, unsigned long player_p, unsigned long playerCount_p);
std::list<octopus::Command *> WaveLevelCommands(octopus::Library &lib_p, octopus::RandomGenerator &rand_p, unsigned long playerCount_p);

struct WaveLevelHeader
{
	// seed for random generation
	int seed;

	/// @brief player to spawn waves
	unsigned long player;

	/// @brief store every tier wave info
	std::vector<WavePoolInfo> tierWaveInfo;
};

/// @brief write header for classic wave level
void writeWaveLevelHeader(std::ofstream &file_p, WaveLevelHeader const &header_p);
/// @brief read header for classic wave level
void readWaveLevelHeader(std::ifstream &file_p, WaveLevelHeader &header_r);
/// @brief read header for classic wave level and return a pair of steppable and command (warning delete and reset rand_p using the written seed)
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readWaveLevelHeader(octopus::Library &lib_p, std::ifstream &file_p,
	octopus::RandomGenerator * &rand_p, WaveLevelHeader &header_r);

class LoseTrigger : public octopus::OneShotTrigger
{
public:
	LoseTrigger(octopus::Listener * listener_p);

	virtual void trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long, octopus::TriggerData const &) const override;
};

} // namespace level2
} // namespace godot

#endif
