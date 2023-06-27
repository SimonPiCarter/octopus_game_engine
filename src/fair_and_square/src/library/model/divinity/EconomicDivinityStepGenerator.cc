#include "EconomicDivinityStepGenerator.hh"

#include "controller/trigger/Listener.hh"
#include "controller/trigger/Trigger.hh"
#include "library/Library.hh"
#include "state/State.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/player/upgrade/StepUpgradeGenerator.hh"
#include "state/player/upgrade/StepUpgradeTyppedBuffGenerator.hh"
#include "step/Step.hh"
#include "step/entity/EntityUpdateWaitingStep.hh"
#include "step/player/PlayerBuffAllStep.hh"
#include "step/player/bonus/PlayerAddTimeProdBonusStep.hh"
#include "step/player/bonus/PlayerAddCostBonusStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/trigger/TriggerSpawn.hh"

using namespace octopus;

namespace economic_divinity
{


std::vector<Steppable *> economicTierOneGenertor(EconomicDivinityParams const &params_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	steps_l.push_back(new PlayerAddTimeProdBonusStep(player_p, "worker", {Fixed(-1) * params_p._workerReducedTime, 0}));

	return steps_l;
}

std::vector<Steppable *> economicTierTwoGenertor(EconomicDivinityParams const &params_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	// NA

	return steps_l;
}

std::vector<Steppable *> economicTierThreeGenertor(EconomicDivinityParams const &params_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	std::vector<std::string> models_l = {
		"square",
		"double_square",
		"circle",
		"triangle",
		"reversed_triangle",
	};

	for(std::string const &model_l : models_l)
	{
		steps_l.push_back(new PlayerAddCostBonusStep(player_p, model_l, "bloc", {0, params_p._costDebuffTierThree}));
		steps_l.push_back(new PlayerAddCostBonusStep(player_p, model_l, "ether", {0, params_p._costDebuffTierThree}));
		steps_l.push_back(new PlayerBuffAllStep(player_p, {0, params_p._hpBuffTierThree}, model_l));
	}

	return steps_l;
}

EconomicDivinityParams createDefaultParams()
{
	EconomicDivinityParams params_l;

	/// @brief reduced step time for worker on tier one
	params_l._workerReducedTime = 500;

	/// @brief tier two building model (resource production)
	params_l._tierTwoBuildingModelId = "EconomicDivinity_BuildingResourceProducer";

	/// @brief hp buff for every unit
	params_l._hpBuffTierThree = 1;
	/// @brief cost coef increase for every unit
	params_l._costDebuffTierThree = 1;

	/// @brief resource offset buff
	params_l._resourceProductionBuffTierThree = 5;

	return params_l;
}

// spawn blocs every n steps
void customIdleResource(Entity const &ent_p, Step &step_p, State const &)
{
	if(ent_p._waiting >= ent_p.getFullReload())
	{
		std::map<std::string, Fixed> map_l;
		map_l["bloc"] = ent_p.getDamageNoBonus();

		step_p.addSteppable(new PlayerSpendResourceStep(0, map_l));
		step_p.addSteppable(new EntityUpdateWaitingStep(ent_p._handle, ent_p._waiting, 0));
	}
}

void fillLibrary(EconomicDivinityParams const &params_p, octopus::Library &lib_p)
{
	/// @brief tier two building model
	BuildingModel tiertwounitmodel_l { true, 0.9, 350 };
	tiertwounitmodel_l._isUnit = true;
	tiertwounitmodel_l._buildingTime = 2500;
	tiertwounitmodel_l._cost["bloc"] = 75;
	tiertwounitmodel_l._cost["ether"] = 25;
	tiertwounitmodel_l._damage = 7;
	tiertwounitmodel_l._armor = 0;
	tiertwounitmodel_l._lineOfSight = 6;
	tiertwounitmodel_l._fullReload = 2000;
	tiertwounitmodel_l._requirements._upgradeLvl["EconomicDivinity"] = 2;
	tiertwounitmodel_l._idleFunc = customIdleResource;

	lib_p.registerBuildingModel(params_p._tierTwoBuildingModelId, tiertwounitmodel_l);

	// T3 resource production buff
	TimedBuff resBuffTierThree_l;
	resBuffTierThree_l._coef = params_p._resourceProductionBuffTierThree;
	resBuffTierThree_l._type = TyppedBuff::Type::Damage;
	resBuffTierThree_l._id = "EconomicDivinity_BuffResProdTierThree";

	Upgrade * resUpTierThree_l = new Upgrade("EconomicUpgrade_BuffResProdTierThree", new StepUpgradeTyppedBuffGenerator(resBuffTierThree_l, {params_p._tierTwoBuildingModelId}));
	resUpTierThree_l->_cost["bloc"] = 250;
	resUpTierThree_l->_cost["ether"] = 450;
	resUpTierThree_l->_productionTime = 60000;
	resUpTierThree_l->_requirements._upgradeLvl["EconomicDivinity"] = 3;

	lib_p.registerUpgrade(resUpTierThree_l->_id, resUpTierThree_l);

}

}
