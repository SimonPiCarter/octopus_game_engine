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

#include "UnitModelIds.hh"

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

	octopus::TimedBuff hitPointBuff_l;
	hitPointBuff_l._id = "EconomicDivinity_BuffHPTierThree";
	hitPointBuff_l._coef = params_p._hpBuffTierThree;
	hitPointBuff_l._type = octopus::TyppedBuff::Type::HpMax;

	for(std::string const &model_l : models::BasicUnitModels)
	{
		steps_l.push_back(new PlayerAddCostBonusStep(player_p, model_l, "bloc", {0, params_p._costDebuffTierThree}));
		steps_l.push_back(new PlayerAddCostBonusStep(player_p, model_l, "ether", {0, params_p._costDebuffTierThree}));

		steps_l.push_back(new PlayerBuffAllStep(player_p, hitPointBuff_l, model_l));
	}

	return steps_l;
}

EconomicDivinityParams createDefaultParams()
{
	EconomicDivinityParams params_l;

	/// @brief reduced step time for worker on tier one
	params_l._workerReducedTime = 500;

	/// @brief tier two building model (resource production)
	params_l._tierTwoBuildingModelId = models::EconomicBuildingResourceProducerId;
	/// @brief time to produce resource
	params_l._produceTimer = 10;
	/// @brief resource production
	params_l._resourceProduction = 7;

	/// @brief hp buff for every unit
	params_l._hpBuffTierThree = 1;
	/// @brief cost coef increase for every unit
	params_l._costDebuffTierThree = 1;

	/// @brief resource offset buff
	params_l._resourceProductionBuffTierThree = 5;

	return params_l;
}

// spawn blocs every n steps
void customIdleResource(Entity const &ent_p, StepShallow &step_p, State const &)
{
	if(ent_p._waiting >= ent_p.getFullReload())
	{
		std::map<std::string, Fixed> map_l;
		map_l["bloc"] = Fixed(-1) * ent_p.getDamageNoBonus();

		step_p.addSteppable(new PlayerSpendResourceStep(0, map_l));
		step_p.addSteppable(new EntityUpdateWaitingStep(ent_p._handle, ent_p._waiting, 0));
	}
}

void fillLibrary(EconomicDivinityParams const &params_p, octopus::Library &lib_p)
{
	/// @brief tier two building model
	BuildingModel tiertwounitmodel_l { true, 0.9, 350 };
	tiertwounitmodel_l._buildingTime = 2500;
	tiertwounitmodel_l._cost["bloc"] = 75;
	tiertwounitmodel_l._cost["ether"] = 25;
	tiertwounitmodel_l._damage = params_p._resourceProduction;
	tiertwounitmodel_l._armor = 0;
	tiertwounitmodel_l._lineOfSight = 6;
	tiertwounitmodel_l._fullReload = params_p._produceTimer*100;
	tiertwounitmodel_l._requirements._upgradeLvl[models::EconomicDivId+models::tierTwoSuffix] = 1;
	tiertwounitmodel_l._idleFunc = customIdleResource;

	lib_p.registerBuildingModel(params_p._tierTwoBuildingModelId, tiertwounitmodel_l);

	// T3 resource production building buff
	TimedBuff resBuffTierThree_l;
	resBuffTierThree_l._offset = params_p._resourceProductionBuffTierThree;
	resBuffTierThree_l._type = TyppedBuff::Type::Damage;
	resBuffTierThree_l._id = "EconomicDivinity_BuffResProdTierThree";

	Upgrade * resUpTierThree_l = new Upgrade("EconomicUpgrade_BuffResProdTierThree", new StepUpgradeTyppedBuffGenerator(resBuffTierThree_l, {params_p._tierTwoBuildingModelId}));
	resUpTierThree_l->_cost["bloc"] = 250;
	resUpTierThree_l->_cost["ether"] = 450;
	resUpTierThree_l->_cost["irium"] = 450;
	resUpTierThree_l->_productionTime = 12000;
	resUpTierThree_l->_requirements._upgradeLvl[models::EconomicDivId+models::tierThreeSuffix] = 1;

	lib_p.registerUpgrade(resUpTierThree_l->_id, resUpTierThree_l);

	/// @brief temple
	BuildingModel buildingModel_l { true, 1.8, 1500 };

	models::fillTierUpgrade<EconomicDivinityParams>(lib_p, params_p, models::EconomicDivId,
		economicTierOneGenertor,
		economicTierTwoGenertor,
		economicTierThreeGenertor,
		buildingModel_l
	);

	buildingModel_l._buildingTime = 2500;
	buildingModel_l._cost["bloc"] = 75;
	buildingModel_l._cost["ether"] = 100;
	buildingModel_l._upgrades.push_back(&lib_p.getUpgrade(resUpTierThree_l->_id));
	buildingModel_l._requirements._upgradeLvl[models::EconomicDivId] = 1;

	lib_p.registerBuildingModel(models::EconomicBuildingId, buildingModel_l);
}

}
