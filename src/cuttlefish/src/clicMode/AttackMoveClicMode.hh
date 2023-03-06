#ifndef __AttackMoveClicMode__
#define __AttackMoveClicMode__

#include "ClicMode.hh"

namespace octopus
{
	struct Vector;
} // namespace octopus


namespace cuttlefish
{

/// @brief This clic mode enable selection with left clic
/// and move clic with right clic if anything is selected
class AttackMoveClicMode : public ClicMode
{
public:
	virtual ~AttackMoveClicMode();

	/// @brief handle mouse
	/// @return true if the clic mode is over
	bool handleMouseUp(SDL_Event const & e, Selection &selection_p, World &world_p, Panel &panel_p,
	Window &window_p, octopus::State const &state_p, octopus::Controller &controller_p) override;

	/// @brief display sprites handled in this clic mode
	void display(Window &window_p, double elapsed_p, int x, int y) override;

	/// @brief handle a left clic
	void handleLeftClic(octopus::Vector const &clicWorldPosition_p, Selection &selection_p, World &world_p, Panel &panel_p,
							Window &window_p, octopus::State const &state_p, octopus::Controller &controller_p, SpriteEntity * sprite_p);
};

} // namespace cuttlefish


#endif
