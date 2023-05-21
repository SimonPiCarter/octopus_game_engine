#include "Player.hh"

#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/player/upgrade/Upgrade.hh"

namespace octopus
{

Player::~Player()
{
	for(auto &&pair_l : _options)
	{
		delete pair_l.second;
	}
}

Fixed getResource(Player const &player_p, std::string const &type_p)
{
	auto &&it_l = player_p._resources.find(type_p);
	if(it_l != player_p._resources.end())
	{
		return it_l->second;
	}
	return 0.;
}

bool getDivOption(DivinityOption const &option_p, std::string const &type_p)
{
	auto &&it_l = option_p._types.find(type_p);
	if(it_l != option_p._types.end())
	{
		return true;
	}
	return false;
}

unsigned long getUpgradeLvl(Player const &player_p, std::string const &type_p)
{
	auto &&it_l = player_p._upgradeLvl.find(type_p);
	if(it_l != player_p._upgradeLvl.end())
	{
		return it_l->second;
	}
	return 0;
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

std::list<Upgrade const *> getAvailableUpgrades(BuildingModel const &building_p, Player const &player_p)
{
	std::list<Upgrade const *> availables_l;
	for(Upgrade const * upgrade_l : building_p._upgrades)
	{
		// meet requirement and avoid repeat upgrade
		if(meetRequirements(upgrade_l->_requirements, player_p)
		&& (upgrade_l->_repeatable || player_p._producedUpgrade.find(upgrade_l->_id) == player_p._producedUpgrade.end()))
		{
			availables_l.push_back(upgrade_l);
		}
	}
	return availables_l;
}

} // namespace octopus
