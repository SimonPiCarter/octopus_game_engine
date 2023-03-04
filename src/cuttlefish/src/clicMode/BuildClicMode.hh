#ifndef __BuildClicMode__
#define __BuildClicMode__

#include "ClicMode.hh"

namespace octopus
{
struct BuildingModel;;
} // namespace octopus


namespace cuttlefish
{
class SpriteLibrary;
class Sprite;

/// @brief This clic mode enable selection with left clic
/// and move clic with right clic if anything is selected
class BuildClicMode : public ClicMode
{
public:
	BuildClicMode(octopus::BuildingModel const &model_p, SpriteLibrary const &lib_p);
	~BuildClicMode();

	/// @brief handle mouse
	/// @return true if the clic mode is over
	bool handleMouseUp(SDL_Event const & e, Selection &selection_p, World &world_p, Panel &panel_p,
	Window &window_p, octopus::State const &state_p, octopus::Controller &controller_p) override;

	/// @brief display sprites handled in this clic mode
	void display(Window &window_p, double elapsed_p, int x, int y) override;

protected:
	octopus::BuildingModel const & _model;

	/// @brief sprite of the blueprint
	Sprite * _sprite;
};

} // namespace cuttlefish


#endif
