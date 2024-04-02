#include "SurvivalModelLoader.hh"

#include <vector>

// octopus
#include "library/Library.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/ResourceModel.hh"
#include "state/player/upgrade/Upgrade.hh"
#include "step/player/PlayerAddBuildingModel.hh"
#include "step/player/PlayerAddOptionStep.hh"
#include "step/player/PlayerBuffAllStep.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "utils/RandomGenerator.hh"

// fas
#include "library/model/AnchorTrigger.hh"

using namespace octopus;

namespace fas
{

void createSpecialBuilding(std::string const &name_unit, std::string const &name_building, Library &lib_p)
{
	BuildingModel buildingModel_l { true, 1.8, 1500 };

	buildingModel_l._buildingTime = 2500;
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel(name_unit));
	buildingModel_l._cost["bloc"] = 75;
	buildingModel_l._cost["ether"] = 100;

	lib_p.registerBuildingModel(name_building, buildingModel_l);
}

void createSurvivalHealer(Library &lib_p)
{
	UnitModel unitModel_l { false, 0.5, 0.04, 90 };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 4500;
	unitModel_l._cost["bloc"] = 100;
	unitModel_l._cost["ether"] = 150;
	unitModel_l._damage = 0;
	unitModel_l._heal = 10;
	unitModel_l._armor = 0;
	unitModel_l._range = 5.;
	unitModel_l._lineOfSight = 10;
	unitModel_l._fullReload = 100;
	unitModel_l._windup = 20;

	lib_p.registerUnitModel("survival_heal", unitModel_l);

	createSpecialBuilding("survival_heal", "survival_Heal_building", lib_p);
}

void createSurvivalAttackSpeed(Library &lib_p)
{
	UnitModel unitModel_l { false, 0.5, 0.05, 50 };
	unitModel_l._projectile = true;
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 3000;
	unitModel_l._cost["bloc"] = 25;
	unitModel_l._cost["ether"] = 75;
	unitModel_l._damage = 2;
	unitModel_l._armor = 0;
	unitModel_l._range = 4;
	unitModel_l._lineOfSight = 10;
	unitModel_l._fullReload = 45;
	unitModel_l._windup = 5;
	unitModel_l._damageReturn = 0.5;

	lib_p.registerUnitModel("survival_attackspeed", unitModel_l);

	createSpecialBuilding("survival_attackspeed", "survival_AttackSpeed_building", lib_p);
}

void createSurvivalTank(Library &lib_p)
{
	UnitModel unitModel_l { false, 0.5, 0.04, 250 };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 4500;
	unitModel_l._cost["bloc"] = 150;
	unitModel_l._cost["ether"] = 150;
	unitModel_l._damage = 15;
	unitModel_l._armor = 2;
	unitModel_l._damageReturn = 0.1;
	unitModel_l._range = 0.5;
	unitModel_l._lineOfSight = 10;
	unitModel_l._fullReload = 100;
	unitModel_l._windup = 20;

	lib_p.registerUnitModel("survival_rebound", unitModel_l);

	createSpecialBuilding("survival_rebound", "survival_Rebound_building", lib_p);
}

void createSurvivalAoe(Library &lib_p)
{
	UnitModel unitModel_l { false, 0.5, 0.04, 75 };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 4000;
	unitModel_l._cost["bloc"] = 50;
	unitModel_l._cost["ether"] = 100;
	unitModel_l._damage = 12;
	unitModel_l._armor = 0;
	unitModel_l._range = 3;
	unitModel_l._lineOfSight = 10;
	unitModel_l._fullReload = 100;
	unitModel_l._windup = 20;
	unitModel_l._defaultAttackMod = AoEModifier(0.5, 2., false);

	lib_p.registerUnitModel("survival_aoe", unitModel_l);

	createSpecialBuilding("survival_aoe", "survival_AreaOfDamage_building", lib_p);
}

