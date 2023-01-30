#ifndef __World__
#define __World__

#include <vector>
#include <list>

namespace octopus
{
	class StateAndSteps;
	class Step;
}

namespace cuttlefish
{
class Sprite;
class Window;

/// @brief This class is used to maintain a representation of the world
/// based on state the engine sends
/// Each update checks for new Entity and remove non alive entities
class World
{
public:

	/// @brief update all sprites from the steps given
	/// only works if all previous steps have been handled this way
	void handleStep(Window &window_p, octopus::StateAndSteps const &steps_p);

	/// @brief update all sprites and
	void display(Window &window_p, double elapsed_p);

	/// @brief get sprite under coordinate on the window
	/// @return nullptr if no sprint under coordinate
	Sprite * getSprite(Window &window_p, int x, int y) const;

private:
	/// @brief sprite of every entity
	/// content can be nullptr in case of sprite
	/// reprensenting dead entities
	std::vector<Sprite *> _sprites;
	/// @brief this list is kept to avoid iterating on a lot of dead sprites
	std::list<Sprite *> _listSprite;

	bool _first {true};
	std::list<octopus::Step *>::const_iterator _lastIt;

	friend class WorldUpdaterStepVisitor;
};
} // namespace cuttlefish


#endif
