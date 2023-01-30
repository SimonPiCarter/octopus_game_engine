#ifndef __Panel__
#define __Panel__

#include <list>
#include <map>
#include "sprite/Sprite.hh"

namespace octopus
{
	class Entity;
	class State;
} // namespace octopus


namespace cuttlefish
{
class Texture;
class Window;

struct SpriteInfo
{
	int state;
	int frame;
};

/// @brief this class represent a Panel
/// displaying the options for the building
/// or unit
/// Display a background and an image for each model
class Panel
{
public:
	Panel(int x, int y, Texture const * background_p, Texture const *icons_p, int iconsPerLine_p);

	/// @brief refresh Panel if necessary
	void refresh(octopus::Entity const *selected_p, octopus::State const &state_p);

	/// @brief display panel
	void render(Window &window_p) const;

	/// @brief add a sprite info for a given model
	void addSpriteInfo(std::string const &model_p, int state_p, int frame_p);
protected:
	/// @brief last selection
	octopus::Entity const * _lastSelection {nullptr};
	/// @brief sprites
	std::list<Sprite *> _sprites;

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