template<int offset, TyppedBuff::Type type>
class UpgradeBasic : public StepUpgradeGenerator
{
public:
	UpgradeBasic(std::string const &name_p) : _name(name_p) {}
	std::string const _name;
    /// @brief Create a new copy of this generator
    /// this is required to ease up memory handling between steps and states for options handling
    /// @return a newly created copy of this genertor
    virtual StepUpgradeGenerator* newCopy() const
	{
		return new UpgradeBasic<offset, type>(_name);
	}

    /// @brief get internal steppable for the given option
    /// @param options_p the option index
    /// @return a vector of steppables (owner is given away)
    virtual std::vector<Steppable *> getSteppables(unsigned long player_p, unsigned long level_p) const
	{
		std::vector<Steppable *> steps_l;
		TimedBuff buff_l;
		buff_l._type = type;
		buff_l._offset = offset;
		std::stringstream name_l;
		name_l<<_name<<"."<<level_p;
		buff_l._id = name_l.str();
		for(std::string const &model_l : allSurivvalModels())
		{
    		steps_l.push_back(new PlayerBuffAllStep(player_p, buff_l, model_l));
		}
		return steps_l;
	}
};

class UpgradeHarvest : public StepUpgradeGenerator
{
public:
	UpgradeHarvest(std::string const &name_p) : _name(name_p) {}
	std::string const _name;
    /// @brief Create a new copy of this generator
    /// this is required to ease up memory handling between steps and states for options handling
    /// @return a newly created copy of this genertor
    virtual StepUpgradeGenerator* newCopy() const
	{
		return new UpgradeHarvest(_name);
	}

    /// @brief get internal steppable for the given option
    /// @param options_p the option index
    /// @return a vector of steppables (owner is given away)
    virtual std::vector<Steppable *> getSteppables(unsigned long player_p, unsigned long level_p) const
	{
		std::vector<Steppable *> steps_l;
		TimedBuff buff_l;
		buff_l._type = TyppedBuff::Type::Harvest;
		buff_l._coef = 0.1;
		std::stringstream name_l;
		name_l<<_name<<"."<<level_p;
		buff_l._id = name_l.str();
		for(std::string const &model_l : {"gate", "command_center", "deposit"})
		{
    		steps_l.push_back(new PlayerBuffAllStep(player_p, buff_l, model_l));
		}
		return steps_l;
	}
};

class UpgradeOption : public StepUpgradeGenerator
{
public:
	UpgradeOption(
		std::string const &name_p,
		octopus::RandomGenerator &rand_p,
		std::vector<fas::SurvivalSpecialType> const &forbidden_p,
		Library &lib_p
	) : _name(name_p),
		_rand(rand_p),
		_forbidden(forbidden_p),
		_lib(lib_p)
	 {}
	std::string const _name;
	octopus::RandomGenerator &_rand;
	std::vector<fas::SurvivalSpecialType> const _forbidden;
	Library &_lib;

    /// @brief Create a new copy of this generator
    /// this is required to ease up memory handling between steps and states for options handling
    /// @return a newly created copy of this genertor
    virtual StepUpgradeGenerator* newCopy() const
	{
		return new UpgradeOption(_name, _rand, _forbidden, _lib);
	}

    /// @brief get internal steppable for the given option
    /// @param options_p the option index
    /// @return a vector of steppables (owner is given away)
    virtual std::vector<Steppable *> getSteppables(unsigned long player_p, unsigned long level_p) const
	{
		std::stringstream name_l;
		name_l<<_name<<"."<<level_p;
		std::string nameStr_l = name_l.str();
		unsigned long seed_l = _rand.roll(0,std::numeric_limits<int>::max());
		auto optionsGenerator_l = [seed_l, forbidden_l = _forbidden, player_p, nameStr_l](const octopus::State &state_p) {
			return generateOptions(
						nameStr_l,
						seed_l,
						forbidden_l,
						player_p,
						state_p);
		};

		return {new PlayerAddOptionStep(player_p, "0", new BuffGenerator("0", optionsGenerator_l, _lib))};
	}
};

