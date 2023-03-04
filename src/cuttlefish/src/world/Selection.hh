#ifndef __Selection__
#define __Selection__

#include <list>
#include <set>
#include <map>
#include <string>

namespace octopus
{
class State;
} // namespace octopus


namespace cuttlefish
{
class SpriteEntity;
class Window;

/// @brief safe comparator comparing handles
struct SpriteComparator
{
	bool operator()(SpriteEntity const *a, SpriteEntity const *b) const;
};

/// @brief key to fast compare selections
struct SelectionKey
{
	std::map<std::string, unsigned long> _spritesPerModel;

	bool operator==(SelectionKey const &other_p) const
	{
		return _spritesPerModel == other_p._spritesPerModel;
	}
};

/// @brief Handle selected sprites/entities
class Selection
{
public:
	/// @brief main unit selection
	SpriteEntity *_sprite {nullptr};

	std::set<SpriteEntity *, SpriteComparator> _sprites;

	/// @brief contains sprites for models selected
	std::map<std::string, std::set<SpriteEntity *, SpriteComparator> > _spritesPerModel;

	void removeSprite(SpriteEntity * sprite_p, octopus::State const &state_p);

	void render(Window &window_p) const;

	/// @brief clear the full selection
	void clear();

	/// @brief create a key
	SelectionKey key() const;
};

/// @brief add the list of spriteentity to the current selection
/// @note remove duplicated if necessary @todo
void addToSelection(Selection &selection_p, std::list<SpriteEntity *> const &sprites_p, octopus::State const &state_p);

void addToSelection(Selection &selection_p, Selection &other_p);

/// @brief replace the selection with the list of spriteentity
/// @note update main selected if necessary
/// @note remove duplicated if any @todo
void replaceSelection(Selection &selection_p, std::list<SpriteEntity *> const &sprites_p, octopus::State const &state_p);

} // namespace cuttlefish


#endif
