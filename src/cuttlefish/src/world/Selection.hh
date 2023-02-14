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

	/// @brief clear the full selection
	void clear();
};

/// @brief add the list of sprite to the current selection
/// @note remove duplicated if necessary
void addToSelection(Selection &selection_p, std::list<Sprite *> const &sprites_p);

/// @brief replace the selection with the list of sprite
/// @note update main selected if necessary
/// @note remove duplicated if any
void replaceSelection(Selection &selection_p, std::list<Sprite *> const &sprites_p);

} // namespace cuttlefish


#endif
