#include "OptionManager.h"

#include "step/player/PlayerAddOptionStep.hh"
#include "step/player/PlayerPopOptionStep.hh"
#include "command/player/PlayerChoseOptionCommand.hh"

namespace godot {

OptionManager::OptionManager(unsigned long player_p) : _player(player_p) {}

void OptionManager::addOptionLayer(octopus::PlayerAddOptionStep const *options_p)
{
	// skip if other player
	if(options_p->_player != _player)
	{
		return;
	}

	BuffGenerator const *gen_l = dynamic_cast<BuffGenerator const *>(options_p->_generator);
	_queuedOptions.push_back(gen_l->_options);
	_queuedKeys.push_back(options_p->_key);
}

void OptionManager::popOptionLayer(octopus::PlayerPopOptionStep const *options_p)
{
	// skip if other player
	if(options_p->_player != _player)
	{
		return;
	}

	// store chosen option to display them later
	_chosenOptions.push_back(_queuedOptions.front().at(options_p->_choice));

	// pop first queue
	_queuedOptions.pop_front();
	_queuedKeys.pop_front();
}

octopus::Command * OptionManager::newCommandFromOption(int option_p)
{
	return new octopus::PlayerChoseOptionCommand(_player, _queuedKeys.front(), option_p);
}

::Option const &OptionManager::getOption(size_t idx_p) const
{
    return _queuedOptions.front().at(idx_p);
}

std::string const &OptionManager::getKey() const
{
    return _queuedKeys.front();
}

::Option const &OptionManager::getChosenOption(size_t idx_p) const
{
    return _chosenOptions.at(idx_p);
}

size_t OptionManager::getQueuedOptionsSize() const
{
    return _queuedOptions.size();
}

size_t OptionManager::getChosenOptionsSize() const
{
    return _chosenOptions.size();
}

size_t OptionManager::getCurrentOptionSize() const
{
    return _queuedOptions.front().size();
}


}
