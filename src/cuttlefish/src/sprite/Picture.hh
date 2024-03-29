#ifndef __Picture__
#define __Picture__

#include <string>
#include <vector>

#include <SDL2/SDL.h>

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
/// - The texture to be used (is gonna be shared among all pictures)
class Picture
{
	public:
		/// @brief
		/// @param texture_p
		/// @param width_p
		/// @param height_p
		/// @param nbFramesPerState_p
		/// @param timePerFramePerState_p
		/// @param absCoord_p if false the destination should be given in absolute pixel coordinate and will be updated based on camera
		Picture(Texture const * texture_p, int width_p, int height_p, std::vector<int> const &nbFramesPerState_p, std::vector<double> const &timePerFramePerState_p, bool absCoord_p=true);
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
		void update(double elapsedTime_p);

		/// @brief render picture
		/// call texture render using a clip based on state and frame
		void display( Window &window_p);

		/// @brief check if the position in on the picture
		bool isInside(Window const &window_p, int x, int y) const;

		/// @brief check if the picture intersect the box
		/// @note do not require lx < ux nor ly < uy
		/// @note x and y coordinates must be in absolute world coordinate
		bool intersect(int lx, int ly, int ux, int uy) const;

		/// @brief set the destination on screen
		/// @param x
		/// @param y
		/// @param w
		/// @param h
		void setDestination(int x, int y, int w, int h);

		SDL_Rect const & getDestination() const;
		SDL_Rect getClip() const;

		//Getter SCREEN picture dimensions
		int getWidth() const;
		int getHeight() const;

		/// Getter texture picture dimension
		int getTextureAtomicWidth() const;
		int getTextureAtomicHeight() const;

		void setEndAfterLastFrame(bool b);
		bool isEndAfterLastFrame() const;

		bool isEnded() const;

	protected:
		//The underlying texture
		Texture const * const _texture;

		//Image dimensions
		int const _width;
		int const _height;

		/// @brief the number of frames per state
		std::vector<int> const _nbFramesPerState;

		std::vector<double> const _timePerFramePerState;

		////
		//// Dynamic data for position
		////

		// destination on screen
		SDL_Rect _dest;

		////
		//// Dynamic data for animation
		////

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
		/// @brief boolean to tell that this Picture should end at next frame reset
		bool _endAfterLastFrame {false};
		/// @brief boolean to track if the picture ended (result of a frame reset with _endAfterLastFrame true)
		/// @note picture will not render if this is set to true
		bool _ended {false};
		/// @brief if true destination will be in absolute coordinate (no camera adaptation)
		bool _absCoord {false};
};

} // namespace cuttlefish


#endif
