#ifndef __Godoctopus_OptionManager__
#define __Godoctopus_OptionManager__

#include "Option.h"
#include <cstdint>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include "state/State.hh"
#include "state/player/Player.hh"
#include "step/player/PlayerAddOptionStep.hh"
#include "step/player/PlayerPopOptionStep.hh"

#include "library/model/bonus/BuffGenerators.hh"
#include "library/model/divinity/DivinityGenerator.hh"

namespace octopus {
    class Command;
}

namespace godot {

class AbstractOptionManager
{
public:
    AbstractOptionManager(unsigned long player_p);

	/// @brief method to add option from a PlayerAddOptionStep
	void addOptionLayer(octopus::State const &state_p, octopus::PlayerAddOptionStep const *);

	/// @brief method to pop option from a PlayerPopOptionStep
	void popOptionLayer(octopus::State const &state_p, octopus::PlayerPopOptionStep const *);

    /// @brief return a a new command to be commited to the controller based on the option
    /// @param option_p is supposed to be valid (>= 0)
    octopus::Command * newCommandFromOption(int option_p);
    /// @brief accessor for front queued key
    std::string const &getKey() const;

    // To override

	/// @brief method to add option from a PlayerAddOptionStep
    virtual void registerOption(octopus::State const &state_p, octopus::PlayerAddOptionStep const *) = 0;
	/// @brief method to pop option from a PlayerPopOptionStep
    virtual void unregisterOption(octopus::State const &state_p, octopus::PlayerPopOptionStep const *) = 0;

    /// @brief accessor for front queued options
    virtual godot::Option *getPrimaryOption(uint32_t idx_p) const = 0;
    virtual godot::Option *getSecondaryOption(uint32_t idx_p) const = 0;

    /// @brief accessor for already chosen options
    virtual godot::Option *getChosenPrimaryOption(uint32_t idx_p) const = 0;
    virtual godot::Option *getChosenSecondaryOption(uint32_t idx_p) const = 0;

    /// @brief getter of chosen options size
    virtual uint32_t getChosenOptionsSize() const = 0;

protected:
	std::list<std::string> _queuedKeys;

    unsigned long _player;
};

template<typename gen_t, typename option_t>
std::vector<option_t> get_front_options(octopus::State const &state_p, unsigned long player_p)
{
	auto &&options_l = state_p.getPlayer(player_p)->_options;
	if(options_l.empty())
	{
		return {};
	}
	gen_t const *gen_l = dynamic_cast<gen_t const *>(options_l.front());
	if(!gen_l)
	{
		return {};
	}
	return gen_l->_options;
}

template<typename gen_t, typename option_t>
class GenericOptionManager : public AbstractOptionManager
{
public:
    GenericOptionManager(unsigned long player_p) : AbstractOptionManager(player_p) {}

	/// @brief method to add option from a PlayerAddOptionStep
    virtual void registerOption(octopus::State const &state_p, octopus::PlayerAddOptionStep const *options_p)
	{
		gen_t const *gen_l = dynamic_cast<gen_t const *>(options_p->_generator);
		_queuedOptions = get_front_options<gen_t, option_t>(state_p, _player);
	}

	/// @brief method to pop option from a PlayerPopOptionStep
    virtual void unregisterOption(octopus::State const &state_p, octopus::PlayerPopOptionStep const *options_p)
	{
		// store chosen option to display them later
		_chosenOptions.push_back(_queuedOptions.at(options_p->_choice));
		// refresh queued options
		_queuedOptions = get_front_options<gen_t, option_t>(state_p, _player);
	}

    /// @brief getter of chosen options size
    virtual uint32_t getChosenOptionsSize() const
	{
		return _chosenOptions.size();
	}

protected:
	/// @brief list of all options queued
	std::vector<option_t> _queuedOptions;
	/// @brief list of all chosen options
	std::vector<option_t> _chosenOptions;
};

/// @brief this class is supposed to handle options
class OptionManager : public GenericOptionManager<BuffGenerator, ::Option>
{
public:
    OptionManager(unsigned long player_p);
    /// @brief accessor for front queued options
    virtual godot::Option *getPrimaryOption(uint32_t idx_p) const override;
    virtual godot::Option *getSecondaryOption(uint32_t idx_p) const override;

    /// @brief accessor for already chosen options
    virtual godot::Option *getChosenPrimaryOption(uint32_t idx_p) const override;
    virtual godot::Option *getChosenSecondaryOption(uint32_t idx_p) const override;
};

/// @brief this class is supposed to handle options for divinity
class DivinityOptionManager : public GenericOptionManager<DivinityGenerator, fas::DivinityType>
{
public:
    DivinityOptionManager(unsigned long player_p);
    /// @brief accessor for front queued options
    virtual godot::Option *getPrimaryOption(uint32_t idx_p) const override;
    virtual godot::Option *getSecondaryOption(uint32_t idx_p) const override;

    /// @brief accessor for already chosen options
    virtual godot::Option *getChosenPrimaryOption(uint32_t idx_p) const override;
    virtual godot::Option *getChosenSecondaryOption(uint32_t idx_p) const override;
};

}

#endif //__Godoctopus_OptionManager__
