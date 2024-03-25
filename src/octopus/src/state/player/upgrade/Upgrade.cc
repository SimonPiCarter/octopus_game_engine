#include "Upgrade.hh"

namespace octopus
{

std::map<std::string, Fixed> const &getCost(Upgrade const &upgrade_p, unsigned long level_p)
{
	// if level is zero use base cost
	if(level_p == 0 || upgrade_p._costAdvanced.empty())
	{
		return upgrade_p._cost;
	}

	return upgrade_p._costAdvanced[std::min<size_t>(upgrade_p._costAdvanced.size()-1, level_p)];
}

unsigned long getMaxLevel(Upgrade const &upgrade_p)
{
	return upgrade_p._costAdvanced.size() + 1;
}

}
