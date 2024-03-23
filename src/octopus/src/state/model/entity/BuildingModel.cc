#include "BuildingModel.hh"

#include <list>

#include "command/building/BuildingUnitProductionCommand.hh"
#include "utils/Vector.hh"
#include "EntityModel.hh"
#include "state/State.hh"
#include "state/entity/Building.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/EntityUpdateWaitingStep.hh"
#include "step/Step.hh"

namespace octopus
{

BuildingModel::BuildingModel(bool isStatic_p, Fixed ray_p, Fixed hpMax_p)
	: EntityModel(isStatic_p, ray_p, 0., hpMax_p)
{
	_idleFunc = buildingIdleFunction;
	_isBuilding = true;
	_stepSpeed = 0.;
	_damage = 0.;
}

bool BuildingModel::isDeposit(std::string type_p) const
{
	auto && it_l = _deposit.find(type_p);
	if(it_l != _deposit.end())
	{
		return it_l->second;
	}
	return false;
}

bool BuildingModel::isAnyDeposit() const
{
	for(auto && pair_l : _deposit)
	{
		if(pair_l.second)
		{
			return true;
		}
	}
	return false;
}

bool BuildingModel::isProduction() const
{
	return !_unitModels.empty() || !_upgrades.empty();
}

bool BuildingModel::canProduce(UnitModel const * model_p) const
{
	return std::find(_unitModels.begin(), _unitModels.end(), model_p) != _unitModels.end();
}

bool BuildingModel::canProduce(Upgrade const * upgrade_p) const
{
	return std::find(_upgrades.begin(), _upgrades.end(), upgrade_p) != _upgrades.end();
}

bool BuildingModel::checkLegality(Building const &, State const &) const
{
	return true;
}

bool BuildingModel::checkGrid(Building const &building_p, State const &state_p) const
{
	return octopus::checkGrid(state_p, &building_p, false);
}

void buildingIdleFunction(Entity const &ent_p, Step & step_p, State const &state_p)
{
	static unsigned long const waitTime_l = 50;
	// check for build every 50 steps
	Command * cmd_l = nullptr;
	Building const &building_l = static_cast<Building const &>(ent_p);

	if(building_l._autoBuild)
	{
		cmd_l = new BuildingUnitProductionCommand(building_l._handle, building_l._handle, *building_l._autoBuild);
		cmd_l->registerCommand(step_p, state_p);
	}

	if(cmd_l)
	{
		// reset waiting time
		step_p.addSteppable(new EntityUpdateWaitingStep(ent_p._handle, ent_p._waiting, 0));
	}
	else if(ent_p._waiting >= waitTime_l)
	{
		step_p.addSteppable(new EntityUpdateWaitingStep(ent_p._handle, ent_p._waiting, 0));
	}
}

} // namespace octopus
