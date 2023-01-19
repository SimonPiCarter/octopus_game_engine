#ifndef __Sprite__
#define __Sprite__

#include <string>
#include <vector>

class SDL_Renderer;

namespace cuttlefish
{

class Texture;

/// @brief sprite used to render an entity in the world
/// a sprite is composed of different states each composed of
/// different frames
/// a sprite must be initialized with a texture using the
/// parameters
/// - number of states
/// - number of frames per state
/// - offset per state (width of one sprite)
/// - offset per frame (height of one sprite)
/// - logical center of the frame : a sprite will be positioned on the screen with
///   its logical center placed on the given point of the frame
/// - The texture to be used (is gonna be shared among all sprites)
class Sprite
{
	public:
		Sprite(Texture const * texture_p, double scale_p, int logX_p, int logY_p, int width_p, int height_p,
			std::vector<int> const &nbFramesPerState_p, std::vector<double> const &timePerFramePerState_p);

		/// @brief update state and reset current frame and time into frame
		void setState(int state_p);

		/// @brief update frame based on elapsed time and refresh rate
		void update(double elapsedTime_l);

		/// @brief render sprite with its logical center on (x, y)
		/// call texture render using a clip based on state and frame
		void render( SDL_Renderer* renderer, int x, int y );

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The underlying texture
		Texture const * const _texture;

		double const _scale;

		int const _logicalX;
		int const _logicalY;

		//Image dimensions
		int const _width;
		int const _height;

		/// @brief the number of frames per state
		std::vector<int> const _nbFramesPerState;

		std::vector<double> const _timePerFramePerState;

		/// @brief the time since frame has changed
		double _timeIntoFrame {0.};
		/// @brief current state
		int _state {0};
		/// @brief current frame
		int _frame {0};
};

} // namespace cuttlefish


#endif
