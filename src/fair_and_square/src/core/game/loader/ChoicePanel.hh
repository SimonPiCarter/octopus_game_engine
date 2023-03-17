#ifndef __ChoicePanel__
#define __ChoicePanel__

#include "panel/OptionPanel.hh"
#include "library/model/bonus/BuffGenerators.hh"

namespace fas
{

/// @brief this class represent a Divinity Panel
/// displaying the options for divinity
class ChoicePanel : public cuttlefish::OptionPanel
{
public:
	ChoicePanel(int x, int y, cuttlefish::Texture const * background_p, cuttlefish::Texture const *icons_p, unsigned long player_p);
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

    /// @brief returna a new command to be commited to the controller based on the option
    /// @param option_p is supposed to be valid (>= 0)
    virtual octopus::Command * newCommandFromOption(int option_p) override;

protected:
	std::vector<BuffOption> _options;

	std::vector<cuttlefish::Picture *> _optionsBackground;

};
} // namespace octopus


#endif
