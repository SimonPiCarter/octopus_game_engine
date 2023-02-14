#include "Selection.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "texture/Texture.hh"
#include "window/Window.hh"
#include "sprite/Sprite.hh"

namespace cuttlefish
{

void Selection::removeSprite(Sprite * sprite_p)
{
	_sprites.remove(sprite_p);
	if(_sprite == sprite_p)
	{
		if(!_sprites.empty())
		{
			_sprite = *_sprites.begin();
		}
		else
		{
			_sprite = nullptr;
		}
	}
}

void displaySelection(Window &window_p, Sprite const *sprite_p)
{
	SDL_Rect final_l {
		int(sprite_p->getX() - sprite_p->getWidth()/2),
		int(sprite_p->getY() - sprite_p->getHeight()/2),
		int(sprite_p->getWidth()),
		int(sprite_p->getHeight()),
	};

	SDL_Point const &cam_l = window_p.getCamera();
	window_p.loadTexture("resources/selection.png")->render(window_p.getRenderer(),
		final_l.x - cam_l.x, final_l.y - cam_l.y, final_l.h, final_l.w, nullptr);
}

void Selection::render(Window &window_p) const
{
	for(Sprite const * sprite_l : _sprites)
	{
		displaySelection(window_p, sprite_l);
	}
}

void Selection::clear()
{
	_sprites.clear();
	_sprite = nullptr;
}

void addToSelection(Selection &selection_p, std::list<Sprite *> const &sprites_p)
{
	selection_p._sprites.insert(selection_p._sprites.end(), sprites_p.begin(), sprites_p.end());
	if(!selection_p._sprites.empty())
	{
		selection_p._sprite = *selection_p._sprites.begin();
	}
}

void replaceSelection(Selection &selection_p, std::list<Sprite *> const &sprites_p)
{
	selection_p._sprites = sprites_p;
	if(!selection_p._sprites.empty())
	{
		selection_p._sprite = *selection_p._sprites.begin();
	}
	else
	{
		selection_p._sprite = nullptr;
	}
}

} // cuttlefish
