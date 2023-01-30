
#include "BuildingUnitProductionCommand.hh"

#include <cmath>

#include "logger/Logger.hh"
#include "state/State.hh"
#include "state/entity/Building.hh"
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
	, _model(model_p)
{}

void BuildingUnitProductionCommand::registerCommand(Step & step_p, State const &state_p)
{
	Logger::getDebug() << "BuildingUnitProductionCommand:: register Command "<<_source <<std::endl;
	Building const * building_l = dynamic_cast<Building const *>(state_p.getEntity(_source));

	// check if we can pay for it
	if(checkResource(state_p, building_l->_player, _model._cost))
	{
		step_p.addSteppable(new PlayerSpendResourceStep(building_l->_player, _model._cost));
		step_p.addSteppable(new CommandSpawnStep(this));
	}
	// else add informative step for failure
	else
	{
		step_p.addSteppable(new MissingResourceStep(building_l->_player));
	}
}

bool BuildingUnitProductionCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *data_p) const
{
	// get data
	UnitProductionData const &data_l = *static_cast<UnitProductionData const *>(data_p);

	Logger::getDebug() << "BuildingUnitProductionCommand:: apply Command "<<_source <<std::endl;
	Building const * building_l = dynamic_cast<Building const *>(state_p.getEntity(_source));

	if(data_l._progression < data_l._model._productionTime)
	{
		Logger::getDebug() << "BuildingUnitProductionCommand :: adding production progression step " <<std::endl;
		step_p.addSteppable(new ProductionProgressionStep(_handleCommand, building_l->getProduction()));
	}
	else
	{
		Logger::getDebug() << "BuildingUnitProductionCommand :: adding spawn step " <<std::endl;

		Unit unit_l(building_l->_pos + building_l->_buildingModel._productionOutput, false, data_l._model);
		unit_l._player = building_l->_player;
		step_p.addSteppable(new UnitSpawnStep(unit_l));

		return true;
	}

	return false;
}

} // namespace octopus