void setUpUpgradeCost(Upgrade *up_p)
{
	const std::vector<std::map<std::string, long> > mapCosts_l {
		{{"bloc", 150},		{"ether", 150},		{"irium", 0}		},
		{{"bloc", 150},		{"ether", 300},		{"irium", 0}		},
		{{"bloc", 150},		{"ether", 300},		{"irium", 300}		},
		{{"bloc", 300},		{"ether", 450},		{"irium", 300}		},
		{{"bloc", 450},		{"ether", 450},		{"irium", 450}		},
		{{"bloc", 450},		{"ether", 900},		{"irium", 900}		},
		{{"bloc", 450},		{"ether", 1800},	{"irium", 1800}		},
		{{"bloc", 450},		{"ether", 3600},	{"irium", 3600}		},
		{{"bloc", 450},		{"ether", 7200},	{"irium", 7200}		},
		{{"bloc", 450},		{"ether", 14400},	{"irium", 14400}	},
	};
	for(size_t i = 0 ; i < mapCosts_l.size() ; ++ i)
	{
		if(i > 0)
		{
			up_p->addLevel();
		}
		up_p->_cost[i]["bloc"] = mapCosts_l[i].at("bloc");
		up_p->_cost[i]["ether"] = mapCosts_l[i].at("ether");
		up_p->_cost[i]["irium"] = mapCosts_l[i].at("irium");
		up_p->_productionTime[i] = 6000;
	}
}

void createUpgrades(Library &lib_p)
{
	Upgrade *harvest_l = new Upgrade(
		"survival_harvest_upgrade",
		new UpgradeHarvest("survival_harvest_upgrade")
	);
	setUpUpgradeCost(harvest_l);

	lib_p.registerUpgrade(harvest_l->_id, harvest_l);
	lib_p.getBuildingModel("gate")._upgrades.push_back(harvest_l);
	lib_p.getBuildingModel("command_center")._upgrades.push_back(harvest_l);

	Upgrade *damage_l = new Upgrade(
		"survival_damage_upgrade",
		new UpgradeBasic<1, TyppedBuff::Type::Damage>("survival_damage_upgrade")
	);
	setUpUpgradeCost(damage_l);

	Upgrade *armor_l = new Upgrade(
		"survival_armor_upgrade",
		new UpgradeBasic<1, TyppedBuff::Type::Armor>("survival_armor_upgrade")
	);
	setUpUpgradeCost(armor_l);

	lib_p.registerUpgrade(damage_l->_id, damage_l);
	lib_p.registerUpgrade(armor_l->_id, armor_l);

	lib_p.getBuildingModel("gate")._upgrades.push_back(damage_l);
	lib_p.getBuildingModel("gate")._upgrades.push_back(armor_l);

	lib_p.getBuildingModel("command_center")._upgrades.push_back(damage_l);
	lib_p.getBuildingModel("command_center")._upgrades.push_back(armor_l);
}

void createOptionUpgrades(Library &lib_p, octopus::RandomGenerator &rand_p, std::vector<fas::SurvivalSpecialType> const &forbidden_p)
{
	Upgrade *option_l = new Upgrade(
		"survival_option_upgrade",
		new UpgradeOption("survival_option_upgrade", rand_p, forbidden_p, lib_p)
	);
	setUpUpgradeCost(option_l);
	lib_p.registerUpgrade(option_l->_id, option_l);
	lib_p.getBuildingModel("gate")._upgrades.push_back(option_l);
	lib_p.getBuildingModel("command_center")._upgrades.push_back(option_l);
}

void loadSurvivalModels(Library &lib_p)
{
	createSurvivalHealer(lib_p);
	createSurvivalAttackSpeed(lib_p);
	createSurvivalTank(lib_p);
	createSurvivalAoe(lib_p);
	createUpgrades(lib_p);
}


