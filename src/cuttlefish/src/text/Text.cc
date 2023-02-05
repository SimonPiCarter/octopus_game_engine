#include "Text.hh"

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "window/Window.hh"
#include "logger/Logger.hh"

namespace cuttlefish
{

Text::Text(Window* window_p, SDL_Color const &color_p, int x, int y)
	: _window(window_p)
	, _color(color_p)
	, _x(x)
	, _y(y)
{}

Text::~Text()
{
	clean();
}

void Text::setText(std::string const & text_p)
{
	if(text_p != _text)
	{
		clean();
		_surface = TTF_RenderText_Solid( _window->getFont(), text_p.c_str(), _color );
		if ( !_surface ) {
			octopus::Logger::getNormal() << "Failed to render text: " << TTF_GetError() << std::endl;
		}
		_texture = SDL_CreateTextureFromSurface( _window->getRenderer(), _surface );
	}
	_text = text_p;
}

void Text::display()
{
		SDL_Rect dest_l = { _x, _y, _surface->w, _surface->h };

		SDL_RenderCopy( _window->getRenderer(), _texture, nullptr, &dest_l );
}

void Text::clean()
{
	if(_texture)
	{
		SDL_DestroyTexture(_texture);
	}
	if(_surface)
	{
		SDL_FreeSurface(_surface);
	}
}


} // namespace cuttlefish

