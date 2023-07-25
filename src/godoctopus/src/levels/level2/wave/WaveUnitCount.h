#ifndef __Godoctopus_WaveUnitCount__
#define __Godoctopus_WaveUnitCount__

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
	double dropCoef = 0.;
};

/// @brief store all possible waves for this pool
struct WavePoolInfo
{
	std::vector<WaveInfo> infos;
};

} //godot

#endif // __Godoctopus_WaveUnitCount__
