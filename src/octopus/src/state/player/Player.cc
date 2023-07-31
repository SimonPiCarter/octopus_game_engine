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

Fixed getProductionTime(UnitModel const &model_p, Player const & player_p)
{
	Fixed prodTime_l = model_p._productionTime;
	auto &&it_l = player_p._bonuses._bonusTimeProduction.find(model_p._id);
	if(it_l == player_p._bonuses._bonusTimeProduction.end())
	{
		return prodTime_l;
	}
	Buff const &buff_l = it_l->second;
	prodTime_l += buff_l._offset;
	prodTime_l = std::max(Fixed(0.), prodTime_l);
	return std::max(Fixed(0.), prodTime_l * (Fixed(1) + buff_l._coef));
}

std::map<std::string, Fixed> getCost(UnitModel const &model_p, Player const & player_p)
{
	// if no bonus return the cost directly
	auto &&itCost_l = player_p._bonuses._bonusCost.find(model_p._id);
	if(itCost_l == player_p._bonuses._bonusCost.end())
	{
		return model_p._cost;
	}
	// else apply bonus on every resource
	CostBonus const &bonus_p = itCost_l->second;
	std::map<std::string, Fixed> cost_l;
	for(auto &&pairRes_l : model_p._cost)
	{
		std::string const &res_l = pairRes_l.first;
		Fixed const &qty_l = pairRes_l.second;
		cost_l[res_l] = qty_l;

		// if there is a bonus for this resource apply it
		auto &&itBonusRes_l = bonus_p._resourceModifier.find(res_l);
		if(itBonusRes_l != bonus_p._resourceModifier.end())
		{
			Buff const &buff_l = itBonusRes_l->second;
			cost_l[res_l] = std::max(Fixed(0.), cost_l[res_l] + buff_l._offset);
			cost_l[res_l] = std::max(Fixed(0.), cost_l[res_l] * (Fixed(1) + buff_l._coef));
		}
	}
	return cost_l;
}

std::list<BuildingModel const *> getAvailableBuildingModels(Player const &player_p, bool checkRequirements_p)
{
	std::list<BuildingModel const *> availables_l;
	for(BuildingModel const * model_l : player_p._buildingModels)
	{
		if(meetRequirements(model_l->_requirements, player_p) || !checkRequirements_p)
		{
			availables_l.push_back(model_l);
		}
	}
	return availables_l;
}

std::list<UnitModel const *> getAvailableUnitModels(BuildingModel const &building_p, Player const &player_p, bool checkRequirements_p)
{
	std::list<UnitModel const *> availables_l;
	for(UnitModel const * model_l : building_p._unitModels)
	{
		if(meetRequirements(model_l->_requirements, player_p) || !checkRequirements_p)
		{
			availables_l.push_back(model_l);
		}
	}
	return availables_l;
}

bool checkUpgradeRequirements(Player const &player_p, Upgrade const &upgrade_p)
{
	bool metRequirement_l = meetRequirements(upgrade_p._requirements, player_p);
	bool alreadyProduced = getUpgradeLvl(player_p, upgrade_p._id) > 0;
	bool beingProduced = player_p._producedUpgrade.find(upgrade_p._id) != player_p._producedUpgrade.end();
	return metRequirement_l && (upgrade_p._repeatable || (!alreadyProduced && !beingProduced));
}

std::list<Upgrade const *> getAvailableUpgrades(BuildingModel const &building_p, Player const &player_p, bool checkRequirements_p)
{
	std::list<Upgrade const *> availables_l;
	for(Upgrade const * upgrade_l : building_p._upgrades)
	{
		// meet requirement and avoid repeat upgrade
		if(!checkRequirements_p || checkUpgradeRequirements(player_p, *upgrade_l))
		{
			availables_l.push_back(upgrade_l);
		}
	}
	return availables_l;
}

} // namespace octopus
