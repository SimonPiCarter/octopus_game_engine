#ifndef __Window__
#define __Window__

#include <chrono>
#include <string>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "text/Text.hh"

#include "utils/Vector.hh"

namespace cuttlefish
{
class Texture;

/// @brief This class is supposed to contains the
/// requirement for the window and the drawing panel
/// It stores internal SDL pointers and camera options
/// for example
class Window
{
public:
	/// @brief initialize window and renderer
	bool init(int width_p, int height_p, unsigned long worldSize_p);

	void close();

	void clear();

	void draw();

	SDL_Window* getWindow() { return _window; }
	SDL_Renderer* getRenderer() { return _renderer; }

	Texture const * loadTexture(std::string const &path_p);
	void deleteTexture(std::string const &path_p);

	SDL_Point const &getCamera() const { return _camera; }
	/// @brief update given parameters to clamp them into the world
	void clampCamera(double &x_r, double &y_r);
	void setCamera(int x, int y);

	void displayFps(bool display_p) { _displayFps = display_p; }

	TTF_Font* getFont() { return _font; }

	void setWorldSize(unsigned long worldSize_p) { _worldSize = worldSize_p; }
	unsigned long getWorldSize() const { return _worldSize; }

	/// @brief return the position in the world given the coordinate on the window
	/// and the camera position
	octopus::Vector getWorldVector(int x, int y) const;

	/// @return pixel position based on a given world position
	/// @note works in absolute coordinate
	octopus::Vector getPixelVector(double x, double y) const;

	/// @brief get the window size in world unit
	octopus::Vector getWindowSize() const;

	int getWidth() const { return _width; }
	int getHeight() const { return _height; }

	void setWindowedFullScreeen();
	void setWindowed();

private:
	void updateFromRenderer();
	std::map<std::string, Texture *> _mapTexture;

	//The window we'll be rendering to
	SDL_Window* _window {nullptr};

	//The window renderer
	SDL_Renderer* _renderer {nullptr};

	// camera offset
	SDL_Point _camera {0, 0};

	TTF_Font* _font {nullptr};

	int _width {0};
	int _height {0};
	int _baseWidth {0};
	int _baseHeight {0};
	unsigned long _worldSize {0};

	// display fps
	bool _displayFps {false};
	double _fps {0.};
	double _elpased {0.};
	long _frameCount {0};
	std::chrono::time_point<std::chrono::steady_clock> _refTime;

	// Text for fps display
	Text *_text {nullptr};
};

} // cuttlefish

#endif
