#include "RecycleDivinityStepGenerator.hh"

#include "controller/trigger/Listener.hh"
#include "controller/trigger/Trigger.hh"
#include "library/Library.hh"
#include "state/State.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/player/upgrade/StepUpgradeGenerator.hh"
#include "step/Step.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/player/PlayerBuffAllStep.hh"
#include "step/trigger/TriggerSpawn.hh"

#include "UnitModelIds.hh"

using namespace octopus;

namespace recycle_divinity
{

class RecycleDeathResTrigger : public OnEachTrigger
{
public:
	RecycleDeathResTrigger(unsigned long player_p, std::string const &res_p, Fixed const &qty_p) :
		OnEachTrigger(new ListenerEntityModelDied(nullptr, player_p)),
		_player(player_p),
		_res(res_p),
		_qty(qty_p)
	{}

	virtual void trigger(State const &state_p, Step &step_p, unsigned long, TriggerData const &) const override
	{
		// add resource for the player
		std::map<std::string, Fixed> map_l;
		map_l[_res] = -_qty;
		step_p.addSteppable(new PlayerSpendResourceStep(_player, map_l));
	}

private:
	unsigned long const _player;
	std::string const _res;
	Fixed const _qty;
};

class RecycleDeathBuffTrigger : public OnEachTrigger
{
public:
	RecycleDeathBuffTrigger(std::string const &model_p, unsigned long player_p, Fixed trigger_p, std::vector<TimedBuff> buffs_p) :
		OnEachTrigger(new ListenerEntityModelDied(nullptr, player_p)),
		_player(player_p),
		_model(model_p),
		_trigger(trigger_p),
		_buffs(buffs_p)
	{}

	virtual void trigger(State const &state_p, Step &step_p, unsigned long, TriggerData const &) const override
	{
		// when trigger is exactly reached
		if(getResource(*state_p.getPlayer(_player), models::RecycleDeathResourceId) - step_p.getResourceSpent(_player, models::RecycleDeathResourceId) == _trigger)
		{
			for(TimedBuff const & buff_l : _buffs)
			{
				step_p.addSteppable(new PlayerBuffAllStep(_player, buff_l, _model));
			}
		}
	}

private:
	unsigned long const _player;
	std::string const _model;
	Fixed const _trigger;
	std::vector<TimedBuff> _buffs;
};

std::vector<Steppable *> recycleTierOneGenertor(RecycleDivinityParams const &params_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	steps_l.push_back(new TriggerSpawn(new RecycleDeathResTrigger(player_p, models::RecycleDeathResourceId, 1)));
	steps_l.push_back(new TriggerSpawn(new RecycleDeathBuffTrigger(params_p._tierOneUnitModelId, player_p, params_p._triggerNbUnitDeadBuffTierOne, params_p._buffTierOne)));

	return steps_l;
}

std::vector<Steppable *> recycleTierTwoGenertor(RecycleDivinityParams const &params_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	for(auto &&pair_l : params_p._resourceReturnedTierTwo)
	{
		steps_l.push_back(new TriggerSpawn(new RecycleDeathResTrigger(player_p, pair_l.first, pair_l.second)));
	}

	return steps_l;
}

std::vector<Steppable *> recycleTierThreeGenertor(RecycleDivinityParams const &params_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	steps_l.push_back(new TriggerSpawn(new RecycleDeathBuffTrigger(params_p._tierOneUnitModelId, player_p, params_p._triggerNbUnitDeadBuffTierThree, params_p._buffTierThree)));

	return steps_l;
}

class RecycleHarvestUpgrade : public StepUpgradeGenerator
{
public:
	RecycleHarvestUpgrade(TimedBuff const &buff_p) : _buff(buff_p) {}

	virtual RecycleHarvestUpgrade* newCopy() const
	{
		return new RecycleHarvestUpgrade(_buff);
	}

	virtual std::vector<Steppable *> getSteppables(unsigned long player_p) const
	{
		std::vector<Steppable *> steps_l;

		steps_l.push_back(new PlayerBuffAllStep(player_p, _buff, "deposit"));

		return steps_l;
	}
private:
	TimedBuff const _buff;
};

class RecycleDeathResUpgrade : public StepUpgradeGenerator
{
public:
	RecycleDeathResUpgrade(std::map<std::string, octopus::Fixed> const &mapRes_p) : _mapRes(mapRes_p) {}

	virtual RecycleDeathResUpgrade* newCopy() const
	{
		return new RecycleDeathResUpgrade(_mapRes);
	}

