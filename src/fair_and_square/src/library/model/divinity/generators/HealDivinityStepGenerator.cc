#include "HealDivinityStepGenerator.hh"

#include "controller/trigger/Listener.hh"
#include "controller/trigger/Trigger.hh"
#include "library/Library.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Unit.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/player/upgrade/StepUpgradeTyppedBuffGenerator.hh"
#include "step/Step.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/player/PlayerBuffAllStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/trigger/TriggerSpawn.hh"

#include "UnitModelIds.hh"

using namespace octopus;

namespace heal_divinity
{

/// @brief will heal every unit of same player when a unit die
class HealDeathHealAoeTrigger : public OnEachTrigger
{
public:
	HealDeathHealAoeTrigger(unsigned long player_p, Fixed const &aoe_p, Fixed const &qty_p) :
		OnEachTrigger(new ListenerEntityModelDied(nullptr, player_p)),
		_player(player_p),
		_aoe(aoe_p),
		_qty(qty_p)
	{}

	virtual void trigger(State const &state_p, Step &step_p, unsigned long count_p, TriggerData const &data_p) const override
	{
		ListenerEntityData const * entData_l = dynamic_cast<ListenerEntityData const * >(data_p._listenerData[0]);
		Entity const * ent_l = entData_l->_entities[count_p];

		/// @brief get all within range
		TargetPanel panel_l(lookUpNewTargets(state_p, ent_l->_handle, _aoe, false));

		for(Entity const * subTarget_l : panel_l.units)
		{
			// same player and different entity
			if(subTarget_l->_player == ent_l->_player
			&& ent_l->_handle != subTarget_l->_handle)
			{
				Fixed curHp_l = subTarget_l->_hp + step_p.getHpChange(subTarget_l->_handle);
				Fixed maxHp_l = subTarget_l->getHpMax();
				step_p.addSteppable(new EntityHitPointChangeStep(subTarget_l->_handle, _qty, curHp_l, maxHp_l));
			}
		}
	}

private:
	unsigned long const _player;
	Fixed const _aoe;
	Fixed const _qty;
};

std::vector<Steppable *> healTierOneGenertor(HealDivinityParams const &param_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	// NA

	return steps_l;
}

std::vector<Steppable *> healTierTwoGenertor(HealDivinityParams const &param_p, Library const &lib_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	TimedBuff regenBuff_l;
	regenBuff_l._offset = param_p._tierTwoRegen;
	regenBuff_l._type = TyppedBuff::Type::HpRegeneration;
	regenBuff_l._id = "HealDivinity_RegenBuffTierTwo";

	for(std::string const &id_l : models::AllUnitModels)
	{
		steps_l.push_back(new PlayerBuffAllStep(player_p, regenBuff_l, id_l));
	}

	return steps_l;
}

std::vector<Steppable *> healTierThreeGenertor(HealDivinityParams const &param_p, Library const &lib_p, unsigned long player_p)
{
	std::vector<Steppable *> steps_l;

	steps_l.push_back(new TriggerSpawn(new HealDeathHealAoeTrigger(player_p, param_p._rangeHealTierThree, param_p._healTierThree)));

	return steps_l;
}

HealDivinityParams createDefaultParams()
{
	HealDivinityParams params_l;

	/// @brief hp buff of tier one unit from upgrage tier one
	params_l._buffHpTierOne = 45;
	/// @brief regen for all units from tier two
	params_l._tierTwoRegen = 2;
	/// @brief heal buff of tier one unit from upgrade tier two
	params_l._buffHealTierTwo = 5;

	/// @brief heal amount from tier three
	params_l._healTierThree = 5;
	/// @brief heal range from tier three
	params_l._rangeHealTierThree = 3;

	/// @brief hp buff of tier one unit from upgrage tier three
	params_l._buffHpTierThree = 45;
	/// @brief heal buff of tier one unit from upgrade tier three
	params_l._buffHealTierThree = 5;

	/// @brief tier one unit model (heal unit)
	params_l._tierOneUnitModelId = models::HealUnitModelTierOneId;

	return params_l;
}

void fillLibrary(HealDivinityParams const &params_p, octopus::Library &lib_p)
{
	/// @brief tier one unit model (heal unit)
	UnitModel tieroneunitmodel_l { false, 0.5, 0.04, 90 };
	tieroneunitmodel_l._isUnit = true;
	tieroneunitmodel_l._productionTime = 5500;
	tieroneunitmodel_l._cost["bloc"] = 100;
	tieroneunitmodel_l._cost["ether"] = 150;
	tieroneunitmodel_l._damage = 0;
	tieroneunitmodel_l._heal = 10;
	tieroneunitmodel_l._armor = 0;
	tieroneunitmodel_l._range = 5.;
	tieroneunitmodel_l._lineOfSight = 10;
	tieroneunitmodel_l._fullReload = 100;
	tieroneunitmodel_l._windup = 20;
	tieroneunitmodel_l._requirements._upgradeLvl[models::HealDivId+models::tierOneSuffix] = 1;

	lib_p.registerUnitModel(params_p._tierOneUnitModelId, tieroneunitmodel_l);

	// declare upgrades

	// T1 hp buff
	TimedBuff hpBuffTierOne_l;
	hpBuffTierOne_l._offset = params_p._buffHpTierOne;
	hpBuffTierOne_l._type = TyppedBuff::Type::HpMax;
	hpBuffTierOne_l._id = "HealDivinity_HpBuffTierOne";

	Upgrade * healUpHpTierOne_l = new Upgrade("HealUpgrade_BuffTierOneHp", new StepUpgradeTyppedBuffGenerator(hpBuffTierOne_l, {params_p._tierOneUnitModelId}));
	healUpHpTierOne_l->_cost["bloc"] = 125;
	healUpHpTierOne_l->_cost["ether"] = 175;
	healUpHpTierOne_l->_productionTime = 9000;
	healUpHpTierOne_l->_requirements._upgradeLvl[models::HealDivId+models::tierOneSuffix] = 1;
	lib_p.registerUpgrade(healUpHpTierOne_l->_id, healUpHpTierOne_l);

	// T2 heal buff
	TimedBuff healBuffTierTwo_l;
	healBuffTierTwo_l._offset = params_p._buffHealTierTwo;
	healBuffTierTwo_l._type = TyppedBuff::Type::Heal;
	healBuffTierTwo_l._id = "HealDivinity_HealBuffTierTwo";

	Upgrade * healUpHealTierTwo_l = new Upgrade("HealUpgrade_BuffTierTwoHeal", new StepUpgradeTyppedBuffGenerator(healBuffTierTwo_l, {params_p._tierOneUnitModelId}));
	healUpHealTierTwo_l->_cost["bloc"] = 250;
	healUpHealTierTwo_l->_cost["ether"] = 250;
	healUpHealTierTwo_l->_cost["irium"] = 100;
	healUpHealTierTwo_l->_productionTime = 12000;
	healUpHealTierTwo_l->_requirements._upgradeLvl[models::HealDivId+models::tierTwoSuffix] = 1;
	lib_p.registerUpgrade(healUpHealTierTwo_l->_id, healUpHealTierTwo_l);

	// T3 heal/hp buff
	TimedBuff hpBuffTierThree_l;
	hpBuffTierThree_l._offset = params_p._buffHpTierThree;
	hpBuffTierThree_l._type = TyppedBuff::Type::HpMax;
	hpBuffTierThree_l._id = "HealDivinity_HpBuffTierThree";
	TimedBuff healBuffTierThree_l;
	healBuffTierThree_l._offset = params_p._buffHealTierThree;
	healBuffTierThree_l._type = TyppedBuff::Type::Heal;
	healBuffTierThree_l._id = "HealDivinity_HealBuffTierThree";

	Upgrade * healUpHealHpTierThree_l = new Upgrade("HealUpgrade_BuffTierThreeHeal", new StepUpgradeTyppedBuffGenerator({hpBuffTierThree_l, healBuffTierThree_l}, {params_p._tierOneUnitModelId}));
	healUpHealHpTierThree_l->_cost["bloc"] = 250;
	healUpHealHpTierThree_l->_cost["ether"] = 350;
	healUpHealHpTierThree_l->_cost["irium"] = 450;
	healUpHealHpTierThree_l->_productionTime = 24000;
	healUpHealHpTierThree_l->_requirements._upgradeLvl[models::HealDivId+models::tierThreeSuffix] = 1;
	lib_p.registerUpgrade(healUpHealHpTierThree_l->_id, healUpHealHpTierThree_l);

	/// @brief temple
	BuildingModel buildingModel_l { true, 1.8, 1500 };

	models::fillTierUpgrade<HealDivinityParams>(lib_p, params_p, models::HealDivId,
		healTierOneGenertor,
		std::bind(healTierTwoGenertor, std::placeholders::_1, std::ref(lib_p), std::placeholders::_2),
		std::bind(healTierThreeGenertor, std::placeholders::_1, std::ref(lib_p), std::placeholders::_2),
		buildingModel_l
	);

	buildingModel_l._buildingTime = 2500;
	buildingModel_l._unitModels.push_back(&lib_p.getUnitModel(params_p._tierOneUnitModelId));
	buildingModel_l._cost["bloc"] = 75;
	buildingModel_l._cost["ether"] = 100;
	buildingModel_l._upgrades.push_back(&lib_p.getUpgrade(healUpHpTierOne_l->_id));
	buildingModel_l._upgrades.push_back(&lib_p.getUpgrade(healUpHealTierTwo_l->_id));
	buildingModel_l._upgrades.push_back(&lib_p.getUpgrade(healUpHealHpTierThree_l->_id));
	buildingModel_l._requirements._upgradeLvl[models::HealDivId] = 1;

	lib_p.registerBuildingModel(models::HealBuildingId, buildingModel_l);
}

}
