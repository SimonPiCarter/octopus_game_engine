#include "Window.hh"

#include <iostream>
#include <SDL2/SDL_image.h>

#include "texture/Texture.hh"
#include "text/Text.hh"

using octopus::to_double;

namespace cuttlefish
{

bool Window::init(int width_p, int height_p, unsigned long worldSize_p)
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		std::cout<< "SDL could not initialize! SDL Error: "<< SDL_GetError() <<std::endl;
		success = false;
	}
	else
	{
		//Create window
		_window = SDL_CreateWindow( "Cuttlefish", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width_p, height_p, SDL_WINDOW_SHOWN );
		if( _window == nullptr )
		{
			std::cout<< "Window could not be created! SDL Error: "<< SDL_GetError() <<std::endl;
			success = false;
		}
		else
		{
			_width = width_p;
			_height = height_p;
			_worldSize = worldSize_p;
			//Create renderer for window
			_renderer = SDL_CreateRenderer( _window, -1, SDL_RENDERER_ACCELERATED );
			if( _renderer == nullptr )
			{
				std::cout<< "Renderer could not be created! SDL Error: "<< SDL_GetError() <<std::endl;
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( _renderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_Rect clip_l {0, 0, width_p, height_p};
				SDL_RenderSetClipRect(_renderer, &clip_l);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					std::cout<< "SDL_image could not initialize! SDL_image Error: "<< IMG_GetError() <<std::endl;
					success = false;
				}
				if ( TTF_Init() < 0 ) {
					std::cout << "Error initializing SDL_ttf: " << TTF_GetError() << std::endl;
				}

				_font = TTF_OpenFont("resources/font.ttf", 24);
				if ( !_font ) {
					std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
				}
			}
		}
	}

	_text = new Text(this, {0,0,0}, 0, 0);

	_refTime = std::chrono::steady_clock::now();

	return success;
}

void Window::close()
{
	delete _text;

	for(auto &&pair_l : _mapTexture)
	{
		if(pair_l.second)
		{
			pair_l.second->free();
		}
	}
	_mapTexture.clear();
	//Destroy window
	SDL_DestroyRenderer( _renderer );
	SDL_DestroyWindow( _window );
	_window = nullptr;
	_renderer = nullptr;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}
void Window::clear()
{
	//Clear screen
	SDL_SetRenderDrawColor( _renderer, 0xFF, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear( _renderer );
}

void Window::draw()
{
	// udate framerate
	++_frameCount;
	auto cur_l = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds_l = cur_l-_refTime;
	_elpased += elapsed_seconds_l.count();
	_refTime = cur_l;

	if(_frameCount == 100)
	{
		_fps = _frameCount/_elpased;
		_frameCount = 0;
		_elpased = 0;
	}

	if(_displayFps)
	{
		_text->setText(stringify(_fps));
		_text->display(*this);
	}

	//Update screen
	SDL_RenderPresent( _renderer );

}

Texture const * Window::loadTexture(std::string const &path_p)
{
	if(!_mapTexture[path_p])
	{
		Texture *texture_l = new Texture();
		if(texture_l->loadFromFile(path_p, _renderer))
		{
			_mapTexture[path_p] = texture_l;
		}
	}
	return _mapTexture[path_p];
}

void Window::deleteTexture(std::string const &path_p)
{
	if(_mapTexture[path_p])
	{
		delete _mapTexture[path_p];
		_mapTexture[path_p] = nullptr;
	}
}

void Window::clampCamera(double &x_r, double &y_r)
{
	// clamp position
	octopus::Vector winsize_l {double(_width), double(_height)};
	octopus::Vector max_l = getPixelVector(_worldSize, _worldSize) - winsize_l;

	x_r = std::max(0., std::min(to_double(max_l.x), x_r));
	y_r = std::max(0., std::min(to_double(max_l.y), y_r));
}

void Window::setCamera(int x, int y)
{
	_camera.x = x;
	_camera.y = y;
}

octopus::Vector Window::getWorldVector(int x, int y) const
{
	return {(x + _camera.x)/32., (y + _camera.y)/32. };
}

octopus::Vector Window::getPixelVector(double x, double y) const
{
	return { x * 32, y * 32 };
}

octopus::Vector Window::getWindowSize() const
{
	return {_width/32., _height/32.};
}


} // cuttlefish
