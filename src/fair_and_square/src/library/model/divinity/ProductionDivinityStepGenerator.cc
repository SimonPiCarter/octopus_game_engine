#include "ProductionDivinityStepGenerator.hh"

#include "controller/trigger/Listener.hh"
#include "controller/trigger/Trigger.hh"
#include "library/Library.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Unit.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/player/upgrade/StepUpgradeGenerator.hh"
#include "step/Step.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/player/PlayerBuffAllStep.hh"
#include "step/player/bonus/PlayerAddTimeProdBonusStep.hh"
#include "step/player/bonus/PlayerAddCostBonusStep.hh"
#include "step/trigger/TriggerSpawn.hh"

#include "UnitModelIds.hh"

using namespace octopus;

namespace production_divinity
{

class ProductionDeathResTrigger : public OnEachTrigger
{
public:
	ProductionDeathResTrigger(EntityModel const *model_p, unsigned long player_p, std::string const &res_p, Fixed const &qty_p) :
		OnEachTrigger(new ListenerEntityModelDied(model_p, player_p)),
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

class ProductionDeathRespawnTrigger : public OnEachTrigger
{
public:
	ProductionDeathRespawnTrigger(EntityModel const *model_p, UnitModel const &modelRespawn_p, unsigned long player_p) :
		OnEachTrigger(new ListenerEntityModelDied(model_p, player_p)),
		_player(player_p),
		_modelRespawn(modelRespawn_p)
	{}

	virtual void trigger(State const &state_p, Step &step_p, unsigned long count_p, TriggerData const &data_p) const override
	{
		ListenerEntityData const * entData_l = dynamic_cast<ListenerEntityData const * >(data_p._listenerData[0]);
		Entity const * ent_l = entData_l->_entities[count_p];
		step_p.addSteppable(new UnitSpawnStep(getNextHandle(step_p, state_p), {ent_l->_pos, false, _modelRespawn}));
	}

private:
	unsigned long const _player;
	UnitModel const &_modelRespawn;
};

std::vector<Steppable *> productionTierOneGenertor(ProductionDivinityParams const &param_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	// NA
	// just unlocking a unit

	return steps_l;
}

std::vector<Steppable *> productionTierTwoGenertor(ProductionDivinityParams const &param_p, Library const &lib_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	// add starting number of resource for buffer
	std::map<std::string, Fixed> map_l;
	map_l[param_p._productionBufferResourceId] = -param_p._numberOfProductionBufferTierTwo;
	steps_l.push_back(new PlayerSpendResourceStep(player_p, map_l));
	// regain resource on death
	steps_l.push_back(new TriggerSpawn(new ProductionDeathResTrigger(&lib_p.getUnitModel(param_p._productionBufferModelId),
		player_p, param_p._productionBufferResourceId, 1)));

	return steps_l;
}

std::vector<Steppable *> productionTierThreeGenertor(ProductionDivinityParams const &param_p, Library const &lib_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	steps_l.push_back(new TriggerSpawn(new ProductionDeathRespawnTrigger(&lib_p.getUnitModel(param_p._tierOneUnitModelId),
		lib_p.getUnitModel(param_p._tierOneUnitRespawnModelId), player_p)));

	return steps_l;
}

class ProductionBuffUpgrade : public StepUpgradeGenerator
{
public:
	ProductionBuffUpgrade(std::vector<std::string> const &models_p, Fixed ratioReduction_p) : _models(models_p), _ratioReduction(ratioReduction_p) {}

	virtual ProductionBuffUpgrade* newCopy() const
	{
		return new ProductionBuffUpgrade(_models, _ratioReduction);
	}

	virtual std::vector<Steppable *> getSteppables(unsigned long player_p) const
	{
		std::vector<Steppable *> steps_l;

		for(std::string const &model_l : _models)
		{
			steps_l.push_back(new PlayerAddTimeProdBonusStep(player_p, model_l, {0, Fixed(-1) * _ratioReduction}));
		}
		return steps_l;
	}

private:
	std::vector<std::string> const _models;
	Fixed const _ratioReduction;
};

class ProductionResourceBuffUpgrade : public StepUpgradeGenerator
{
public:
	ProductionResourceBuffUpgrade(std::string const &model_p, std::map<std::string, Fixed> const &prodTierOneUnitCostReductionCoef_p)
		: _model(model_p)
		, _prodTierOneUnitCostReductionCoef(prodTierOneUnitCostReductionCoef_p)
	{}

