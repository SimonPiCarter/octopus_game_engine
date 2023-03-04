#include "Selection.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "texture/Texture.hh"
#include "window/Window.hh"
#include "sprite/SpriteEntity.hh"

#include "state/State.hh"
#include "state/entity/Entity.hh"

namespace cuttlefish
{

bool SpriteComparator::operator()(SpriteEntity const *a, SpriteEntity const *b) const
{
	return a->getHandle() < b->getHandle();
}

void Selection::removeSprite(SpriteEntity * sprite_p, octopus::State const &state_p)
{
	_sprites.erase(sprite_p);
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
	octopus::Entity const * ent_l = state_p.getEntity(sprite_p->getHandle());
	_spritesPerModel[ent_l->_model._id].erase(sprite_p);
}

void displaySelection(Window &window_p, SpriteEntity const *sprite_p)
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
	for(SpriteEntity const * sprite_l : _sprites)
	{
		displaySelection(window_p, sprite_l);
	}
}

void Selection::clear()
{
	_sprites.clear();
	_spritesPerModel.clear();
	_sprite = nullptr;
}

SelectionKey Selection::key() const
{
	SelectionKey key_l;

	for(auto &&pair_l : _spritesPerModel)
	{
		key_l._spritesPerModel[pair_l.first] = pair_l.second.size();
	}

	return key_l;
}

void addToSelection(Selection &selection_p, std::list<SpriteEntity *> const &sprites_p, octopus::State const &state_p)
{
	selection_p._sprites.insert(sprites_p.begin(), sprites_p.end());
	if(!selection_p._sprites.empty())
	{
		selection_p._sprite = *selection_p._sprites.begin();
	}
	// update model lists
	for(SpriteEntity * sprite_l : sprites_p)
	{
		octopus::Entity const * ent_l = state_p.getEntity(sprite_l->getHandle());
		selection_p._spritesPerModel[ent_l->_model._id].insert(sprite_l);
	}
}

void addToSelection(Selection &selection_p, Selection &other_p)
{
	selection_p._sprites.insert(other_p._sprites.begin(), other_p._sprites.end());
	if(!selection_p._sprites.empty())
	{
		selection_p._sprite = *selection_p._sprites.begin();
	}
	// update model lists
	for(auto &&pair_l : other_p._spritesPerModel)
	{
		selection_p._spritesPerModel[pair_l.first].insert(pair_l.second.begin(), pair_l.second.end());
	}
}

void replaceSelection(Selection &selection_p, std::list<SpriteEntity *> const &sprites_p, octopus::State const &state_p)
{
	selection_p._sprites.clear();
	selection_p._sprites.insert(sprites_p.begin(), sprites_p.end());
	if(!selection_p._sprites.empty())
	{
		selection_p._sprite = *selection_p._sprites.begin();
	}
	else
	{
		selection_p._sprite = nullptr;
	}
	// update model lists
	selection_p._spritesPerModel.clear();
	for(SpriteEntity * sprite_l : sprites_p)
	{
		octopus::Entity const * ent_l = state_p.getEntity(sprite_l->getHandle());
		selection_p._spritesPerModel[ent_l->_model._id].insert(sprite_l);
	}
}

} // cuttlefish
