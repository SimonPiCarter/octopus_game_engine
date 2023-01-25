#include "Player.hh"

#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"

namespace octopus
{
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
