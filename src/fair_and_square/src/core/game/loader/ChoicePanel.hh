#ifndef __ChoicePanel__
#define __ChoicePanel__

#include "library/model/bonus/BuffGenerators.hh"
#include "panel/OptionPanel.hh"
#include "text/SegmentedText.hh"
#include "choice/ChoiceSubPanel.hh"

namespace fas
{

/// @brief this class represent a Divinity Panel
/// displaying the options for divinity
class ChoicePanel : public cuttlefish::OptionPanel
{
public:
	ChoicePanel(cuttlefish::Window &window_p, int x, int y, cuttlefish::Texture const * background_p, cuttlefish::Texture const * subBackground_p, cuttlefish::Texture const *icons_p, cuttlefish::Texture const *statsIcons_p, unsigned long player_p);
	~ChoicePanel();

	/// @brief refresh Panel if necessary
	virtual void refresh() override;

	/// @brief display panel
	virtual void render(cuttlefish::Window &window_p) override;

	/// @brief return the option clicked
	/// @note return -1 if no option was picked
	virtual int getOption(cuttlefish::Window &window_p, int x, int y) const override;

	/// @brief return true if the panel as active options to be chosen
	virtual bool isActive() const  override;

	/// @brief method to add option from a PlayerAddOptionStep
	virtual void addOptionLayer(octopus::PlayerAddOptionStep const *) override;

	/// @brief method to pop option from a PlayerPopOptionStep
	virtual void popOptionLayer(octopus::PlayerPopOptionStep const *) override;

    /// @brief returna a new command to be commited to the controller based on the option
    /// @param option_p is supposed to be valid (>= 0)
    virtual octopus::Command * newCommandFromOption(int option_p) override;

	/// @brief refresh from mouse (to highlight hovered sub panel)
	virtual void refreshFromMouse(cuttlefish::Window &window_p, int x, int y) override;

	/// @brief load chosen option for display
	virtual void loadChosenOption() override;
	/// @brief unload chosen option for display
	virtual void unloadChosenOption() override;

protected:
	cuttlefish::Window &_window;
	unsigned long const _player;

	/// @brief background for the options
	std::vector<cuttlefish::Picture *> _optionsBackground;
	std::vector<ChoiceSubPanel *> _optionsSubPanel;

	/// @brief temporary texts for option
	std::vector<cuttlefish::SegmentedText *> _optionsTexts;

	/// @brief list of all options queued
	std::list<std::vector<Option>> _queuedOptions;
	std::list<std::string> _queuedKeys;
	/// @brief list of all chosen options
	std::vector<Option> _chosenOptions;

	/// @brief background for sub panels
	cuttlefish::Picture _subBackground;
	/// @brief icons for sub panels
	cuttlefish::Texture const * const _icons;
	/// @brief stat icons for sub panels
	cuttlefish::Texture const * const _statsIcons;

	/// @brief update current option with the first option queued
	void updateCurrent();

	/// @brief option list of the current choice
	std::vector<Option> _options;
	/// @brief key of the current choice
	std::string _key;
	/// @brief true if the panel is in chosen mode
	bool _chosenMode { false };

};
} // namespace octopus


#endif
