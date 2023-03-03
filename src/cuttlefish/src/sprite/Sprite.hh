#ifndef __Sprite__
#define __Sprite__

#include "state/Handle.hh"
#include "Picture.hh"

namespace cuttlefish
{

/// @brief sprite used to render an entity in the world based on Picture
/// - logical center of the frame : a picture will be positioned on the screen with
///   its logical center placed on the given point of the frame
class Sprite : public Picture
{
	public:
		Sprite(octopus::Handle const &ent_p, Texture const * texture_p, double scale_p, int logX_p, int logY_p, int width_p, int height_p,
			std::vector<int> const &nbFramesPerState_p, std::vector<double> const &timePerFramePerState_p, bool absolute_p=false);

		octopus::Handle const & getHandle() const;

		void render(Window &window_p);
		void renderScaled(Window &window_p, double scaleX_p, double scaleY_p);

		void setPosition(double x, double y);
		double getX() const;
		double getY() const;

	private:
		/// @brief the handle of the entity displayed
		octopus::Handle const _handle;

		double const _scale;
		int const _logicalX;
		int const _logicalY;

		bool const _absolute;

		double _x {0};
		double _y {0};
};

} // namespace cuttlefish


#endif
