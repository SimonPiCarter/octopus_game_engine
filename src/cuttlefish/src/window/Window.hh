#ifndef __Window__
#define __Window__

#include <string>
#include <map>
#include <SDL2/SDL.h>


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
	bool init(int width_l, int height_l);

	void close();

	void clear();

	void draw();

	SDL_Window* getWindow() { return _window; }
	SDL_Renderer* getRenderer() { return _renderer; }

	Texture const * loadTexture(std::string const &path_p);
	void deleteTexture(std::string const &path_p);

	SDL_Point const &getCamera() const { return _camera; }
	void setCamera(int x, int y);

private:
	std::map<std::string, Texture *> _mapTexture;

	//The window we'll be rendering to
	SDL_Window* _window {nullptr};

	//The window renderer
	SDL_Renderer* _renderer {nullptr};

	// camera offset
	SDL_Point _camera {0, 0};
};

} // cuttlefish

#endif
