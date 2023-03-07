#ifndef __World__
#define __World__

#include <vector>
#include <list>
#include <unordered_map>

#include "Selection.hh"

namespace octopus
{
	struct StateAndSteps;
	class State;
	class Step;
}

namespace cuttlefish
{
class DivinityPanel;
class Panel;
class Picture;
class SpriteEntity;
class SpriteLibrary;
class StatsPanel;
class Window;

/// @brief This class is used to maintain a representation of the world
/// based on state the engine sends
/// Each update checks for new Entity and remove non alive entities
class World
{
public:
	/// @brief Create a world and set up the player playing on it
	/// @param player_p player idx
	World(unsigned long player_p) : _player(player_p) {}

	/// @brief update all sprites from the steps given
	/// only works if all previous steps have been handled this way
	void handleStep(Window &window_p, Panel &panel_p, StatsPanel &statsPanel_p, DivinityPanel &divPanel_p, octopus::StateAndSteps const &steps_p, SpriteLibrary const &lib_p);

	/// @brief update all sprites and
	void display(Window &window_p, octopus::State const &state_p, double elapsed_p);

	/// @brief get sprite under coordinate on the window
	/// @param state_p is optional, ifset only visible sprites will be returned
	/// @return nullptr if no sprint under coordinate
	SpriteEntity * getSprite(Window const &window_p, int x, int y, octopus::State const *state_p) const;

	/// @brief get all sprites in the box on the window
	/// @param state_p is optional, ifset only visible sprites will be returned
	/// @return nullptr if no sprint under coordinate
	std::list<SpriteEntity *> getSprites(Window const &window_p, int lx, int ly, int ux, int uy, octopus::State const *state_p) const;

	Selection &getSelection() { return _selection; }
	Selection const &getSelection() const { return _selection; }

	/// @brief get all registered selection
	std::unordered_map<unsigned long, Selection> & getSelections() { return _selections; }
	const std::unordered_map<unsigned long, Selection>  & getSelections() const { return _selections; }

	/// @brief adding to a selection
	void addSelection(unsigned long idx_p);
	/// @brief register a selection
	void registerSelection(unsigned long idx_p);
	/// @brief use selection (discarding current)
	void useSelection(unsigned long idx_p);

	/// @brief remove a sprite from all selections
	void clearSpriteFromSelections(SpriteEntity * sprite_p, octopus::State const &state_p);

	std::list<SpriteEntity *> const &getListSprite() const;

	unsigned long getPlayer() const { return _player; }


	std::list<Picture *> const &getDyingPictures() const { return _dyingPictures; }
	std::list<Picture *> &getDyingPictures() { return _dyingPictures; }

	std::list<SpriteEntity *> const &getDyingSprites() const { return _dyingSprites; }
	std::list<SpriteEntity *> &getDyingSprites() { return _dyingSprites; }

private:
	/// @brief the player for this world
	unsigned long const _player;

	/// @brief sprite of every entity
	/// content can be nullptr in case of sprite
	/// reprensenting dead entities
	std::vector<SpriteEntity *> _sprites;
	/// @brief this list is kept to avoid iterating on a lot of dead sprites
	std::list<SpriteEntity *> _listSprite;

	/// @brief list of all picture that are going to be destroyed after one run
	std::list<Picture *> _dyingPictures;
	/// @brief list of all sprites that are going to be destroyed after one run
	std::list<SpriteEntity *> _dyingSprites;

	/// @brief current selection
	Selection _selection;

	/// @brief group of controls
	std::unordered_map<unsigned long, Selection> _selections;

	/// @brief handlers for last step used
	bool _first {true};
	std::list<octopus::Step *>::const_iterator _lastIt;

	friend class WorldUpdaterStepVisitor;
};
} // namespace cuttlefish


#endif
