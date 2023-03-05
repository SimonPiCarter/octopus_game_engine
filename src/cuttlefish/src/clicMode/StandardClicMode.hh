#ifndef __StandardClicMode__
#define __StandardClicMode__

#include <chrono>
#include "ClicMode.hh"

namespace octopus
{
	struct Vector;
} // namespace octopus


namespace cuttlefish
{

/// @brief This clic mode enable selection with left clic
/// and move clic with right clic if anything is selected
class StandardClicMode : public ClicMode
{
public:
	virtual ~StandardClicMode();


	virtual void handleMouseDown(SDL_Event const &e)
	{
		if(e.button.button == SDL_BUTTON_LEFT)
		{
			_downX = e.button.x;
			_downY = e.button.y;
		}
	}

	virtual void reset(SDL_Event const &e)
	{
		if(e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT)
		{
			_downX = -1;
			_downY = -1;
		}
	}

	/// @brief handle mouse
	/// @return true if the clic mode is over
	bool handleMouseUp(SDL_Event const & e, Selection &selection_p, World &world_p, Panel &panel_p,
	Window &window_p, octopus::State const &state_p, octopus::Controller &controller_p) override;

	/// @brief display sprites handled in this clic mode
	void display(Window &window_p, double elapsed_p, int x, int y) override;

	/// @brief handle a right clic
	void handleRightClic(octopus::Vector const &clicWorldPosition_p, Selection &selection_p, World &world_p, Panel &panel_p,
							Window &window_p, octopus::State const &state_p, octopus::Controller &controller_p, SpriteEntity * sprite_p);
private:
	int _downX {-1};
	int _downY {-1};

	/// @brief check if box is buig enough to trigger box selection
	bool isBoxEnough(int x, int y) const;

	/// @brief check if double clic
	/// @return true if time between two calls of this is small enough
	/// @note cannot be const because it will update last clic time
	bool isDoubleClic();
	std::chrono::time_point<std::chrono::steady_clock> lastCallForDoubleClic_l;

	/// @brief last clicked sprite for doubl clic handlin
	SpriteEntity * _lastCickedSprite {nullptr};
};

} // namespace cuttlefish


#endif
