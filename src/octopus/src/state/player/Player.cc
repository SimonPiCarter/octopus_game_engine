#include "Player.hh"

#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"

namespace octopus
{
double getResource(Player const &player_p, ResourceType type_p)
{
	auto &&it_l = player_p._resources.find(type_p);
	if(it_l != player_p._resources.end())
	{
		return it_l->second;
	}
	return 0.;
}

unsigned long getDivOption(Player const &player_p, DivinityType type_p)
{
	auto &&it_l = player_p._divOptions.find(type_p);
	if(it_l != player_p._divOptions.end())
	{
		return it_l->second;
	}
	return 0;
}

unsigned long getDivLvl(Player const &player_p, DivinityType type_p)
{
	auto &&it_l = player_p._divLvl.find(type_p);
	if(it_l != player_p._divLvl.end())
	{
		return it_l->second;
	}
	return 0;
}

double getDivAnchor(Player const &player_p, DivinityType type_p, bool floor_p)
{
	auto &&it_l = player_p._divAnchor.find(type_p);
	if(it_l != player_p._divAnchor.end())
	{
		return floor_p ? std::max(0., it_l->second) : it_l->second;
	}
	return 0.;
}


std::list<BuildingModel const *> getAvailableBuildingModels(Player const &player_p)
{
	std::list<BuildingModel const *> availables_l;
	for(BuildingModel const * model_l : player_p._buildingModels)
	{
		if(meetRequirements(model_l->_requirements, player_p))
		{
			availables_l.push_back(model_l);
		}
	}
	return availables_l;
}

std::list<UnitModel const *> getAvailableUnitModels(BuildingModel const &building_p, Player const &player_p)
{
	std::list<UnitModel const *> availables_l;
	for(UnitModel const * model_l : building_p._unitModels)
	{
		if(meetRequirements(model_l->_requirements, player_p))
		{
			availables_l.push_back(model_l);
		}
	}
	return availables_l;
}

} // namespace octopus
