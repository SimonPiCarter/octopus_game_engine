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

void Text::setColor(SDL_Color const &color_p)
{
	_color = color_p;
	clean();
	generate();
}

void Text::setText(std::string const & text_p)
{
	if(text_p != _text)
	{
		_text = text_p;
		clean();
		if(text_p != "")
		{
			generate();
		}
	}
}

void Text::setPosition(int x, int y)
{
	_x = x;
	_y = y;
}

void Text::display(Window& window_p) const
{
	if(_surface && _texture)
	{
		SDL_Rect dest_l = { _x, _y, _surface->w, _surface->h };

		SDL_RenderCopy( window_p.getRenderer(), _texture, nullptr, &dest_l );
	}
}

void Text::clean()
{
	if(_texture)
	{
		SDL_DestroyTexture(_texture);
		_texture = nullptr;
	}
	if(_surface)
	{
		SDL_FreeSurface(_surface);
		_surface = nullptr;
	}
}

void Text::generate()
{
	_surface = TTF_RenderText_Blended( _window->getFont(), _text.c_str(), _color );
	if ( !_surface ) {
		octopus::Logger::getNormal() << "Failed to render text: " << TTF_GetError() << std::endl;
	}
	else
	{
		_texture = SDL_CreateTextureFromSurface( _window->getRenderer(), _surface );
	}
}

} // namespace cuttlefish

