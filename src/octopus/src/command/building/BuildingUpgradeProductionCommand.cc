
#include "BuildingUpgradeProductionCommand.hh"

#include <cmath>

#include "command/entity/EntityRallyPointCommand.hh"
#include "logger/Logger.hh"
#include "state/State.hh"
#include "state/entity/Building.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/player/Player.hh"
#include "step/Step.hh"
#include "step/player/PlayerLevelUpUpgradeStep.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/command/MissingResourceStep.hh"
#include "step/command/ProductionPaidStep.hh"
#include "step/command/ProductionProgressionStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/player/PlayerProducedUpgradeStep.hh"

namespace octopus
{

BuildingUpgradeProductionCommand::BuildingUpgradeProductionCommand(Handle const &commandHandle_p, Handle const &source_p, Upgrade const &upgrade_p)
	: Command(commandHandle_p)
	, _source(source_p)
	, _upgrade(&upgrade_p)
	, _data(upgrade_p)
{}

void BuildingUpgradeProductionCommand::registerCommand(Step & step_p, State const &state_p)
{
	Logger::getDebug() << "BuildingUpgradeProductionCommand:: register Command "<<_source <<std::endl;
	Building const * building_l = dynamic_cast<Building const *>(state_p.getEntity(_source));

	// if building is not build we skip everything
	if(!building_l->isBuilt())
	{
		return;
	}

	Player const &player_l = *state_p.getPlayer(building_l->_player);
	// check if we can pay for it and if building can produce it
	if(checkResource(state_p, building_l->_player, _upgrade->_cost, step_p.getResourceSpent(building_l->_player))
	&& building_l->_buildingModel.canProduce(_upgrade)
	&& meetRequirements(_upgrade->_requirements, player_l)
	&& (_upgrade->_repeatable || player_l._producedUpgrade.find(_upgrade->_id) == player_l._producedUpgrade.end()))
	{
		step_p.addSteppable(new PlayerSpendResourceStep(building_l->_player, _upgrade->_cost));
		step_p.addSteppable(new CommandSpawnStep(this));
		step_p.addSteppable(new PlayerProducedUpgradeStep(building_l->_player, _upgrade->_id, true));
	}
	// else add informative step for failure
	else
	{
		step_p.addSteppable(new MissingResourceStep(building_l->_player));
		step_p.addSteppable(new CommandStorageStep(this));
	}
}

bool BuildingUpgradeProductionCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *, PathManager &) const
{
	Logger::getDebug() << "BuildingUpgradeProductionCommand:: apply Command "<<_source <<std::endl;
	Building const * building_l = dynamic_cast<Building const *>(state_p.getEntity(_source));

	if(_data._canceled)
	{
		Logger::getDebug() << "BuildingUpgradeProductionCommand:: canceled Command "<<_source <<std::endl;
		return true;
	}

	if(_data._progression < _data._upgrade->_productionTime)
	{
		Logger::getDebug() << "BuildingUpgradeProductionCommand :: adding production progression step " <<std::endl;
		step_p.addSteppable(new ProductionProgressionStep(_handleCommand, building_l->getProduction()));
	}
	else
	{
		Logger::getDebug() << "BuildingUpgradeProductionCommand :: adding spawn step " <<std::endl;

		// set ugrade as researched
		step_p.addSteppable(new PlayerLevelUpUpgradeStep(building_l->_player, _upgrade->_id));
		step_p.addSteppable(new PlayerProducedUpgradeStep(building_l->_player, _upgrade->_id, false));
		if(_upgrade->_generator)
		{
			// set all steppable unlocked by the upgrade
			for(Steppable *steppable_l : _upgrade->_generator->getSteppables(building_l->_player))
			{
				step_p.addSteppable(steppable_l);
			}
		}

		return true;
	}

	return false;
}

} // namespace octopus

