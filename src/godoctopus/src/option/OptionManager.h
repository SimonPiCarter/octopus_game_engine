#ifndef __Godoctopus_OptionManager__
#define __Godoctopus_OptionManager__

#include <thread>
#include <cstdint>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include "library/model/bonus/BuffGenerators.hh"

namespace octopus {
    class Command;
    class PlayerAddOptionStep;
    class PlayerPopOptionStep;
}

namespace godot {

/// @brief this class is supposed to handle options
class OptionManager
{

public:
    OptionManager(unsigned long player_p);

	/// @brief method to add option from a PlayerAddOptionStep
	virtual void addOptionLayer(octopus::PlayerAddOptionStep const *);

	/// @brief method to pop option from a PlayerPopOptionStep
	virtual void popOptionLayer(octopus::PlayerPopOptionStep const *);

    /// @brief returna a new command to be commited to the controller based on the option
    /// @param option_p is supposed to be valid (>= 0)
    virtual octopus::Command * newCommandFromOption(int option_p);

    /// @brief accessor for front queued options
    ::Option const &getOption(size_t idx_p) const;
    /// @brief accessor for front queued key
    std::string const &getKey() const;

    /// @brief accessor for already chosen options
    ::Option const &getChosenOption(size_t idx_p) const;

    /// @brief getter of queued options size
    size_t getQueuedOptionsSize() const;

    /// @brief getter of chosen options size
    size_t getChosenOptionsSize() const;

    /// @brief getter for the current option size (number of options availables)
    size_t getCurrentOptionSize() const;
private:
	/// @brief list of all options queued
	std::list<std::vector<::Option>> _queuedOptions;
	std::list<std::string> _queuedKeys;
	/// @brief list of all chosen options
	std::vector<::Option> _chosenOptions;

    unsigned long const _player;
};

}

#endif //__Godoctopus_OptionManager__
