#ifndef __Panel__
#define __Panel__

#include <list>
#include <map>
#include "sprite/Sprite.hh"
#include "text/SegmentedText.hh"

namespace octopus
{
	class Entity;
	class State;
	struct UnitModel;;
	struct BuildingModel;;
} // namespace octopus


namespace cuttlefish
{
class Texture;
class Window;

struct SpriteInfo
{
	int state {0};
	int frame {0};
};

struct SpriteModel
{
	Sprite * sprite {nullptr};
	octopus::UnitModel const * unitModel {nullptr};
	octopus::BuildingModel const * buildingModel {nullptr};
};

/// @brief this class represent a Panel
/// displaying the options for the building
/// or unit
/// Display a background and an image for each model
class Panel
{
public:
	Panel(Window* window_p, int x, int y, Texture const * background_p, Texture const *icons_p, int iconsPerLine_p);
	~Panel();

	/// @brief refresh Panel if necessary
	void refresh(Sprite const *selected_p, octopus::State const &state_p);

	/// @brief display panel
	void render(Window &window_p);

	/// @brief add a sprite info for a given model
	void addSpriteInfo(std::string const &model_p, int state_p, int frame_p);

	/// @brief return a sprite model combination if clicked
	/// @note return nullptr if no clic
	SpriteModel const * getSpriteModel(Window &window_p, int x, int y) const;

	/// @brief return a sprite model combination for grid position
	/// @note return nullptr if no match
	SpriteModel const * getSpriteModelOnGrid(int x, int y) const;

	Sprite const * getBackground() const { return _background; }
protected:
	/// @brief last selection
	octopus::Entity const * _lastSelection {nullptr};
	/// @brief sprites
	std::list<SpriteModel> _sprites;
	/// @brief sprite models used for grid coordinate access
	std::map<std::pair<int, int>, SpriteModel *> _grid;

	Sprite * _background;

	/// @brief map of sprite info (state/frame)
	/// for models
	std::map<std::string, SpriteInfo> _mapIcons;

	Texture const * const _icons;

	int _x;
	int _y;
	int const _iconsPerLine;

	SegmentedText _textStats;
};
} // namespace octopus


#endif
