#ifndef __Godoctopus_OptionManager__
#define __Godoctopus_OptionManager__

#include "Option.h"
#include <cstdint>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include "library/model/bonus/BuffGenerators.hh"
#include "library/model/divinity/DivinityGenerator.hh"

namespace octopus {
    class Command;
    class PlayerAddOptionStep;
    class PlayerPopOptionStep;
}

namespace godot {

class AbstractOptionManager
{
public:
    AbstractOptionManager(unsigned long player_p);

	/// @brief method to add option from a PlayerAddOptionStep
	void addOptionLayer(octopus::State const &state_p, octopus::PlayerAddOptionStep const *);

	/// @brief method to pop option from a PlayerPopOptionStep
	void popOptionLayer(octopus::PlayerPopOptionStep const *);

    /// @brief return a a new command to be commited to the controller based on the option
    /// @param option_p is supposed to be valid (>= 0)
    octopus::Command * newCommandFromOption(int option_p);
    /// @brief accessor for front queued key
    std::string const &getKey() const;

    // To override

	/// @brief method to add option from a PlayerAddOptionStep
    virtual void registerOption(octopus::State const &state_p, octopus::PlayerAddOptionStep const *) = 0;
	/// @brief method to pop option from a PlayerPopOptionStep
    virtual void unregisterOption(octopus::PlayerPopOptionStep const *) = 0;

    /// @brief accessor for front queued options
    virtual godot::Option *getPrimaryOption(uint32_t idx_p) const = 0;
    virtual godot::Option *getSecondaryOption(uint32_t idx_p) const = 0;

    /// @brief accessor for already chosen options
    virtual godot::Option *getChosenPrimaryOption(uint32_t idx_p) const = 0;
    virtual godot::Option *getChosenSecondaryOption(uint32_t idx_p) const = 0;

    /// @brief getter of queued options size
    virtual uint32_t getQueuedOptionsSize() const = 0;

    /// @brief getter of chosen options size
    virtual uint32_t getChosenOptionsSize() const = 0;

    /// @brief getter for the current option size (number of options availables)
    virtual uint32_t getCurrentOptionSize() const = 0;

protected:
	std::list<std::string> _queuedKeys;

    unsigned long _player;
};

/// @brief this class is supposed to handle options
class OptionManager : public AbstractOptionManager
{
public:
    OptionManager(unsigned long player_p);

	/// @brief method to add option from a PlayerAddOptionStep
	virtual void registerOption(octopus::State const &state_p, octopus::PlayerAddOptionStep const *) override;

	/// @brief method to pop option from a PlayerPopOptionStep
	virtual void unregisterOption(octopus::PlayerPopOptionStep const *) override;

    /// @brief accessor for front queued options
    virtual godot::Option *getPrimaryOption(uint32_t idx_p) const override;
    virtual godot::Option *getSecondaryOption(uint32_t idx_p) const override;

    /// @brief accessor for already chosen options
    virtual godot::Option *getChosenPrimaryOption(uint32_t idx_p) const override;
    virtual godot::Option *getChosenSecondaryOption(uint32_t idx_p) const override;

    /// @brief getter of queued options size
    virtual uint32_t getQueuedOptionsSize() const override;

    /// @brief getter of chosen options size
    virtual uint32_t getChosenOptionsSize() const override;

    /// @brief getter for the current option size (number of options availables)
    virtual uint32_t getCurrentOptionSize() const override;
private:
	/// @brief list of all options queued
	std::list<std::vector<::Option>> _queuedOptions;
	/// @brief list of all chosen options
	std::vector<::Option> _chosenOptions;
};

/// @brief this class is supposed to handle options for divinity
class DivinityOptionManager : public AbstractOptionManager
{
public:
    DivinityOptionManager(unsigned long player_p);

	/// @brief method to add option from a PlayerAddOptionStep
	virtual void registerOption(octopus::State const &state_p, octopus::PlayerAddOptionStep const *) override;

	/// @brief method to pop option from a PlayerPopOptionStep
	virtual void unregisterOption(octopus::PlayerPopOptionStep const *) override;

    /// @brief accessor for front queued options
    virtual godot::Option *getPrimaryOption(uint32_t idx_p) const override;
    virtual godot::Option *getSecondaryOption(uint32_t idx_p) const override;

    /// @brief accessor for already chosen options
    virtual godot::Option *getChosenPrimaryOption(uint32_t idx_p) const override;
    virtual godot::Option *getChosenSecondaryOption(uint32_t idx_p) const override;

    /// @brief getter of queued options size
    virtual uint32_t getQueuedOptionsSize() const override;

    /// @brief getter of chosen options size
    virtual uint32_t getChosenOptionsSize() const override;

    /// @brief getter for the current option size (number of options availables)
    virtual uint32_t getCurrentOptionSize() const override;
private:
	/// @brief list of all options queued
	std::list<std::vector<fas::DivinityType>> _queuedOptions;
	/// @brief list of all chosen options
	std::vector<fas::DivinityType> _chosenOptions;
};

}

#endif //__Godoctopus_OptionManager__
