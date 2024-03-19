#include "OptionManager.h"

#include "state/State.hh"
#include "state/player/Player.hh"
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

OptionManager::OptionManager(unsigned long player_p) : AbstractOptionManager(player_p) {}

std::vector<::Option> get_front_buff_options(octopus::State const &state_p, unsigned long player_p)
{
	auto &&options_l = state_p.getPlayer(player_p)->_options;
	if(options_l.empty())
	{
		return {};
	}
	BuffGenerator const *gen_l = dynamic_cast<BuffGenerator const *>(options_l.front());
	if(!gen_l)
	{
		return {};
	}
	return gen_l->_options;
}

void OptionManager::registerOption(octopus::State const &state_p, octopus::PlayerAddOptionStep const *options_p)
{
	BuffGenerator const *gen_l = dynamic_cast<BuffGenerator const *>(options_p->_generator);
	_queuedOptions = get_front_buff_options(state_p, _player);
}

void OptionManager::unregisterOption(octopus::State const &state_p, octopus::PlayerPopOptionStep const *options_p)
{
	// store chosen option to display them later
	_chosenOptions.push_back(_queuedOptions.at(options_p->_choice));
	// refresh queued options
	_queuedOptions = get_front_buff_options(state_p, _player);
}

godot::Option *OptionManager::getPrimaryOption(uint32_t idx_p) const
{
    godot::Option * opt_l = memnew(Option());
    opt_l->set_option(_queuedOptions.at(idx_p)._playerOption);
    return opt_l;
}

godot::Option *OptionManager::getSecondaryOption(uint32_t idx_p) const
{
    godot::Option * opt_l = memnew(Option());
    opt_l->set_option(_queuedOptions.at(idx_p)._enemyOption);
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

uint32_t OptionManager::getChosenOptionsSize() const
{
    return _chosenOptions.size();
}

DivinityOptionManager::DivinityOptionManager(unsigned long player_p) : AbstractOptionManager(player_p) {}

std::vector<fas::DivinityType> get_front_divinity_options(octopus::State const &state_p, unsigned long player_p)
{
	auto &&options_l = state_p.getPlayer(player_p)->_options;
	if(options_l.empty())
	{
		return {};
	}
	DivinityGenerator const *gen_l = dynamic_cast<DivinityGenerator const *>(options_l.front());
	if(!gen_l)
	{
		return {};
	}
	return gen_l->_options;
}

void DivinityOptionManager::registerOption(octopus::State const &state_p, octopus::PlayerAddOptionStep const *options_p)
{
	DivinityGenerator const *gen_l = dynamic_cast<DivinityGenerator const *>(options_p->_generator);
	_queuedOptions = get_front_divinity_options(state_p, _player);
}

void DivinityOptionManager::unregisterOption(octopus::State const &state_p, octopus::PlayerPopOptionStep const *options_p)
{
	// store chosen option to display them later
	_chosenOptions.push_back(_queuedOptions.at(options_p->_choice));
	// refresh queued options
	_queuedOptions = get_front_divinity_options(state_p, _player);
}

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

uint32_t DivinityOptionManager::getChosenOptionsSize() const
{
    return _chosenOptions.size();
}

}
