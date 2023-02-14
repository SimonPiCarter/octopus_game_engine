#ifndef __World__
#define __World__

#include <vector>
#include <list>
#include <unordered_map>

#include "Selection.hh"

namespace octopus
{
	struct StateAndSteps;
	class Step;
}

namespace cuttlefish
{
class DivinityPanel;
class Panel;
class Sprite;
class SpriteLibrary;
class Window;

/// @brief This class is used to maintain a representation of the world
/// based on state the engine sends
/// Each update checks for new Entity and remove non alive entities
class World
{
public:

	/// @brief update all sprites from the steps given
	/// only works if all previous steps have been handled this way
	void handleStep(Window &window_p, Panel &panel_p, DivinityPanel &divPanel_p, octopus::StateAndSteps const &steps_p, SpriteLibrary const &lib_p);

	/// @brief update all sprites and
	void display(Window &window_p, double elapsed_p);

	/// @brief get sprite under coordinate on the window
	/// @return nullptr if no sprint under coordinate
	Sprite * getSprite(Window const &window_p, int x, int y) const;

	/// @brief get all sprites in the box on the window
	/// @return nullptr if no sprint under coordinate
	std::list<Sprite *> getSprites(Window const &window_p, int lx, int ly, int ux, int uy) const;

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

private:
	/// @brief sprite of every entity
	/// content can be nullptr in case of sprite
	/// reprensenting dead entities
	std::vector<Sprite *> _sprites;
	/// @brief this list is kept to avoid iterating on a lot of dead sprites
	std::list<Sprite *> _listSprite;

	/// @brief current selection
	Selection _selection;

	/// @brief group of controls
	std::unordered_map<unsigned long, Selection> _selections;

	bool _first {true};
	std::list<octopus::Step *>::const_iterator _lastIt;

	friend class WorldUpdaterStepVisitor;
};
} // namespace cuttlefish


#endif
