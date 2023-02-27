#ifndef __Picture__
#define __Picture__

#include <string>
#include <vector>

namespace cuttlefish
{

class Texture;
class Window;

/// @brief picture used to render an entity in the world
/// a picture is composed of different states each composed of
/// different frames
/// a picture must be initialized with a texture using the
/// parameters
/// - number of states
/// - number of frames per state
/// - offset per state (width of one picture)
/// - offset per frame (height of one picture)
/// - logical center of the frame : a picture will be positioned on the screen with
///   its logical center placed on the given point of the frame
/// - The texture to be used (is gonna be shared among all pictures)
class Picture
{
	public:
		Picture(Texture const * texture_p, double scale_p, int logX_p, int logY_p, int width_p, int height_p,
			std::vector<int> const &nbFramesPerState_p, std::vector<double> const &timePerFramePerState_p, bool absolute_p=false);
        virtual ~Picture() {}

		/// @brief update state and reset current frame and time into frame
		void setState(int state_p);

		/// @brief update state
		/// @warning do not reset the frame
		/// @warning this may result in out of bound frame selection (if the frame is > to frame of the new state)
		void setStateNoReset(int state_p);

		/// @brief queue state after this one
		/// @param state_p next state
		void queueState(int state_p);

		/// @brief return true if a state is queued
		bool hasStateQueued() const;

		/// @brief update frame to given value
		/// @note mainly used for icons
		void setFrame(int frame_p);

		/// @brief update frame based on elapsed time and refresh rate
		void update(double elapsedTime_l);

		/// @brief render picture with its logical center on (x, y)
		/// call texture render using a clip based on state and frame
		/// @param scale_p allow to scale final position and size
		/// @param forceAbsolute_p force absolute drawing
		void render( Window &window_p, double scaleX_p=1., double scaleY_p=1., bool forceAbsolute_p=false );

		/// @brief check if the position in on the picture
		bool isInside(Window const &window_p, int x, int y) const;

		/// @brief check if the picture intersect the box
		/// @note do not require lx < ux nor ly < uy
		/// @note x and y coordinates must be in absolute world coordinate
		bool intersect(int lx, int ly, int ux, int uy) const;

		//Gets picture dimensions
		double getWidth() const;
		double getHeight() const;

		void setPosition(double x, double y);
		void move(double dx, double dy);
		double getX() const;
		double getY() const;

	protected:
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

		/// @brief if true camera is not applied
		bool const _absolute;

		////
		//// Dynamic data
		////

		double _x {0};
		double _y {0};

		/// @brief the time since frame has changed
		double _timeIntoFrame {0.};
		/// @brief current state
		int _state {0};
		/// @brief current frame
		int _frame {0};
		/// @brief boolean to know if one state is queued up
		bool _hasStateQueuedUp {false};
		/// @brief the state queued up after current is over
		int _stateNext {0};
};

} // namespace cuttlefish


#endif
