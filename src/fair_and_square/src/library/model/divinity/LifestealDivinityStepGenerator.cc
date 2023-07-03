#include "LifestealDivinityStepGenerator.hh"

#include "controller/trigger/Listener.hh"
#include "controller/trigger/Trigger.hh"
#include "library/Library.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Unit.hh"
#include "state/entity/attackModifier/modifiers/LifeStealModifier.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/player/upgrade/StepUpgradeGenerator.hh"
#include "step/Step.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/player/PlayerAttackModAllStep.hh"
#include "step/player/PlayerBuffAllStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/player/bonus/PlayerAddTimeProdBonusStep.hh"
#include "step/player/bonus/PlayerAddCostBonusStep.hh"
#include "step/trigger/TriggerSpawn.hh"

#include "UnitModelIds.hh"

using namespace octopus;

namespace lifesteal_divinity
{

class LifestealUpgrade : public StepUpgradeGenerator
{
public:
	LifestealUpgrade(std::vector<std::string> const &models_p, Fixed ratio_p) : _models(models_p), _ratio(ratio_p) {}

	virtual LifestealUpgrade* newCopy() const
	{
		return new LifestealUpgrade(_models, _ratio);
	}

	virtual std::vector<Steppable *> getSteppables(unsigned long player_p) const
	{
		std::vector<Steppable *> steps_l;
		for(std::string const &model_l : _models)
		{
			steps_l.push_back(new PlayerAttackModAllStep(player_p, LifeStealModifier(_ratio), model_l));
		}
		return steps_l;
	}
private:
	std::vector<std::string> const _models;
	Fixed const _ratio;
};

std::vector<Steppable *> lifestealTierOneGenertor(LifestealDivinityParams const &param_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	// NA

	return steps_l;
}

std::vector<Steppable *> lifestealTierTwoGenertor(LifestealDivinityParams const &param_p, Library const &lib_p, unsigned long player_p)
{
	LifestealUpgrade up_l(param_p._modelsGeneralLifeSteal, param_p._lifeStealTierTwo);
	return up_l.getSteppables(player_p);
}

std::vector<Steppable *> lifestealTierThreeGenertor(LifestealDivinityParams const &param_p, Library const &lib_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	// NA (enable ability)

	return steps_l;
}

LifestealDivinityParams createDefaultParams()
{
	LifestealDivinityParams params_l;

	params_l._modelsGeneralLifeSteal = models::BasicUnitModels;

	/// @brief tier one upgrade of life steal
	params_l._improvedLifeStealTierOne = 0.25;
	/// @brief tier two upgrade of life steal for all units
	params_l._lifeStealTierTwo = 0.05;
	/// @brief tier three upgrade of life steal for all units
	params_l._lifeStealTierThree = 0.15;
	/// @brief tier one unit model (lifesteal unit)
	params_l._tierOneUnitModelId = models::LifestealUnitModelTierOneId;

	TimedBuff buff_l;
	buff_l._coef = -0.5;
	buff_l._type = TyppedBuff::Type::FullReload;
	buff_l._duration = 1000;
	buff_l._id = "LifeStealDivinity_Stimpack";

	params_l._stimBuff = buff_l;
	params_l._stimHpCost = 10;

	return params_l;
}

namespace stimpack
{
	void runnable(TimedBuff buff_p, Fixed hpCost_p, Step &step_p, State const &state_p, Handle const &source_p, Handle const &, Vector const &)
	{
		step_p.addSteppable(new EntityBuffStep(source_p, buff_p));

		Entity const *ent_l = state_p.getEntity(source_p);
		Fixed curHp_l = ent_l->_hp + step_p.getHpChange(source_p);
		Fixed maxHp_l = ent_l->getHpMax();
		step_p.addSteppable(new EntityHitPointChangeStep(source_p, Fixed(-1) * hpCost_p, curHp_l, maxHp_l));
	}

