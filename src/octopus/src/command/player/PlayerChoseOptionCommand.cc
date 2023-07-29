
#include "PlayerChoseOptionCommand.hh"

#include "logger/Logger.hh"
#include "step/Step.hh"
#include "state/State.hh"
#include "state/player/Player.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/player/PlayerPopOptionStep.hh"

namespace octopus
{

PlayerChoseOptionCommand::PlayerChoseOptionCommand(unsigned long player_p, std::string const &key_p, unsigned long option_p)
	: Command(Handle(0))
	, _player(player_p)
	, _key(key_p)
	, _option(option_p)
{}

void PlayerChoseOptionCommand::registerCommand(Step & step_p, State const &state_p)
{
	Logger::getDebug() << "PlayerChoseOptionCommand:: apply Command "<<_player <<std::endl;
	// check for available option
	Player const & player_l = *state_p.getPlayer(_player);

	step_p.addSteppable(new CommandStorageStep(this));

	if(player_l._options.find(_key) != player_l._options.end()
	&& player_l._options.at(_key)->getNumOptions() > _option)
	{
		step_p.addSteppable(new PlayerPopOptionStep(_player, _key, _option));
	}
}

bool PlayerChoseOptionCommand::applyCommand(Step &, State const &, CommandData const *, PathManager &) const
{
	return true;
}

} // namespace octopus

