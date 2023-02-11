#ifndef __Selection__
#define __Selection__

#include <list>

namespace cuttlefish
{
class Sprite;
class Window;

/// @brief Handle selected sprites/entities
class Selection
{
public:
	Sprite *_sprite {nullptr};

	std::list<Sprite *> _sprites;

	void removeSprite(Sprite * sprite_p);

	void render(Window &window_p) const;
};

} // namespace cuttlefish


#endif
