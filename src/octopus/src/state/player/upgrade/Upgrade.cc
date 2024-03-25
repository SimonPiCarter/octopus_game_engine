#include "Upgrade.hh"

namespace octopus
{

Upgrade::Upgrade(std::string const &id_p, StepUpgradeGenerator const *generator_p) : _id(id_p), _generator(generator_p)
{
	addLevel();
}

void Upgrade::addLevel()
{
	_requirements.push_back(Requirements());
	_cost.push_back({});
	_productionTime.push_back(100);
}

std::map<std::string, Fixed> const &getCost(Upgrade const &upgrade_p, unsigned long level_p)
{
	static std::map<std::string, Fixed> default_l;
	if(level_p < upgrade_p._cost.size())
	{
		return upgrade_p._cost[level_p];
	}
	return default_l;
}

Requirements const &getRequirements(Upgrade const &upgrade_p, unsigned long level_p)
{
	static Requirements default_l;
	if(level_p < upgrade_p._requirements.size())
	{
		return upgrade_p._requirements[level_p];
	}
	return default_l;
}

int getProductionTime(Upgrade const &upgrade_p, unsigned long level_p)
{
	if(level_p < upgrade_p._productionTime.size())
	{
		return upgrade_p._productionTime[level_p];
	}
	return 0;
}

unsigned long getMaxLevel(Upgrade const &upgrade_p)
{
	return upgrade_p._cost.size();
}

}
