
#include "BuildingBlueprintCommand.hh"

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

BuildingBlueprintCommand::BuildingBlueprintCommand(Vector const &pos_p, unsigned long player_p, BuildingModel const &model_p)
	: Command(0)
	, _pos(pos_p)
	, _player(player_p)
	, _model(model_p)
{}

void BuildingBlueprintCommand::registerCommand(Step & step_p, State const &state_p)
{
	Logger::getDebug() << "BuildingBlueprintCommand:: register Command "<<_player <<std::endl;
	// If not payed we update player resource
	// and mark this production as paid
	if(!checkResource(state_p, _player, _model._cost, step_p.getResourceSpent(_player)))
	{
		Logger::getDebug() << "BuildingBlueprintCommand:: missing resource "<<_player <<std::endl;
		step_p.addSteppable(new MissingResourceStep(_player));
	}
	else
	{
		Building building_l(_pos, true, _model);
		building_l._player = _player;
		if(_model.checkLegality(building_l, state_p))
		{
			Logger::getDebug() << "BuildingBlueprintCommand:: spawn building "<<_player <<std::endl;
			step_p.addSteppable(new PlayerSpendResourceStep(_player, _model._cost));
			step_p.addSteppable(new BuildingSpawnStep(getNextHandle(step_p, state_p), building_l, false));
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

