#ifndef __DivinityPanel__
#define __DivinityPanel__

#include <list>
#include <map>

#include "sprite/Sprite.hh"

#include "OptionPanel.hh"

namespace cuttlefish
{

struct OptionInfo
{
	int state {0};
	int frame {0};
};

/// @brief this class represent a Divinity Panel
/// displaying the options for divinity
class DivinityPanel : public OptionPanel
{
public:
	DivinityPanel(int x, int y, Texture const * background_p, Texture const *icons_p, unsigned long player_p);
	virtual ~DivinityPanel();

	/// @brief refresh Panel if necessary
	virtual void refresh() override;

	/// @brief display panel
	virtual void render(Window &window_p) override;

	virtual bool isActive() const override { return !_options.empty(); }

	virtual void addOptionDivinityLayer(unsigned long player_p, std::set<octopus::DivinityType> const &options_p) override;

    virtual octopus::Command * newCommandFromOption(int option_p) override;

	/// @brief load chosen option for display
	virtual void loadChosenOption() {}
	/// @brief unload chosen option for display
	virtual void unloadChosenOption() {}

	virtual int getOption(Window &window_p, int x, int y) const;

	/// @brief add a sprite info for a given divinity type
	void addOptionInfo(octopus::DivinityType type_p, int state_p, int frame_p);

protected:
	/// @brief sprites and options used
	std::list<std::pair<Picture *, octopus::DivinityType> > _options;
	/// @brief sprite models used for grid coordinate access
	std::map<std::pair<int, int>, octopus::DivinityType> _grid;

	/// @brief map of sprite info (state/frame)
	/// for models
	std::map<octopus::DivinityType, OptionInfo> _mapIcons;


	unsigned long const _player;

	Texture const * const _icons;

	std::list<std::set<octopus::DivinityType> > _optionLayers;

	/// @brief clean all options
	void cleanOptions();
};
} // namespace octopus


#endif
