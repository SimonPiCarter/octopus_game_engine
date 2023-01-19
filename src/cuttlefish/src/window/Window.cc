#include "Window.hh"

#include <iostream>
#include <SDL2/SDL_image.h>

#include "texture/Texture.hh"

namespace cuttlefish
{

bool Window::init(int width_l, int height_l)
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Create window
		_window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width_l, height_l, SDL_WINDOW_SHOWN );
		if( _window == nullptr )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			_renderer = SDL_CreateRenderer( _window, -1, SDL_RENDERER_ACCELERATED );
			if( _renderer == nullptr )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( _renderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_Rect clip_l {0, 0, width_l, height_l};
				SDL_RenderSetClipRect(_renderer, &clip_l);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

void Window::close()
{
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

void Window::setCamera(int x, int y)
{
	_camera.x = x;
	_camera.y = y;
}

} // cuttlefish
