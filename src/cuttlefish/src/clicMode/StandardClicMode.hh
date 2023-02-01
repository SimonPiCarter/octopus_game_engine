#ifndef __StandardClicMode__
#define __StandardClicMode__

#include "ClicMode.hh"

namespace cuttlefish
{

/// @brief This clic mode enable selection with left clic
/// and move clic with right clic if anything is selected
class StandardClicMode : public ClicMode
{
public:
	virtual ~StandardClicMode();

	/// @brief handle mouse
	/// @return true if the clic mode is over
	bool handleMouse(SDL_Event const & e, Selection &selection_p, World &world_p, Panel &panel_p,
	Window &window_p, octopus::State const &state_p, octopus::Controller &controller_p) override;

	/// @brief display sprites handled in this clic mode
	void display(Window &window_p, double elapsed_p, int x, int y) override;
};

} // namespace cuttlefish


#endif
