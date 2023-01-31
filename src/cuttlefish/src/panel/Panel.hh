#ifndef __Panel__
#define __Panel__

#include <list>
#include <map>
#include "sprite/Sprite.hh"

namespace octopus
{
	class Entity;
	class State;
	class UnitModel;
	class BuildingModel;
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
	Panel(int x, int y, Texture const * background_p, Texture const *icons_p, int iconsPerLine_p);
	~Panel();

	/// @brief refresh Panel if necessary
	void refresh(octopus::Entity const *selected_p, octopus::State const &state_p);

	/// @brief display panel
	void render(Window &window_p) const;

	/// @brief add a sprite info for a given model
	void addSpriteInfo(std::string const &model_p, int state_p, int frame_p);

	/// @brief return a sprite model combination if clicked
	/// @note return nullptr if no clic
	SpriteModel const * getSpriteModel(Window &window_p, int x, int y) const;

	Sprite const * getBackground() const { return _background; }
protected:
	/// @brief last selection
	octopus::Entity const * _lastSelection {nullptr};
	/// @brief sprites
	std::list<SpriteModel> _sprites;

	Sprite * _background;

	/// @brief map of sprite info (state/frame)
	/// for models
	std::map<std::string, SpriteInfo> _mapIcons;

	Texture const * const _icons;

	int _x;
	int _y;
	int const _iconsPerLine;
};
} // namespace octopus


#endif
