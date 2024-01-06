#include "OptionManager.h"

#include "step/player/PlayerAddOptionStep.hh"
#include "step/player/PlayerPopOptionStep.hh"
#include "command/player/PlayerChoseOptionCommand.hh"

namespace godot {

AbstractOptionManager::AbstractOptionManager(unsigned long player_p) : _player(player_p) {}

octopus::Command * AbstractOptionManager::newCommandFromOption(int option_p)
{
	return new octopus::PlayerChoseOptionCommand(_player, _queuedKeys.front(), option_p);
}

std::string const &AbstractOptionManager::getKey() const
{
    return _queuedKeys.front();
}
void AbstractOptionManager::addOptionLayer(octopus::PlayerAddOptionStep const *options_p)
{
	// skip if other player
	if(options_p->_player != _player)
	{
		return;
	}

	registerOption(options_p);
	_queuedKeys.push_back(options_p->_key);
}

void AbstractOptionManager::popOptionLayer(octopus::PlayerPopOptionStep const *options_p)
{
	// skip if other player
	if(options_p->_player != _player)
	{
		return;
	}

	unregisterOption(options_p);
	_queuedKeys.pop_front();
}

OptionManager::OptionManager(unsigned long player_p) : AbstractOptionManager(player_p) {}

void OptionManager::registerOption(octopus::PlayerAddOptionStep const *options_p)
{
	BuffGenerator const *gen_l = dynamic_cast<BuffGenerator const *>(options_p->_generator);
	_queuedOptions.push_back(gen_l->_options);
}

void OptionManager::unregisterOption(octopus::PlayerPopOptionStep const *options_p)
{
	// store chosen option to display them later
	_chosenOptions.push_back(_queuedOptions.front().at(options_p->_choice));
	// pop first queue
	_queuedOptions.pop_front();
}

godot::Option *OptionManager::getPrimaryOption(uint32_t idx_p) const
{
    godot::Option * opt_l = memnew(Option());
    opt_l->set_option(_queuedOptions.front().at(idx_p)._playerOption);
    return opt_l;
}

godot::Option *OptionManager::getSecondaryOption(uint32_t idx_p) const
{
    godot::Option * opt_l = memnew(Option());
    opt_l->set_option(_queuedOptions.front().at(idx_p)._enemyOption);
    return opt_l;
}

godot::Option *OptionManager::getChosenPrimaryOption(uint32_t idx_p) const
{
    godot::Option * opt_l = memnew(Option());
    opt_l->set_option(_chosenOptions.at(idx_p)._playerOption);
    return opt_l;
}

godot::Option *OptionManager::getChosenSecondaryOption(uint32_t idx_p) const
{
    godot::Option * opt_l = memnew(Option());
    opt_l->set_option(_chosenOptions.at(idx_p)._enemyOption);
    return opt_l;
}

uint32_t OptionManager::getQueuedOptionsSize() const
{
    return _queuedOptions.size();
}

uint32_t OptionManager::getChosenOptionsSize() const
{
    return _chosenOptions.size();
}

uint32_t OptionManager::getCurrentOptionSize() const
{
    return _queuedOptions.front().size();
}

DivinityOptionManager::DivinityOptionManager(unsigned long player_p) : AbstractOptionManager(player_p) {}

void DivinityOptionManager::registerOption(octopus::PlayerAddOptionStep const *options_p)
{
	DivinityGenerator const *gen_l = dynamic_cast<DivinityGenerator const *>(options_p->_generator);
	_queuedOptions.push_back(gen_l->_options);
}

void DivinityOptionManager::unregisterOption(octopus::PlayerPopOptionStep const *options_p)
{
	// store chosen option to display them later
	_chosenOptions.push_back(_queuedOptions.front().at(options_p->_choice));
	// pop first queue
	_queuedOptions.pop_front();
}

godot::Option *DivinityOptionManager::getPrimaryOption(uint32_t idx_p) const
{
    godot::Option * opt_l = memnew(Option());
    opt_l->set_option(_player, _queuedOptions.front().at(idx_p));
    return opt_l;
}

godot::Option *DivinityOptionManager::getSecondaryOption(uint32_t idx_p) const
{
    return nullptr;
}

godot::Option *DivinityOptionManager::getChosenPrimaryOption(uint32_t idx_p) const
{
    godot::Option * opt_l = memnew(Option());
    opt_l->set_option(_player, _chosenOptions.at(idx_p));
    return opt_l;
}

godot::Option *DivinityOptionManager::getChosenSecondaryOption(uint32_t idx_p) const
{
    return nullptr;
}

uint32_t DivinityOptionManager::getQueuedOptionsSize() const
{
    return _queuedOptions.size();
}

uint32_t DivinityOptionManager::getChosenOptionsSize() const
{
    return _chosenOptions.size();
}

uint32_t DivinityOptionManager::getCurrentOptionSize() const
{
    return _queuedOptions.front().size();
}

}
