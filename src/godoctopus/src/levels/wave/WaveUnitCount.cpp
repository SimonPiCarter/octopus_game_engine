#include "WaveUnitCount.h"

#include "utils/RandomGenerator.hh"

namespace godot
{
WaveInfo rollWave(octopus::RandomGenerator &rand_p, WavePoolInfo const &wavePool_p)
{
	int totalDrop_l = 0;
	for(WaveInfo const &info_l : wavePool_p.infos)
	{
		totalDrop_l += std::max(1, info_l.dropCoef);
	}
	int roll_l = rand_p.roll(0, std::max(0, totalDrop_l-1));

	int currentRoll_l = 0.;
	for(WaveInfo const &info_l : wavePool_p.infos)
	{
		currentRoll_l += std::max(1, info_l.dropCoef);
		if(roll_l < currentRoll_l)
		{
			return info_l;
		}
	}

	return WaveInfo();
}
} // namespace godot