	virtual ProductionResourceBuffUpgrade* newCopy() const
	{
		return new ProductionResourceBuffUpgrade(_model, _prodTierOneUnitCostReductionCoef);
	}

	virtual std::vector<Steppable *> getSteppables(unsigned long player_p) const
	{
		std::vector<Steppable *> steps_l;

		for(auto &&pair_l : _prodTierOneUnitCostReductionCoef)
		{
			steps_l.push_back(new PlayerAddCostBonusStep(player_p, _model, pair_l.first, {0, Fixed(-1) * pair_l.second}));
		}

		return steps_l;
	}
private:
	std::string const _model;
	std::map<std::string, Fixed> const _prodTierOneUnitCostReductionCoef;
};

class ProductionMoreBufferUpgrade : public StepUpgradeGenerator
{
public:
	ProductionMoreBufferUpgrade(std::string const &res_p, long increase_p) : _res(res_p), _increase(increase_p) {}

	virtual ProductionMoreBufferUpgrade* newCopy() const
	{
		return new ProductionMoreBufferUpgrade(_res, _increase);
	}

	virtual std::vector<Steppable *> getSteppables(unsigned long player_p) const
	{
		std::vector<Steppable *> steps_l;

		// add starting number of resource for buffer
		std::map<std::string, Fixed> map_l;
		map_l[_res] = Fixed(-1) * _increase;
		steps_l.push_back(new PlayerSpendResourceStep(player_p, map_l));

		return steps_l;
	}
private:
	std::string const _res;
	long const _increase;
};

ProductionDivinityParams createDefaultParams()
{
	ProductionDivinityParams params_l;
	/// @brief constraint for the number of buff production unit
	params_l._numberOfProductionBufferTierTwo = 4;
	/// @brief increased qty of production buffer after t3 upgrade
	params_l._increaseOfProductionBufferTierTwo = 4;
	/// @brief identifier of the resource used to limit the number of buffer
	params_l._productionBufferResourceId = "ProductionBufferResource";
	/// @brief percent reduction for time production
	params_l._productionTimeUpgradeCoef = 0.1;
	/// @brief percent reduction for resource cost production for tier one unit model
	params_l._prodTierOneUnitCostReductionCoef["bloc"] = 0.2;
	params_l._prodTierOneUnitCostReductionCoef["ether"] = 0.2;

	params_l._tierOneUnitModelId = models::ProductionUnitModelTierOneId;
	params_l._tierOneUnitRespawnModelId = models::ProductionUnitModelTierOneRespawnId;
	params_l._productionBufferModelId = models::ProductionUnitModelTierTwoId;

	return params_l;
}

void fillLibrary(ProductionDivinityParams const &params_p, octopus::Library &lib_p)
{
	/// @brief tier one unit model (cheap unit)
	UnitModel tieroneunitmodel_l { false, 0.5, 0.05, 35 };
	tieroneunitmodel_l._isUnit = true;
	tieroneunitmodel_l._productionTime = 1500;
	tieroneunitmodel_l._cost["bloc"] = 20;
	tieroneunitmodel_l._cost["ether"] = 10;
	tieroneunitmodel_l._damage = 5;
	tieroneunitmodel_l._armor = 0;
	tieroneunitmodel_l._range = 0.5;
	tieroneunitmodel_l._lineOfSight = 10;
	tieroneunitmodel_l._fullReload = 125;
	tieroneunitmodel_l._windup = 20;
	tieroneunitmodel_l._requirements._upgradeLvl[models::ProductionDivId] = 1;

	lib_p.registerUnitModel(params_p._tierOneUnitModelId, tieroneunitmodel_l);

	/// @brief tier one unit model respawn after t3 upgrade (same unit just different id to avoid infinite respawn)
	lib_p.registerUnitModel(params_p._tierOneUnitRespawnModelId, tieroneunitmodel_l);

	/// @brief tier two unit model that buff production buildings
	UnitModel tiertwounitmodel_l { false, 0.5, 0.05, 35 };
	tiertwounitmodel_l._isUnit = true;
	tiertwounitmodel_l._productionTime = 1500;
	tiertwounitmodel_l._cost["bloc"] = 50;
	tiertwounitmodel_l._cost["irium"] = 100;
	tiertwounitmodel_l._cost[params_p._productionBufferResourceId] = 1;
	tiertwounitmodel_l._damage = 0;
	tiertwounitmodel_l._armor = 0;
	tiertwounitmodel_l._range = 0.5;
	tiertwounitmodel_l._lineOfSight = 10;
	tiertwounitmodel_l._fullReload = 125;
	tiertwounitmodel_l._windup = 20;

	// buff of 100% production
	tiertwounitmodel_l._buffer._buff._coef = 1;
	tiertwounitmodel_l._buffer._buff._id = "production_buff_production";
	tiertwounitmodel_l._buffer._buff._type = TyppedBuff::Type::Production;
	tiertwounitmodel_l._buffer._buff._duration = 3000;
	tiertwounitmodel_l._buffer._buff._offset = 0;
	tiertwounitmodel_l._buffer._range = 5.;
	tiertwounitmodel_l._buffer._reload = 3000;
	tiertwounitmodel_l._buffer._active = true;

	tiertwounitmodel_l._requirements._upgradeLvl[models::ProductionDivId] = 2;
	lib_p.registerUnitModel(params_p._productionBufferModelId, tiertwounitmodel_l);

	// declare upgrades

	// T1 production buff
	Upgrade * upProductionTime_l = new Upgrade("ProductionUpgrade_BuffProduction", new ProductionBuffUpgrade({}, params_p._productionTimeUpgradeCoef));
	upProductionTime_l->_cost["bloc"] = 125;
	upProductionTime_l->_cost["ether"] = 75;
	upProductionTime_l->_productionTime = 6000;
	upProductionTime_l->_requirements._upgradeLvl[models::ProductionDivId] = 1;
	lib_p.registerUpgrade(upProductionTime_l->_id, upProductionTime_l);

	// T2 cost reduction for unit 1
	Upgrade * upProductionCost_l = new Upgrade("ProductionUpgrade_BuffResourceProduction",
		new ProductionResourceBuffUpgrade(params_p._tierOneUnitModelId, params_p._prodTierOneUnitCostReductionCoef));
	upProductionCost_l->_cost["bloc"] = 100;
	upProductionCost_l->_cost["ether"] = 200;
	upProductionCost_l->_cost["irium"] = 200;
	upProductionCost_l->_productionTime = 15000;
	upProductionCost_l->_requirements._upgradeLvl[models::ProductionDivId] = 2;
	lib_p.registerUpgrade(upProductionCost_l->_id, upProductionCost_l);

	// T3 increase buffer count
	Upgrade * upProductionIncreaseBufferCount_l = new Upgrade("ProductionUpgrade_BuffNumberBuffer",
		new ProductionMoreBufferUpgrade(params_p._productionBufferResourceId, params_p._increaseOfProductionBufferTierTwo));
	upProductionIncreaseBufferCount_l->_cost["bloc"] = 125;
	upProductionIncreaseBufferCount_l->_cost["ether"] = 250;
	upProductionIncreaseBufferCount_l->_cost["irium"] = 350;
	upProductionIncreaseBufferCount_l->_productionTime = 30000;
	upProductionIncreaseBufferCount_l->_requirements._upgradeLvl[models::ProductionDivId] = 3;
	lib_p.registerUpgrade(upProductionIncreaseBufferCount_l->_id, upProductionIncreaseBufferCount_l);

	/// @brief temple
	BuildingModel buildingModel_l { true, 0.9, 1500 };
	buildingModel_l._buildingTime = 2500;
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel(params_p._tierOneUnitModelId));
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel(params_p._productionBufferModelId));
	buildingModel_l._cost["bloc"] = 75;
	buildingModel_l._cost["ether"] = 100;
	buildingModel_l._upgrades.push_back(&lib_p.getUpgrade(upProductionTime_l->_id));
	buildingModel_l._upgrades.push_back(&lib_p.getUpgrade(upProductionCost_l->_id));
	buildingModel_l._upgrades.push_back(&lib_p.getUpgrade(upProductionIncreaseBufferCount_l->_id));
	buildingModel_l._requirements._upgradeLvl[models::ProductionDivId] = 1;

	lib_p.registerBuildingModel(models::ProductionBuildingId, buildingModel_l);
}

}
