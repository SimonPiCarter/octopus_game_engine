#ifndef __DivinityPanel__
#define __DivinityPanel__

#include <list>
#include <map>
#include <set>

#include "sprite/Sprite.hh"

#include "state/DivinityType.hh"

namespace octopus
{
	class Player;
	class State;
} // namespace octopus


namespace cuttlefish
{
class Texture;
class Window;

struct OptionInfo
{
	int state {0};
	int frame {0};
};

/// @brief this class represent a Divinity Panel
/// displaying the options for divinity
class DivinityPanel
{
public:
	DivinityPanel(Window* window_p, int x, int y, Texture const * background_p, Texture const *icons_p, unsigned long player_p);
	~DivinityPanel();

	/// @brief refresh Panel if necessary
	void refresh();

	/// @brief display panel
	void render(Window &window_p);

	/// @brief add a sprite info for a given divinity type
	void addOptionInfo(octopus::DivinityType type_p, int state_p, int frame_p);

	/// @brief return a sprite model combination if clicked
	/// @note return a boolean true is an option is picked as first element of the pair
	std::pair<bool, octopus::DivinityType> getOption(Window &window_p, int x, int y) const;

	/// @brief return a sprite model combination for grid position
	/// @note return a boolean false if no match as firt element of the pair
	std::pair<bool, octopus::DivinityType> getOptionOnGrid(int x, int y) const;

	Picture const * getBackground() const { return _background; }

	bool isActive() const { return !_options.empty(); }


	void addOptionLayer(unsigned long player_p, std::set<octopus::DivinityType> const &options_p);
	void popOptionLayer();
protected:
	/// @brief sprites and options used
	std::list<std::pair<Picture *, octopus::DivinityType> > _options;
	/// @brief sprite models used for grid coordinate access
	std::map<std::pair<int, int>, octopus::DivinityType> _grid;

	Picture * _background;

	/// @brief map of sprite info (state/frame)
	/// for models
	std::map<octopus::DivinityType, OptionInfo> _mapIcons;


	int const _x;
	int const _y;
	unsigned long const _player;

	Texture const * const _icons;

	std::list<std::set<octopus::DivinityType> > _optionLayers;

	/// @brief clean all options
	void cleanOptions();
};
} // namespace octopus


#endif
