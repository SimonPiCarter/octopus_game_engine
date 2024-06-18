#include "OptionManager.h"

#include "command/player/PlayerChoseOptionCommand.hh"

namespace godot {


/////////////////////////////
/// AbstractOptionManager ///
/////////////////////////////

AbstractOptionManager::AbstractOptionManager(unsigned long player_p) : _player(player_p) {}

octopus::Command * AbstractOptionManager::newCommandFromOption(int option_p)
{
	return new octopus::PlayerChoseOptionCommand(_player, _queuedKeys.front(), option_p);
}

std::string const &AbstractOptionManager::getKey() const
{
    return _queuedKeys.front();
}
void AbstractOptionManager::addOptionLayer(octopus::State const &state_p, octopus::PlayerAddOptionStep const *options_p)
{
	// skip if other player
	if(options_p->_player != _player)
	{
		return;
	}

	registerOption(state_p, options_p);
	_queuedKeys.push_back(options_p->_key);
}

void AbstractOptionManager::popOptionLayer(octopus::State const &state_p, octopus::PlayerPopOptionStep const *options_p)
{
	// skip if other player
	if(options_p->_player != _player)
	{
		return;
	}

	unregisterOption(state_p, options_p);
	_queuedKeys.pop_front();
}

/////////////////////////////
///       OptionManager   ///
/////////////////////////////

OptionManager::OptionManager(unsigned long player_p) : GenericOptionManager<BuffGenerator, ::Option>(player_p) {}

godot::Option *OptionManager::getPrimaryOption(uint32_t idx_p) const
{
    godot::Option * opt_l = memnew(Option());
    opt_l->set_option(_queuedOptions.at(idx_p)._playerOption);
	opt_l->set_rarity(_queuedOptions.at(idx_p)._rarity);
    return opt_l;
}

godot::Option *OptionManager::getSecondaryOption(uint32_t /*idx_p*/) const
{
    // godot::Option * opt_l = memnew(Option());
    // opt_l->set_option(_queuedOptions.at(idx_p)._enemyOption);
	// opt_l->set_rarity(_queuedOptions.at(idx_p)._rarity);
    return nullptr;
}

godot::Option *OptionManager::getChosenPrimaryOption(uint32_t idx_p) const
{
    godot::Option * opt_l = memnew(Option());
    opt_l->set_option(_chosenOptions.at(idx_p)._playerOption);
	opt_l->set_rarity(_chosenOptions.at(idx_p)._rarity);
    return opt_l;
}

godot::Option *OptionManager::getChosenSecondaryOption(uint32_t idx_p) const
{
    godot::Option * opt_l = memnew(Option());
    opt_l->set_option(_chosenOptions.at(idx_p)._enemyOption);
    return nullptr;
}

/////////////////////////////
/// DivinityOptionManager ///
/////////////////////////////

DivinityOptionManager::DivinityOptionManager(unsigned long player_p) : GenericOptionManager<DivinityGenerator, fas::DivinityType>(player_p) {}

godot::Option *DivinityOptionManager::getPrimaryOption(uint32_t idx_p) const
{
	if(idx_p >= _queuedOptions.size())
	{
		return nullptr;
	}
    godot::Option * opt_l = memnew(Option());
    opt_l->set_option(_player, _queuedOptions.at(idx_p));
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

}
