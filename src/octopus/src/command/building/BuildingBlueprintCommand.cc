
#include "BuildingBlueprintCommand.hh"

#include "logger/Logger.hh"
#include "step/Step.hh"
#include "state/State.hh"
#include "state/entity/Building.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/command/MissingResourceStep.hh"
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
	// If not payed we upadte player resource
	// and mark this production as paid
	if(!checkResource(state_p, _player, _model._cost))
	{
		Logger::getDebug() << "BuildingBlueprintCommand:: missing resource "<<_player <<std::endl;
		step_p.addSteppable(new MissingResourceStep(_player));
	}
	else
	{
		Logger::getDebug() << "BuildingBlueprintCommand:: spawn building "<<_player <<std::endl;
		Building building_l(_pos, true, _model);
		building_l._player = _player;
		step_p.addSteppable(new PlayerSpendResourceStep(_player, _model._cost));
		step_p.addSteppable(new BuildingSpawnStep(building_l, false));
	}
}

bool BuildingBlueprintCommand::applyCommand(Step &, State const &, CommandData const *) const
{
	// NA
	return true;
}

} // namespace octopus

