
#include "PlayerChoseDivinityCommand.hh"

#include "logger/Logger.hh"
#include "step/Step.hh"
#include "state/State.hh"
#include "state/player/Player.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/player/PlayerAnchorDivinityStep.hh"
#include "step/player/PlayerLevelUpDivinityStep.hh"
#include "step/player/PlayerResetOptionDivinityStep.hh"

namespace octopus
{

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

DivinityOption const *getCurrentDivOption(Step const & step_p, Player const & player_p)
{
	unsigned long divSpent_l = step_p.getDivOptionSpent(player_p._id);
	if(player_p._divOptions.empty() || divSpent_l >= player_p._divOptions.size())
	{
		return nullptr;
	}
	// search for older options if some are spent
	auto &&rit_l = player_p._divOptions.rbegin();
	for(unsigned long i = 0 ; i < divSpent_l ; ++ i)
	{
		++rit_l;
	}
	return &(*rit_l);
}

PlayerChoseDivinityCommand::PlayerChoseDivinityCommand(unsigned long player_p, DivinityType type_p)
	: Command(0)
	, _player(player_p)
	, _type(type_p)
{}

void PlayerChoseDivinityCommand::registerCommand(Step & step_p, State const &state_p)
{
	Logger::getDebug() << "PlayerChoseDivinityCommand:: apply Command "<<_player <<std::endl;
	// check for available option
	Player const & player_l = *state_p.getPlayer(_player);

	step_p.addSteppable(new CommandStorageStep(this));

	DivinityOption const *current_l = getCurrentDivOption(step_p, player_l);
	if(current_l
	&& getDivOption(*current_l, _type))
	{
		// check for lvl up legality
		if(current_l->_lvlUp && checkLvlUp(player_l))
		{
			step_p.addSteppable(new PlayerLevelUpDivinityStep(_player, _type));
			step_p.addSteppable(new PlayerResetOptionDivinityStep(_player, *current_l));
			/// @todo read from data how much
			step_p.addSteppable(new PlayerAnchorDivinityStep(_player, _type,
				getDivAnchor(player_l, _type, false), getDivAnchor(player_l, _type) + 120.));
		}
		else if(!current_l->_lvlUp)
		{
			step_p.addSteppable(new PlayerResetOptionDivinityStep(_player, *current_l));
			/// @todo read from data how much
			step_p.addSteppable(new PlayerAnchorDivinityStep(_player, _type,
				getDivAnchor(player_l, _type, false), getDivAnchor(player_l, _type) + 120.));
		}
	}
}

bool PlayerChoseDivinityCommand::applyCommand(Step &, State const &, CommandData const *) const
{
	return true;
}

} // namespace octopus

