#ifndef __OptionPanel__
#define __OptionPanel__

#include <set>

#include "sprite/Picture.hh"

#include "state/DivinityType.hh"


namespace octopus
{
	class Player;
	class PlayerAddOptionStep;
	class State;
    class Command;
} // namespace octopus


namespace cuttlefish
{
class Texture;
class Window;

/// @brief this class represent an option Panel
class OptionPanel
{
public:
	OptionPanel(int x, int y, Texture const * background_p);
	virtual ~OptionPanel();

	Picture const * getBackground() const;

	/// @brief refresh Panel if necessary
	virtual void refresh() = 0;

	/// @brief display panel
	virtual void render(Window &window_p) = 0;

	/// @brief return the option clicked
	/// @note return -1 if no option was picked
	virtual int getOption(Window &window_p, int x, int y) const = 0;

	/// @brief return true if the panel as active options to be chosen
	virtual bool isActive() const  = 0;

	/// @brief method to add options from divintiy options (should be deprecated)
	virtual void addOptionDivinityLayer(unsigned long player_p, std::set<octopus::DivinityType> const &options_p) {}

	/// @brief method to add option from a PlayerAddOptionStep
	virtual void addOptionLayer(octopus::PlayerAddOptionStep const *) {}

    /// @brief return a a new command to be commited to the controller based on the option
    /// @param option_p is supposed to be valid (>= 0)
	/// @return can return nullptr if no valide command from given option or panel state
    virtual octopus::Command * newCommandFromOption(int option_p) = 0;

	/// @brief load chosen option for display
	virtual void loadChosenOption() = 0;
	/// @brief unload chosen option for display
	virtual void unloadChosenOption() = 0;

    /// @brief refresh from mouse (to highlight hovered sub panel)
	virtual void refreshFromMouse(cuttlefish::Window &window_p, int x, int y) {}

protected:
	Picture * _background;

	int const _x;
	int const _y;
};
} // namespace octopus


#endif
