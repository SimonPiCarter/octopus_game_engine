
#include "PlayerChoseDivinityCommand.hh"

#include "logger/Logger.hh"
#include "step/Step.hh"
#include "state/State.hh"
#include "state/player/Player.hh"
#include "step/player/PlayerAnchorDivinityStep.hh"
#include "step/player/PlayerLevelUpDivinityStep.hh"
#include "step/player/PlayerResetOptionDivinityStep.hh"

namespace octopus
{

bool checkOption(Player const &player_p, DivinityType type_p)
{
	if(player_p._divOptions.find(type_p) != player_p._divOptions.end()
	&& player_p._divOptions.at(type_p) > 0)
	{
		return true;
	}
	return false;
}

bool checkLvlUp(Player const &player_p)
{
	unsigned long lvlUp_l = 0;
	for(auto &&pair_l : player_p._divLvl)
	{
		if(pair_l.second > 1)
		{
			++lvlUp_l;
		}
	}
	/// @todo read from data how much
	return lvlUp_l < 3;
}

PlayerChoseDivinityCommand::PlayerChoseDivinityCommand(Handle const &commandHandle_p, unsigned long player_p, DivinityType type_p, bool lvlUp_p)
	: Command(commandHandle_p)
	, _player(player_p)
	, _type(type_p)
	, _lvlUp(lvlUp_p)
{}

void PlayerChoseDivinityCommand::registerCommand(Step & step_p, State const &state_p)
{
	Logger::getDebug() << "PlayerChoseDivinityCommand:: apply Command "<<_player <<std::endl;
	// check for available option
	if(checkOption(*state_p.getPlayer(_player), _type))
	{
		// check for lvl up legality
		if(_lvlUp && checkLvlUp(*state_p.getPlayer(_player)))
		{
			step_p.addSteppable(new PlayerLevelUpDivinityStep(_player, _type));
			step_p.addSteppable(new PlayerResetOptionDivinityStep(_player, state_p.getPlayer(_player)->_divOptions));
			/// @todo read from data how much
			step_p.addSteppable(new PlayerAnchorDivinityStep(_player, _type, 120.));
		}
	}
}

bool PlayerChoseDivinityCommand::applyCommand(Step &, State const &, CommandData const *) const
{
	return true;
}

} // namespace octopus

