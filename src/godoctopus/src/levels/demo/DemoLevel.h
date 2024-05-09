#ifndef __Demo__
#define __Demo__

#include <list>
#include <vector>
#include <functional>
#include <fstream>
#include "utils/RandomGenerator.hh"

#include "levels/wave/WaveUnitCount.h"
#include "levels/model/utils/EntitySpawner.h"

// THIS IS THE REAL DEMO LEVEL
#define LEVEL_ID_LEVEL_DEMO 1999

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

namespace demo
{

/// @brief
/// @param lib_p
/// @param rand_p
/// @param waveInfo_p
/// @param player_p
/// @param playerCount_p
/// @param entityInfo_p
/// @param difficulty_p Easiest is 1, Hardest is 3
/// @return
std::list<octopus::Steppable *> DemoLevelSteps(octopus::Library &lib_p, octopus::RandomGenerator &rand_p,
	std::vector<WavePoolInfo> const &waveInfo_p, unsigned long player_p, unsigned long playerCount_p,
	std::vector<GodotEntityInfo> const &entityInfo_p,
	bool two_direction_wave_p,
	bool bosses_p,
	bool fast_anchor_decay_p,
	int buff_per_wave_p,
	bool demo_p);

/// @brief
/// @param lib_p
/// @param rand_p
/// @param playerCount_p
/// @param difficulty_p Easiest is 1, Hardest is 3
/// @return
std::list<octopus::Command *> DemoLevelCommands(octopus::Library &lib_p, octopus::RandomGenerator &rand_p, unsigned long playerCount_p,
	bool less_resources_p,
	bool more_enemies_map_p,
	bool bosses_p);

struct DemoLevelHeader
{
	// seed for random generation
	int seed;

	/// @brief player to spawn waves
	unsigned long player;

	// difficulty parameters

	/// @brief less resources on the map
	bool less_resources;
	/// @brief more enemies on the map
	bool more_enemies_map;
	/// @brief two direction wave
	bool two_direction_wave;
	/// @brief spawn bosses on the map
	bool bosses;
	/// @brief fast anchor devay
	bool fast_anchor_decay;
	/// @brief number of buff per wave
	uint32_t buff_per_wave;

	/// @brief player_count
	uint32_t player_count;

	/// @brief store every tier wave info
	std::vector<WavePoolInfo> tierWaveInfo;
};

/// @brief write header for classic wave level
void writeDemoLevelHeader(std::ofstream &file_p, DemoLevelHeader const &header_p);
/// @brief read header for classic wave level
void readDemoLevelHeader(std::ifstream &file_p, DemoLevelHeader &header_r);
/// @brief read header for classic wave level and return a pair of steppable and command (warning delete and reset rand_p using the written seed)
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readDemoLevelHeader(octopus::Library &lib_p, std::ifstream &file_p, std::vector<GodotEntityInfo> const &entityInfo_p,
	octopus::RandomGenerator * &rand_p, DemoLevelHeader &header_r);

} // namespace level2
} // namespace godot

#endif