	bool checker(Step const &, State const &state_p, Handle const &source_p, Handle const &, Vector const &)
	{
		Requirements req_l;
		req_l._upgradeLvl["LifestealDivinity"] = 3;
		return meetRequirements(req_l, *state_p.getPlayer(state_p.getEntity(source_p)->_player));
	}

} // namespace stimpack


void fillLibrary(LifestealDivinityParams const &params_p, octopus::Library &lib_p)
{
	/// @brief tier one unit model (lfie steal unit)
	UnitModel tieroneunitmodel_l { false, 0.5, 0.05, 135 };
	tieroneunitmodel_l._isUnit = true;
	tieroneunitmodel_l._productionTime = 3500;
	tieroneunitmodel_l._cost["bloc"] = 30;
	tieroneunitmodel_l._cost["ether"] = 100;
	tieroneunitmodel_l._damage = 20;
	tieroneunitmodel_l._armor = 0;
	tieroneunitmodel_l._range = 0.5;
	tieroneunitmodel_l._lineOfSight = 10;
	tieroneunitmodel_l._fullReload = 200;
	tieroneunitmodel_l._windup = 20;
	tieroneunitmodel_l._defaultAttackMod = LifeStealModifier(0.10);
	tieroneunitmodel_l._requirements._upgradeLvl["LifestealDivinity"] = 1;

	tieroneunitmodel_l._abilities["LifeStealDivinity_Stimpack"]._id = "LifeStealDivinity_Stimpack";
	tieroneunitmodel_l._abilities["LifeStealDivinity_Stimpack"]._reloadKey = "LifeStealDivinity_Stimpack";
	tieroneunitmodel_l._abilities["LifeStealDivinity_Stimpack"]._runnable = std::bind(stimpack::runnable, params_p._stimBuff, params_p._stimHpCost,
													std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
	tieroneunitmodel_l._abilities["LifeStealDivinity_Stimpack"]._checker = stimpack::checker;

	lib_p.registerUnitModel(params_p._tierOneUnitModelId, tieroneunitmodel_l);

	// declare upgrades

	// T1 lifesteal buff
	Upgrade * lifestealBuffTierOne_l = new Upgrade("LifestealUpgrade_BuffTierOneLifesteal", new LifestealUpgrade({params_p._tierOneUnitModelId}, params_p._improvedLifeStealTierOne));
	lifestealBuffTierOne_l->_cost["bloc"] = 125;
	lifestealBuffTierOne_l->_cost["ether"] = 125;
	lifestealBuffTierOne_l->_productionTime = 40000;
	lifestealBuffTierOne_l->_requirements._upgradeLvl["LifestealDivinity"] = 1;
	lib_p.registerUpgrade(lifestealBuffTierOne_l->_id, lifestealBuffTierOne_l);

	// T3 lifesteal buff
	Upgrade * lifestealBuffTierThree_l = new Upgrade("LifestealUpgrade_BuffTierThreeLifesteal", new LifestealUpgrade(params_p._modelsGeneralLifeSteal, params_p._lifeStealTierThree));
	lifestealBuffTierThree_l->_cost["bloc"] = 250;
	lifestealBuffTierThree_l->_cost["ether"] = 250;
	lifestealBuffTierThree_l->_productionTime = 60000;
	lifestealBuffTierThree_l->_requirements._upgradeLvl["LifestealDivinity"] = 3;
	lib_p.registerUpgrade(lifestealBuffTierThree_l->_id, lifestealBuffTierThree_l);

	/// @brief temple
	BuildingModel buildingModel_l { true, 0.9, 1500 };
	buildingModel_l._buildingTime = 2500;
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel(params_p._tierOneUnitModelId));
	buildingModel_l._cost["bloc"] = 75;
	buildingModel_l._cost["ether"] = 100;
	buildingModel_l._upgrades.push_back(&lib_p.getUpgrade(lifestealBuffTierOne_l->_id));
	buildingModel_l._upgrades.push_back(&lib_p.getUpgrade(lifestealBuffTierThree_l->_id));

	lib_p.registerBuildingModel(models::LifestealBuildingId, buildingModel_l);
}

}
