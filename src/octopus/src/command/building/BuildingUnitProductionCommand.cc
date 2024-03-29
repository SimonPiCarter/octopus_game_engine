
#include "BuildingUnitProductionCommand.hh"

#include <cmath>

#include "command/entity/EntityRallyPointCommand.hh"
#include "logger/Logger.hh"
#include "state/State.hh"
#include "state/entity/Building.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/player/Player.hh"
#include "step/Step.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/command/MissingResourceStep.hh"
#include "step/command/ProductionPaidStep.hh"
#include "step/command/ProductionProgressionStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"

namespace octopus
{

BuildingUnitProductionCommand::BuildingUnitProductionCommand(Handle const &commandHandle_p, Handle const &source_p, UnitModel const &model_p)
	: Command(commandHandle_p)
	, _source(source_p)
	, _model(&model_p)
	, _data(model_p)
{}

void BuildingUnitProductionCommand::registerCommand(Step & step_p, State const &state_p)
{
	Logger::getDebug() << "BuildingUnitProductionCommand:: register Command "<<_source <<std::endl;
	Building const * building_l = dynamic_cast<Building const *>(state_p.getEntity(_source));
	Player const & player_l = *state_p.getPlayer(building_l->_player);

	// if building is not build we skip everything
	if(!building_l->isBuilt())
	{
		return;
	}

	std::map<std::string, Fixed> cost_l = getCost(*_model, player_l);

	std::string missingRes_l = checkResource(state_p, building_l->_player, cost_l, step_p.getResourceSpent(building_l->_player));
	bool req_l = meetRequirements(_model->_requirements, *state_p.getPlayer(building_l->_player));
	// check if we can pay for it and if building can produce it
	if(missingRes_l == ""
	&& building_l->_buildingModel.canProduce(_model)
	&& req_l)
	{
		step_p.addSteppable(new PlayerSpendResourceStep(building_l->_player, cost_l));
		step_p.addSteppable(new CommandSpawnStep(this));
	}
	// else add informative step for failure
	else if(missingRes_l != "")
	{
		step_p.addSteppable(new MissingResourceStep(building_l->_player, missingRes_l));
		step_p.addSteppable(new CommandStorageStep(this));
	}
	else if(!req_l)
	{
		step_p.addSteppable(new MissingResourceStep(building_l->_player, MissingResourceStep::MissingRequirement));
		step_p.addSteppable(new CommandStorageStep(this));
	}
	else
	{
		step_p.addSteppable(new CommandStorageStep(this));
	}
}

bool BuildingUnitProductionCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *, PathManager &) const
{
	Logger::getDebug() << "BuildingUnitProductionCommand:: apply Command "<<_source <<std::endl;
	Building const * building_l = dynamic_cast<Building const *>(state_p.getEntity(_source));
	Player const & player_l = *state_p.getPlayer(building_l->_player);

	if(_data._canceled)
	{
		Logger::getDebug() << "BuildingUnitProductionCommand:: canceled Command "<<_source <<std::endl;
		return true;
	}

	if(_data._progression < getProductionTime(*_data._model, player_l))
	{
		Logger::getDebug() << "BuildingUnitProductionCommand :: adding production progression step " <<std::endl;
		step_p.addSteppable(new ProductionProgressionStep(_handleCommand, building_l->getProduction()));
	}
	else
	{
		Logger::getDebug() << "BuildingUnitProductionCommand :: adding spawn step " <<std::endl;

		Unit unit_l(building_l->_pos + building_l->_buildingModel._productionOutput, false, *_data._model);
		unit_l._player = player_l._id;
		Handle handle_l = getNextHandle(step_p, state_p);
		step_p.addSteppable(new UnitSpawnStep(handle_l, unit_l));

		if(building_l->_rallyPointActive)
		{
			step_p.addSteppable(new CommandSpawnStep(new EntityRallyPointCommand(handle_l, building_l->_rallyPointEntity, building_l->_rallyPoint, !building_l->_rallyPointEntityActive)));
		}

		return true;
	}

	return false;
}

} // namespace octopus

