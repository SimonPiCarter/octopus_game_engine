#include "Minimap.hh"

#include "window/Window.hh"
#include "world/World.hh"
#include "sprite/SpriteEntity.hh"
#include "texture/Texture.hh"
#include "tilemap/Tilemap.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "state/entity/Entity.hh"

using octopus::to_double;
using octopus::to_int;

namespace cuttlefish
{

Minimap::Minimap(Window &window_p, int x, int y, int w, int h,
				std::vector<std::string> const &map_p)
	: _x(x), _y(y), _w(w), _h(h), _vecPlayerToTexture(map_p)
{
	_cadre = window_p.loadTexture("resources/background.png");
}

Minimap::~Minimap()
{
	SDL_FreeSurface(_fogSurface);
}

void Minimap::generate(Window &window_p, Tilemap &tilemap_p, unsigned long worldSize_p)
{
	int resolution_l = 1024;
	_background.createBlank(resolution_l, resolution_l, window_p.getRenderer());

	_background.startRendering(window_p.getRenderer());

	tilemap_p.renderScaled(window_p, resolution_l/(worldSize_p*32.), resolution_l/(worldSize_p*32.));

	_background.stopRendering(window_p.getRenderer());

	_fogSurface = SDL_CreateRGBSurfaceWithFormat(0, worldSize_p, worldSize_p, 32, SDL_PIXELFORMAT_RGBA8888);
}


void Minimap::render(octopus::State const &state_p, World const &world_p, Window &window_p)
{

	// fog rendering
	SDL_LockSurface(_fogSurface);
	Uint32 *pixels = (Uint32 *)_fogSurface->pixels;
	for(unsigned long i = 0; i < state_p.getWorldSize(); ++i)
	{
		for(unsigned long j = 0; j < state_p.getWorldSize(); ++j)
		{
			Uint8 alpha_l = state_p.getVisionHandler().isVisible(0, i, j)?0:120;
			if(!state_p.getVisionHandler().isExplored(0, i, j))
			{
				alpha_l = 255;
			}
			pixels[j * state_p.getWorldSize() + i] = SDL_MapRGBA(_fogSurface->format, 0, 0, 0, alpha_l);
		}
	}

	SDL_UnlockSurface(_fogSurface);

	SDL_Texture * fog_l = SDL_CreateTextureFromSurface(window_p.getRenderer(), _fogSurface);

	// display fog in game
	SDL_Rect cutFog_l;
	cutFog_l.x = to_int(window_p.getWorldVector(0, 0).x);
	cutFog_l.y = to_int(window_p.getWorldVector(0, 0).y);
	cutFog_l.w = to_int((window_p.getWindowSize().x+2.));
	cutFog_l.h = to_int((window_p.getWindowSize().y+2.));
	SDL_Rect posFog_l;
	// position of the fog on screen
	octopus::Vector vec_l = window_p.getPixelVector(
		cutFog_l.x - to_double(window_p.getWorldVector(0, 0).x),
		cutFog_l.y - to_double(window_p.getWorldVector(0, 0).y));
	posFog_l.x = to_int(vec_l.x);
	posFog_l.y = to_int(vec_l.y);
	posFog_l.w = to_int(window_p.getPixelVector(cutFog_l.w, cutFog_l.h).x);
	posFog_l.h = to_int(window_p.getPixelVector(cutFog_l.w, cutFog_l.h).y);

	SDL_RenderCopy( window_p.getRenderer(), fog_l, &cutFog_l, &posFog_l );

	_cadre->render(window_p.getRenderer(), _x-2, _y-2, _w+4, _h+4, nullptr);
	_background.render(window_p.getRenderer(), _x, _y, _w, _h, nullptr);

	octopus::Player const * player_l = state_p.getPlayer(world_p.getPlayer());

	for(SpriteEntity const *sprite_l : world_p.getListSprite())
	{
		octopus::Entity const &ent_l = *state_p.getEntity(sprite_l->getHandle());

		// skip unit out of range
		if(ent_l._model._isUnit
		&& !state_p.getVisionHandler().isVisible(player_l->_team, ent_l))
		{
			continue;
		}

		double x_l = _x + to_double(ent_l._pos.x)*_w/state_p.getWorldSize();
		double y_l = _y + to_double(ent_l._pos.y)*_h/state_p.getWorldSize();

		double w_l = std::max(1., ent_l._model._ray*_w/state_p.getWorldSize());
		double h_l = std::max(1., ent_l._model._ray*_h/state_p.getWorldSize());

		window_p.loadTexture(_vecPlayerToTexture.at(ent_l._player))->render(window_p.getRenderer(), x_l, y_l, w_l, h_l, nullptr);
	}


	//Render texture to screen
	SDL_Rect rect_l {_x, _y, _w, _h};
	SDL_RenderCopy( window_p.getRenderer(), fog_l, NULL, &rect_l );

	SDL_DestroyTexture( fog_l );

	// camera rendering
	SDL_Rect cam_l;
	cam_l.x = _x + to_double(window_p.getWorldVector(0, 0).x)*_w/state_p.getWorldSize();
	cam_l.y = _y + to_double(window_p.getWorldVector(0, 0).y)*_h/state_p.getWorldSize();
	cam_l.w = to_double(window_p.getWindowSize().x)*_w/state_p.getWorldSize();
	cam_l.h = to_double(window_p.getWindowSize().y)*_h/state_p.getWorldSize();
	SDL_SetRenderDrawColor(window_p.getRenderer(), 255, 255, 255, 255);
	SDL_RenderDrawRect(window_p.getRenderer(), &cam_l);
}

bool Minimap::isInside(int x, int y) const
{
	return x >= _x
		&& y >= _y
		&& x <= _x+_w
		&& y <= _y+_h;
}

octopus::Vector Minimap::getRatioInside(int x, int y) const
{
	return { (x - _x) / double(_w), (y -_y) / double(_h) };
}

octopus::Vector getCameraPosition(int x, int y, Minimap const &minimap_p, Window const &window_p, unsigned long worldSize_p)
{
	octopus::Vector pos_l = getPosition(x, y, minimap_p, window_p, worldSize_p) - window_p.getWindowSize()/2.;
	return window_p.getPixelVector(to_double(pos_l.x), to_double(pos_l.y));
}

octopus::Vector getPosition(int x, int y, Minimap const &minimap_p, Window const &window_p, unsigned long worldSize_p)
{
	octopus::Vector ratio_l = minimap_p.getRatioInside(x, y);
	return ratio_l * worldSize_p;
}

} // cuttlefish