	virtual std::vector<Steppable *> getSteppables(unsigned long player_p) const
	{
		std::vector<Steppable *> steps_l;

		for(auto &&pair_l : _mapRes)
		{
			steps_l.push_back(new TriggerSpawn(new RecycleDeathResTrigger(player_p, pair_l.first, pair_l.second)));
		}

		return steps_l;
	}

private:
	std::map<std::string, octopus::Fixed> const _mapRes;
};

RecycleDivinityParams createDefaultParams()
{
	RecycleDivinityParams params_l;

	/// @brief the number of unit dead required for the buff tier one to apply
	params_l._triggerNbUnitDeadBuffTierOne = 5;
	/// @brief buff applied to tier one unit (on tier one)
	params_l._buffTierOne = {
		{0,0.5,TyppedBuff::Type::HpMax,0,false,"RecycleTier1Hp"},
		{0,0.5,TyppedBuff::Type::Damage,0,false,"RecycleTier1Dmg"},
	};
	/// @brief the number of unit dead required for the buff tier three to apply
	params_l._triggerNbUnitDeadBuffTierThree = 50;
	/// @brief buff applied to tier one unit (on tier three)
	params_l._buffTierThree = {
		{0,0.5,TyppedBuff::Type::HpMax,0,false,"RecycleTier3Hp"},
		{0,0.5,TyppedBuff::Type::Damage,0,false,"RecycleTier3Dmg"},
	};
	/// @brief tier one unit model (unit with a buff when several units die)
	params_l._tierOneUnitModelId = models::RecycleUnitModelTierOneId;
	/// @brief resource cash back on tier two
	params_l._resourceReturnedTierTwo["bloc"] = 5;
	params_l._resourceReturnedTierTwo["ether"] = 2;
	/// @brief resource cash back on tier three (upgrade)
	params_l._resourceReturnedTierThree["bloc"] = 5;
	params_l._resourceReturnedTierThree["ether"] = 3;
	/// @brief resource harvest upgrade
	params_l._resourceHarvestUpgrade = {0, 0.1, TyppedBuff::Type::Harvest, 0, false, "RecycleHarvestUpgrade"};

	return params_l;
}

void fillLibrary(RecycleDivinityParams const &params_p, octopus::Library &lib_p)
{
	/// @brief tier one unit model
	UnitModel tieroneunitmodel_l { false, 0.5, 0.05, 60 };
	tieroneunitmodel_l._isUnit = true;
	tieroneunitmodel_l._productionTime = 2500;
	tieroneunitmodel_l._cost["bloc"] = 50;
	tieroneunitmodel_l._cost["ether"] = 25;
	tieroneunitmodel_l._damage = 8;
	tieroneunitmodel_l._armor = 0;
	tieroneunitmodel_l._range = 0.5;
	tieroneunitmodel_l._lineOfSight = 10;
	tieroneunitmodel_l._fullReload = 125;
	tieroneunitmodel_l._windup = 20;
	tieroneunitmodel_l._requirements._upgradeLvl[models::RecycleDivId+models::tierOneSuffix] = 1;

	lib_p.registerUnitModel(params_p._tierOneUnitModelId, tieroneunitmodel_l);

	// T2 cost reduction for unit 1
	Upgrade * upHarvest_l = new Upgrade("RecycleUpgrade_BuffHarvest",
		new RecycleHarvestUpgrade(params_p._resourceHarvestUpgrade));
	upHarvest_l->_cost["bloc"] = 150;
	upHarvest_l->_cost["ether"] = 150;
	upHarvest_l->_productionTime = 9000;
	upHarvest_l->_requirements._upgradeLvl[models::RecycleDivId+models::tierTwoSuffix] = 1;
	lib_p.registerUpgrade(upHarvest_l->_id, upHarvest_l);


	// T3 increase buffer count
	Upgrade * upBuffDeathRes_l = new Upgrade("RecycleUpgrade_BuffResOnDeath",
		new RecycleDeathResUpgrade(params_p._resourceReturnedTierThree));
	upBuffDeathRes_l->_cost["bloc"] = 150;
	upBuffDeathRes_l->_cost["ether"] = 450;
	upBuffDeathRes_l->_cost["irium"] = 450;
	upBuffDeathRes_l->_productionTime = 30000;
	upBuffDeathRes_l->_requirements._upgradeLvl[models::RecycleDivId+models::tierThreeSuffix] = 1;
	lib_p.registerUpgrade(upBuffDeathRes_l->_id, upBuffDeathRes_l);

	/// @brief temple
	BuildingModel buildingModel_l { true, 1.8, 1500 };

	models::fillTierUpgrade<RecycleDivinityParams>(lib_p, params_p, models::RecycleDivId,
		recycleTierOneGenertor,
		recycleTierTwoGenertor,
		recycleTierThreeGenertor,
		buildingModel_l
	);

	buildingModel_l._buildingTime = 2500;
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel(params_p._tierOneUnitModelId));
	buildingModel_l._cost["bloc"] = 75;
	buildingModel_l._cost["ether"] = 100;
	buildingModel_l._upgrades.push_back(&lib_p.getUpgrade(upHarvest_l->_id));
	buildingModel_l._upgrades.push_back(&lib_p.getUpgrade(upBuffDeathRes_l->_id));
	buildingModel_l._requirements._upgradeLvl[models::RecycleDivId] = 1;

	lib_p.registerBuildingModel(models::RecycleBuildingId, buildingModel_l);
}

}
