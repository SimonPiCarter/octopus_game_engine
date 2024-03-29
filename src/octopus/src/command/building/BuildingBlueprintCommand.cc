
#include "BuildingBlueprintCommand.hh"

#include "command/entity/EntityBuildingCommand.hh"
#include "logger/Logger.hh"
#include "step/Step.hh"
#include "state/State.hh"
#include "state/entity/Building.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/command/MissingResourceStep.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"

namespace octopus
{

BuildingBlueprintCommand::BuildingBlueprintCommand()
	: Command(Handle(0))
	, _pos(0,0)
	, _player(0)
	, _model(nullptr)
{}

BuildingBlueprintCommand::BuildingBlueprintCommand(Vector const &pos_p, unsigned long player_p, BuildingModel const &model_p, std::vector<Handle> const &builders_p)
	: Command(Handle(0))
	, _pos(pos_p)
	, _player(player_p)
	, _model(&model_p)
	, _builders(builders_p)
{}

void BuildingBlueprintCommand::registerCommand(Step & step_p, State const &state_p)
{
	Logger::getDebug() << "BuildingBlueprintCommand:: register Command "<<_player <<std::endl;
	Building building_l(_pos, true, *_model);
	building_l._player = _player;
	// If not payed we update player resource
	// and mark this production as paid
	std::string missingRes_l = checkResource(state_p, _player, _model->_cost, step_p.getResourceSpent(_player));
	if(missingRes_l != "")
	{
		Logger::getDebug() << "BuildingBlueprintCommand:: missing resource "<<_player <<std::endl;
		step_p.addSteppable(new MissingResourceStep(_player, missingRes_l));
	}
	else if(!meetRequirements(_model->_requirements, *state_p.getPlayer(_player)))
	{
		Logger::getDebug() << "BuildingBlueprintCommand:: missing requirements "<<_player <<std::endl;
		step_p.addSteppable(new MissingResourceStep(_player, MissingResourceStep::MissingRequirement));
	}
	else if(!checkExplored(state_p, &building_l, _player))
	{
		Logger::getDebug() << "BuildingBlueprintCommand:: unexplored zone "<<_player <<std::endl;
		step_p.addSteppable(new MissingResourceStep(_player, MissingResourceStep::BadPlacement));
	}
	else
	{
		if(noOutOfBounds(state_p, building_l) && _model->checkLegality(building_l, state_p))
		{
			Handle buildingHandle_l = getNextHandle(step_p, state_p);
			Logger::getDebug() << "BuildingBlueprintCommand:: spawn building "<<buildingHandle_l<<" for player "<<_player <<std::endl;
			step_p.addSteppable(new PlayerSpendResourceStep(_player, _model->_cost));
			step_p.addSteppable(new BuildingSpawnStep(buildingHandle_l, building_l, false));
			for(Handle const &handle_l : _builders)
			{
				if(state_p.isEntityAlive(handle_l) && state_p.getEntity(handle_l)->_player == _player)
				{
					Command * cmd_l = new EntityBuildingCommand(handle_l, handle_l, buildingHandle_l, _pos, 0, {_pos}, true);
					cmd_l->setQueued(_queued);
					step_p.addSteppable(new CommandSpawnStep(cmd_l));
				}
			}
		}
	}

	step_p.addSteppable(new CommandStorageStep(this));
}

bool BuildingBlueprintCommand::applyCommand(Step &, State const &, CommandData const *, PathManager &) const
{
	// NA
	return true;
}

} // namespace octopus

