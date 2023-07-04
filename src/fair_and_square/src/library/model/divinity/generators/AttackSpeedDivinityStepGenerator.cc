#include "AttackSpeedDivinityStepGenerator.hh"

#include "controller/trigger/Listener.hh"
#include "controller/trigger/Trigger.hh"
#include "library/Library.hh"
#include "state/State.hh"
#include "state/entity/buff/ConditionalBuff.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Unit.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/player/upgrade/StepUpgradeGenerator.hh"
#include "step/Step.hh"
#include "step/player/PlayerBuffAllStep.hh"
#include "step/player/PlayerConditionalBuffAllStep.hh"

#include "UnitModelIds.hh"

using namespace octopus;

namespace attackspeed_divinity
{

class AttackSpeedUpgrade : public StepUpgradeGenerator
{
public:
	AttackSpeedUpgrade(std::vector<std::string> const &models_p, Fixed offset_p) : _models(models_p), _offset(offset_p) {}

	virtual AttackSpeedUpgrade* newCopy() const
	{
		return new AttackSpeedUpgrade(_models, _offset);
	}

	virtual std::vector<Steppable *> getSteppables(unsigned long player_p) const
	{
		std::vector<Steppable *> steps_l;

		ConditionalBuff buff_l;
		buff_l._type = TyppedBuff::Type::FullReload;
		buff_l._coef = _offset;
		buff_l._conditions.push_back(ConditionMinLife{1});
		buff_l._id = "AttackSpeedDivintiy_BuffSamageConditionalTierThree";

		for(std::string const &model_l : _models)
		{
			steps_l.push_back(new PlayerConditionalBuffAllStep(player_p, buff_l, model_l));
		}
		return steps_l;
	}
private:
	std::vector<std::string> const _models;
	Fixed const _offset;
};

std::vector<Steppable *> attackspeedTierOneGenertor(AttackSpeedDivinityParams const &param_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	// NA

	return steps_l;
}

std::vector<Steppable *> attackspeedTierTwoGenertor(AttackSpeedDivinityParams const &param_p, Library const &lib_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	TimedBuff buff_l;
	buff_l._type = TyppedBuff::Type::Damage;
	buff_l._offset = param_p._tierTwoDamageIncrease;
	buff_l._id = "AttackSpeedDivintiy_BuffDamageTierTwo";
	for(std::string const &id_l : param_p._modelsGeneralDamage)
	{
		steps_l.push_back(new PlayerBuffAllStep(player_p, buff_l, id_l));
	}

	return steps_l;
}

std::vector<Steppable *> attackspeedTierThreeGenertor(AttackSpeedDivinityParams const &param_p, Library const &lib_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	ConditionalBuff buff_l;
	buff_l._type = TyppedBuff::Type::FullReload;
	buff_l._coef = Fixed(-1) * param_p._tierThreeAttackSpeedIfFullLife;
	buff_l._conditions.push_back({ConditionMinLife{1}});
	buff_l._id = "AttackSpeedDivintiy_BuffAttackSpeedTierThree";
	for(std::string const &id_l : param_p._modelsGeneralAttackspeed)
	{
		steps_l.push_back(new PlayerConditionalBuffAllStep(player_p, buff_l, id_l));
	}

	return steps_l;
}

AttackSpeedDivinityParams createDefaultParams()
{
	AttackSpeedDivinityParams params_l;

	/// @brief tier two upgrade of damage
	params_l._tierTwoDamageIncrease = 2;
	/// @brief models affected by general attack damage upgrade (t2)
	params_l._modelsGeneralDamage = models::BasicUnitModels;

	/// @brief tier three attack speed buff when full life
	params_l._tierThreeAttackSpeedIfFullLife = 0.5;
	/// @brief models affected by general attack speed upgrade (t3)
	params_l._modelsGeneralAttackspeed = params_l._modelsGeneralDamage;

	/// @brief tier three upgrade buffing damage of tier one unit when full life
	params_l._damageUpgradeForTierOneUnit = 3;
	/// @brief tier one unit model (attackspeed unit)
	params_l._tierOneUnitModelId = models::AttackSpeedUnitModelTierOneId;
	return params_l;
}

void fillLibrary(AttackSpeedDivinityParams const &params_p, octopus::Library &lib_p)
{
	/// @brief tier one unit model (rapid fire unit)
	UnitModel tieroneunitmodel_l { false, 0.5, 0.05, 50 };
	tieroneunitmodel_l._isUnit = true;
	tieroneunitmodel_l._productionTime = 3000;
	tieroneunitmodel_l._cost["bloc"] = 25;
	tieroneunitmodel_l._cost["ether"] = 75;
	tieroneunitmodel_l._damage = 2;
	tieroneunitmodel_l._armor = 0;
	tieroneunitmodel_l._range = 4;
	tieroneunitmodel_l._lineOfSight = 10;
	tieroneunitmodel_l._fullReload = 30;
	tieroneunitmodel_l._windup = 20;
	tieroneunitmodel_l._requirements._upgradeLvl[models::AttackSpeedDivId] = 1;

	lib_p.registerUnitModel(params_p._tierOneUnitModelId, tieroneunitmodel_l);

	// declare upgrades

	// T3 attackspeed buff
	Upgrade * attackspeedBuffTierThree_l = new Upgrade("AttackspeedUpgrade_BuffTierThreeDamage", new AttackSpeedUpgrade({params_p._tierOneUnitModelId}, params_p._damageUpgradeForTierOneUnit));
	attackspeedBuffTierThree_l->_cost["bloc"] = 250;
	attackspeedBuffTierThree_l->_cost["ether"] = 350;
	attackspeedBuffTierThree_l->_cost["irium"] = 450;
	attackspeedBuffTierThree_l->_productionTime = 30000;
	attackspeedBuffTierThree_l->_requirements._upgradeLvl[models::AttackSpeedDivId] = 3;
	lib_p.registerUpgrade(attackspeedBuffTierThree_l->_id, attackspeedBuffTierThree_l);

	/// @brief temple
	BuildingModel buildingModel_l { true, 0.9, 1500 };

	models::fillTierUpgrade<AttackSpeedDivinityParams>(lib_p, params_p, models::AttackSpeedDivId,
		attackspeedTierOneGenertor,
		std::bind(attackspeedTierTwoGenertor, std::placeholders::_1, std::ref(lib_p), std::placeholders::_2),
		std::bind(attackspeedTierThreeGenertor, std::placeholders::_1, std::ref(lib_p), std::placeholders::_2),
		buildingModel_l
	);

	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel(params_p._tierOneUnitModelId));
	buildingModel_l._buildingTime = 2500;
	buildingModel_l._cost["bloc"] = 75;
	buildingModel_l._cost["ether"] = 100;
	buildingModel_l._upgrades.push_back(&lib_p.getUpgrade(attackspeedBuffTierThree_l->_id));
	buildingModel_l._requirements._upgradeLvl[models::AttackSpeedDivId] = 1;


	lib_p.registerBuildingModel(models::AttackSpeedBuildingId, buildingModel_l);
}

}
