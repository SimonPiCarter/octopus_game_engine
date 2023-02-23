#ifndef __Selection__
#define __Selection__

#include <list>
#include <unordered_map>

namespace octopus
{
class State;
} // namespace octopus


namespace cuttlefish
{
class Sprite;
class Window;

/// @brief Handle selected sprites/entities
class Selection
{
public:
	/// @brief main unit selection
	Sprite *_sprite {nullptr};

	std::list<Sprite *> _sprites;

	/// @brief contains sprites for models selected
	std::unordered_map<std::string, std::list<Sprite *> > _spritesPerModel;

	void removeSprite(Sprite * sprite_p, octopus::State const &state_p);

	void render(Window &window_p) const;

	/// @brief clear the full selection
	void clear();
};

/// @brief add the list of sprite to the current selection
/// @note remove duplicated if necessary @todo
void addToSelection(Selection &selection_p, std::list<Sprite *> const &sprites_p, octopus::State const &state_p);

/// @brief replace the selection with the list of sprite
/// @note update main selected if necessary
/// @note remove duplicated if any @todo
void replaceSelection(Selection &selection_p, std::list<Sprite *> const &sprites_p, octopus::State const &state_p);

} // namespace cuttlefish


#endif