std::vector<octopus::Steppable *> newPlayerBuilding(unsigned long player_p, SurvivalSpecialType type_p, octopus::Library const &lib_p)
{
	std::string name_l = survivalSpecialTypeName(type_p);
	if(name_l != "")
	{
		return { new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel(name_l+"_building")) };
	}
    return {};
}

std::string survivalSpecialTypeName(SurvivalSpecialType type_p)
{
    switch(type_p)
    {
		case SurvivalSpecialType::AttackSpeed:
			return "survival_AttackSpeed";
		case SurvivalSpecialType::Heal:
			return "survival_Heal";
		case SurvivalSpecialType::Rebound:
			return "survival_Rebound";
		case SurvivalSpecialType::AreaOfDamage:
			return "survival_AreaOfDamage";
    }
    return "";
}

SurvivalSpecialType rollOneRandomSurvivalSpecialType(octopus::RandomGenerator &rand_p, std::vector<SurvivalSpecialType> const &exceptions_p)
{
    return rollOneRandomSurvivalSpecialTypeFromList(rand_p, allSurvivalSpecialTypes(), exceptions_p);
}

SurvivalSpecialType rollOneRandomSurvivalSpecialTypeFromList(octopus::RandomGenerator &rand_p, std::vector<SurvivalSpecialType> const &list_p, std::vector<SurvivalSpecialType> const &exceptions_p)
{
    std::vector<SurvivalSpecialType> options_l = list_p;

	for(SurvivalSpecialType type_l : exceptions_p)
	{
		auto &&it_l = std::find(options_l.begin(), options_l.end(), type_l);
		if(it_l != options_l.cend())
		{
			options_l.erase(it_l);
		}
	}

	if(options_l.empty())
	{
		return SurvivalSpecialType::None;
	}

    int roll_l = rand_p.roll(0, options_l.size()-1);

    return options_l[roll_l];
}

std::vector<SurvivalSpecialType> allSurvivalSpecialTypes()
{
    return {
        SurvivalSpecialType::AttackSpeed,
        SurvivalSpecialType::Heal,
        SurvivalSpecialType::Rebound,
        SurvivalSpecialType::AreaOfDamage,
    };
}

std::vector<SurvivalSpecialType> notDemoSurvivalSpecialTypes()
{
    return {
    };
}

std::vector<SurvivalSpecialType> demoSurvivalSpecialTypes()
{
    return {
        SurvivalSpecialType::AttackSpeed,
        SurvivalSpecialType::Heal,
        SurvivalSpecialType::Rebound,
        SurvivalSpecialType::AreaOfDamage,
    };
}

std::vector<SurvivalSpecialType> getMaxedOutSurvivalSpecialTypes(octopus::Player const &player_p, unsigned long maxLevel_p)
{
	std::vector<SurvivalSpecialType> maxedOut_l;
	for(SurvivalSpecialType type_l : allSurvivalSpecialTypes())
	{
		if(getUpgradeLvl(player_p, survivalSpecialTypeName(type_l)) >= maxLevel_p)
		{
			maxedOut_l.push_back(type_l);
		}
	}

	return maxedOut_l;
}

void addSurvivalBuildingPlayer(std::list<octopus::Steppable *> &spawners_p, unsigned long player_p, octopus::Library &lib_p)
{
	spawners_p.push_back(new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel("command_center")));
	spawners_p.push_back(new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel("barrack_circle")));
	spawners_p.push_back(new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel("barrack_triangle")));
	spawners_p.push_back(new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel("deposit")));
	spawners_p.push_back(new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel("anchor")));
}

std::vector<std::string> allSurivvalModels()
{
	return {
		"triangle",
		"circle",
		"survival_heal",
		"survival_attackspeed",
		"survival_rebound",
		"survival_aoe"
	};
}

}
