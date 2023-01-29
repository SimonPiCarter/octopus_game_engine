#include "Text.hh"

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "window/Window.hh"

namespace cuttlefish
{
	void displayText(Window* window_p, std::string const &text_p, SDL_Color const &color_p, int x, int y)
	{
		SDL_Surface* text;

		text = TTF_RenderText_Solid( window_p->getFont(), text_p.c_str(), color_p );
		if ( !text ) {
			std::cout << "Failed to render text: " << TTF_GetError() << std::endl;
		}
		SDL_Texture* text_texture = SDL_CreateTextureFromSurface( window_p->getRenderer(), text );

		SDL_Rect dest = { x, y, text->w, text->h };

		SDL_RenderCopy( window_p->getRenderer(), text_texture, nullptr, &dest );
	}
} // namespace cuttlefish

