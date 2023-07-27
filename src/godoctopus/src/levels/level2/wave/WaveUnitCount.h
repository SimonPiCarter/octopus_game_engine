#ifndef __Godoctopus_WaveUnitCount__
#define __Godoctopus_WaveUnitCount__

#include <string>
#include <vector>

namespace octopus
{
	class RandomGenerator;
} // namespace octopus

namespace godot
{

/// @brief basic struct that conatains a number of model of unit to spawn
struct WaveUnitCount {
    std::string model;
    int count = 0;
};

/// @brief store the content of a wave
struct WaveContentInfo
{
	std::vector<WaveUnitCount> units;
	int steps = 0;
};

/// @brief store wave info (check Wave class for godot version of it)
struct WaveInfo
{
	std::vector<std::string> upgrades;
	WaveContentInfo mainWave;
	WaveContentInfo earlyWave;
	int dropCoef = 0;
};

/// @brief store all possible waves for this pool
struct WavePoolInfo
{
	std::vector<WaveInfo> infos;
};

/// @brief chose a random wave from the given pool
WaveInfo rollWave(octopus::RandomGenerator &rand_p, WavePoolInfo const &wavePool_p);

} //godot

#endif // __Godoctopus_WaveUnitCount__
