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
		std::cout<< "SDL could not initialize! SDL Error: "<< SDL_GetError() <<std::endl;
		success = false;
	}
	else
	{
		//Create window
		_window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width_l, height_l, SDL_WINDOW_SHOWN );
		if( _window == nullptr )
		{
			std::cout<< "Window could not be created! SDL Error: "<< SDL_GetError() <<std::endl;
			success = false;
		}
		else
		{
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
				SDL_Rect clip_l {0, 0, width_l, height_l};
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

	_refTime = std::chrono::steady_clock::now();

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
		SDL_Surface* text;
		// Set color to black
		SDL_Color color = { 0, 0, 0 };

		std::string strFps_l = std::to_string(_fps);
		text = TTF_RenderText_Solid( _font, strFps_l.c_str(), color );
		if ( !text ) {
			std::cout << "Failed to render text: " << TTF_GetError() << std::endl;
		}
		SDL_Texture* text_texture = SDL_CreateTextureFromSurface( _renderer, text );

		SDL_Rect dest = { 0, 0, text->w, text->h };

		SDL_RenderCopy( _renderer, text_texture, nullptr, &dest );
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

void Window::setCamera(int x, int y)
{
	_camera.x = x;
	_camera.y = y;
}

} // cuttlefish
