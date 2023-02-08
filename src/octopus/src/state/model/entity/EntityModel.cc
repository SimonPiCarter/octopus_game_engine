#include "EntityModel.hh"

namespace octopus
{

std::map<ResourceType, double> getReverseCostMap(std::map<ResourceType, double> const &cost_p)
{
	std::map<ResourceType, double> reversedCost_l;
	for(auto && pair_l : cost_p)
	{
		reversedCost_l[pair_l.first] = -pair_l.second;
	}
	return reversedCost_l;
}

} // namespace octopus

