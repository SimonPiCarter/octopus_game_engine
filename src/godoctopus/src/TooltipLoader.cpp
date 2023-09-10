#include "TooltipLoader.h"

#include <cmath>

#include "library/model/divinity/generators/EconomicDivinityStepGenerator.hh"
#include "library/model/divinity/generators/AttackSpeedDivinityStepGenerator.hh"
#include "library/model/divinity/generators/HealDivinityStepGenerator.hh"
#include "library/model/divinity/generators/RecycleDivinityStepGenerator.hh"
#include "library/model/divinity/generators/LifestealDivinityStepGenerator.hh"
#include "library/model/divinity/generators/ProductionDivinityStepGenerator.hh"

namespace godot
{

void economicTooltips(std::string const &model_p, std::vector<std::string> &tooltipParameters_r)
{
	economic_divinity::EconomicDivinityParams params_l = economic_divinity::createDefaultParams();
	if(model_p == "EconomicUpgrade_BuffResProdTierThree")
	{
		tooltipParameters_r.push_back(std::to_string(params_l._resourceProductionBuffTierThree.to_int()));
		tooltipParameters_r.push_back(std::to_string(params_l._produceTimer));
	}
	else if(model_p == "EconomicDivId_UpgradeTierOne")
	{
		tooltipParameters_r.push_back(std::to_string(params_l._workerReducedTime.to_int()/100));
	}
	else if(model_p == "EconomicDivId_UpgradeTierThree")
	{
		tooltipParameters_r.push_back(std::to_string(int(std::floor(params_l._hpBuffTierThree.to_double()*100.))));
		tooltipParameters_r.push_back(std::to_string(int(std::floor(params_l._costDebuffTierThree.to_double()*100.))));
	}
	else if(model_p == "EconomicBuildingResourceProducerId")
	{
		tooltipParameters_r.push_back(std::to_string(params_l._resourceProduction.to_int()));
		tooltipParameters_r.push_back(std::to_string(params_l._produceTimer));
	}
}

void attackSpeedTooltips(std::string const &model_p, std::vector<std::string> &tooltipParameters_r)
{
	attackspeed_divinity::AttackSpeedDivinityParams params_l = attackspeed_divinity::createDefaultParams();

	if(model_p == "AttackSpeedDivId_UpgradeTierTwo")
	{
		tooltipParameters_r.push_back(std::to_string(params_l._tierTwoDamageIncrease.to_int()));
	}
	else if(model_p == "AttackSpeedDivId_UpgradeTierThree")
	{
		tooltipParameters_r.push_back(std::to_string(int(std::floor(params_l._tierThreeAttackSpeedIfFullLife.to_double()*100.))));
	}
	else if(model_p == "AttackspeedUpgrade_BuffTierThreeConditional")
	{
		tooltipParameters_r.push_back(std::to_string(params_l._damageUpgradeForTierOneUnit.to_int()));
	}
}

void healTooltips(std::string const &model_p, std::vector<std::string> &tooltipParameters_r)
{
	heal_divinity::HealDivinityParams params_l = heal_divinity::createDefaultParams();

	if(model_p == "HealDivId_UpgradeTierTwo")
	{
		tooltipParameters_r.push_back(std::to_string(params_l._tierTwoRegen.to_int()));
		tooltipParameters_r.push_back("1");
	}
	else if(model_p == "HealDivId_UpgradeTierThree")
	{
		tooltipParameters_r.push_back(std::to_string(params_l._healTierThree.to_int()));
		tooltipParameters_r.push_back(std::to_string(params_l._rangeHealTierThree.to_int()));
	}
	else if(model_p == "HealUpgrade_BuffTierThreeHeal")
	{
		tooltipParameters_r.push_back(std::to_string(params_l._buffHealTierThree.to_int()));
		tooltipParameters_r.push_back(std::to_string(params_l._buffHpTierThree.to_int()));
	}
	else if(model_p == "HealUpgrade_BuffTierOneHp")
	{
		tooltipParameters_r.push_back(std::to_string(params_l._buffHpTierOne.to_int()));
	}
	else if(model_p == "HealUpgrade_BuffTierTwoHeal")
	{
		tooltipParameters_r.push_back(std::to_string(params_l._buffHealTierTwo.to_int()));
	}
}

void recycleTooltips(std::string const &model_p, std::vector<std::string> &tooltipParameters_r)
{
	recycle_divinity::RecycleDivinityParams params_l = recycle_divinity::createDefaultParams();

	if(model_p == "RecycleDivId_UpgradeTierTwo")
	{
		tooltipParameters_r.push_back(std::to_string(params_l._resourceReturnedTierTwo.at("bloc").to_int()));
		tooltipParameters_r.push_back(std::to_string(params_l._resourceReturnedTierTwo.at("ether").to_int()));
	}
	else if(model_p == "RecycleUpgrade_BuffHarvest")
	{
		tooltipParameters_r.push_back(std::to_string(int(std::floor(params_l._resourceHarvestUpgrade._coef.to_double()*100.))));
	}
	else if(model_p == "RecycleUpgrade_BuffResOnDeath")
	{
		tooltipParameters_r.push_back(std::to_string(params_l._resourceReturnedTierThree.at("bloc").to_int()));
		tooltipParameters_r.push_back(std::to_string(params_l._resourceReturnedTierThree.at("ether").to_int()));
	}
}

void lifestealTooltips(std::string const &model_p, std::vector<std::string> &tooltipParameters_r)
{
	lifesteal_divinity::LifestealDivinityParams params_l = lifesteal_divinity::createDefaultParams();

	if(model_p == "LifestealUpgrade_BuffTierOneLifesteal")
	{
		tooltipParameters_r.push_back(std::to_string(int(std::floor(params_l._improvedLifeStealTierOne.to_double()*100.))));
	}
	else if(model_p == "LifestealUpgrade_BuffTierThreeLifesteal")
	{
		tooltipParameters_r.push_back(std::to_string(int(std::floor(params_l._lifeStealTierThree.to_double()*100.))));
	}
}

void productionTooltips(std::string const &model_p, std::vector<std::string> &tooltipParameters_r)
{
	production_divinity::ProductionDivinityParams params_l = production_divinity::createDefaultParams();

	if(model_p == "ProductionUpgrade_BuffProduction")
	{
		tooltipParameters_r.push_back(std::to_string(int(std::floor(params_l._productionTimeUpgradeCoef.to_double()*100.))));
	}
	else if(model_p == "ProductionUpgrade_BuffResourceProduction")
	{
		tooltipParameters_r.push_back(std::to_string(int(std::floor(params_l._prodTierOneUnitCostReductionCoef.to_double()*100.))));
	}
	else if(model_p == "ProductionUpgrade_BuffNumberBuffer")
	{
		tooltipParameters_r.push_back(std::to_string(params_l._increaseOfProductionBufferTierTwo));
	}
}

std::vector<std::string> loadTooltipParameter(Controller const *controller_p, std::string const &model_p)
{
	std::vector<std::string> tooltipParameters_l;

	economicTooltips(model_p, tooltipParameters_l);
	attackSpeedTooltips(model_p, tooltipParameters_l);
	healTooltips(model_p, tooltipParameters_l);
	recycleTooltips(model_p, tooltipParameters_l);
	lifestealTooltips(model_p, tooltipParameters_l);
	productionTooltips(model_p, tooltipParameters_l);

	return tooltipParameters_l;
}

} // namespace godot
