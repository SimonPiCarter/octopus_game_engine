
#include "BuildingAutoBuildCommand.hh"

#include <cmath>

#include "logger/Logger.hh"
#include "state/State.hh"
#include "state/entity/Building.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/player/Player.hh"
#include "step/Step.hh"
#include "step/building/BuildingAutoBuildStep.hh"
#include "step/command/CommandQueueStep.hh"

namespace octopus
{

BuildingAutoBuildCommand::BuildingAutoBuildCommand(Handle const &source_p, UnitModel const *model_p)
	: Command(source_p)
	, _source(source_p)
	, _model(model_p)
{}

void BuildingAutoBuildCommand::registerCommand(Step & step_p, State const &state_p)
{
	Logger::getDebug() << "BuildingAutoBuildCommand:: register Command "<<_source <<std::endl;
	Building const * building_l = dynamic_cast<Building const *>(state_p.getEntity(_source));
	Player const & player_l = *state_p.getPlayer(building_l->_player);

	// if building is not build we skip everything
	if(!building_l->isBuilt() || !_model)
	{
		return;
	}

	bool req_l = meetRequirements(_model->_requirements, *state_p.getPlayer(building_l->_player));
	// check if we can pay for it and if building can produce it
	if(building_l->_buildingModel.canProduce(_model)
	&& req_l)
	{
		step_p.addSteppable(new BuildingAutoBuildStep(_source, building_l->_autoBuild, _model));
	}
	step_p.addSteppable(new CommandStorageStep(this));
}

bool BuildingAutoBuildCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *, PathManager &) const
{
	// NA
	return true;
}

std::string BuildingAutoBuildCommand::getModelId() const
{
	return _model? _model->_id : "";
}

} // namespace octopus

