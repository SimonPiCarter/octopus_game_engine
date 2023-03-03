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

double getBonus(std::string const &id_p, EntityModel const &model_p)
{
	auto &&it_l = model_p._bonusDamage.find(id_p);
	if(it_l != model_p._bonusDamage.end())
	{
		return it_l->second;
	}
	return 0.;
}

} // namespace octopus

