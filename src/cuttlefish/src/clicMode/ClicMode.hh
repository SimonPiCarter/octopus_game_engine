#ifndef __ClicMode__
#define __ClicMode__

#include <SDL2/SDL.h>

#include "Selection.hh"

namespace octopus
{
class Controller;
class State;
} // namespace octopus


namespace cuttlefish
{
class Panel;
class Window;
class World;
/// @brief A clic mode controls the way clics are going to be handled
/// the panel always takes priority over clic mode
/// clic mode is responsible for
/// - handling clic event
/// - displaying indicators
///   - mouse overlay
///   - blueprint for building
/// - terminating itself
class ClicMode
{
public:
	virtual ~ClicMode() {}

	/// @brief handle mouse
	/// @return true if the clic mode is over
	virtual bool handleMouse(SDL_Event const & e, Selection &selection_p, World &world_p, Panel &panel_p,
	Window &window_p, octopus::State const &state_p, octopus::Controller &controller_p) = 0;

	/// @brief display sprites handled in this clic mode
	virtual void display(Window &window_p, double elapsed_p, int x, int y) = 0;
};

} // namespace cuttlefish


#endif
