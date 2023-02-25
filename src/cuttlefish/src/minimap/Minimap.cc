#include "Minimap.hh"

#include "window/Window.hh"
#include "world/World.hh"
#include "sprite/Sprite.hh"
#include "texture/Texture.hh"
#include "tilemap/Tilemap.hh"

#include "state/State.hh"
#include "state/entity/Entity.hh"

namespace cuttlefish
{

Minimap::Minimap(Window &window_p, int x, int y, int w, int h,
				Tilemap &tilemap_p, unsigned long worldSize_p, std::vector<std::string> const &map_p)
	: _x(x), _y(y), _w(w), _h(h), _vecPlayerToTexture(map_p)
{
	_cadre = window_p.loadTexture("resources/background.png");

	int resolution_l = 1024;
	_background.createBlank(resolution_l, resolution_l, window_p.getRenderer());

	_background.startRendering(window_p.getRenderer());

	tilemap_p.renderScaled(window_p, resolution_l/(worldSize_p*32.), resolution_l/(worldSize_p*32.));

	_background.stopRendering(window_p.getRenderer());

	_fogSurface = SDL_CreateRGBSurfaceWithFormat(0, worldSize_p, worldSize_p, 32, SDL_PIXELFORMAT_RGBA8888);
}

Minimap::~Minimap()
{
	SDL_FreeSurface(_fogSurface);
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
	cutFog_l.x = std::floor(window_p.getWorldVector(0, 0).x);
	cutFog_l.y = std::floor(window_p.getWorldVector(0, 0).y);
	cutFog_l.w = std::ceil(window_p.getWindowSize().x+1.);
	cutFog_l.h = std::ceil(window_p.getWindowSize().y+1.);
	SDL_Rect posFog_l;
	posFog_l.x = window_p.getPixelVector(cutFog_l.x - window_p.getWorldVector(0, 0).x, cutFog_l.y - window_p.getWorldVector(0, 0).y).x;
	posFog_l.y = window_p.getPixelVector(cutFog_l.x - window_p.getWorldVector(0, 0).x, cutFog_l.y - window_p.getWorldVector(0, 0).y).y;
	posFog_l.w = window_p.getPixelVector(cutFog_l.w, cutFog_l.h).x;
	posFog_l.h = window_p.getPixelVector(cutFog_l.w, cutFog_l.h).y;

	SDL_RenderCopy( window_p.getRenderer(), fog_l, &cutFog_l, &posFog_l );

	_cadre->render(window_p.getRenderer(), _x-2, _y-2, _w+4, _h+4, nullptr);
	_background.render(window_p.getRenderer(), _x, _y, _w, _h, nullptr);

	for(Sprite const *sprite_l : world_p.getListSprite())
	{
		octopus::Entity const *ent_l = state_p.getEntity(sprite_l->getHandle());

		double x_l = _x + ent_l->_pos.x*_w/state_p.getWorldSize();
		double y_l = _y + ent_l->_pos.y*_h/state_p.getWorldSize();

		double w_l = std::max(1., ent_l->_model._ray*_w/state_p.getWorldSize());
		double h_l = std::max(1., ent_l->_model._ray*_h/state_p.getWorldSize());

		window_p.loadTexture(_vecPlayerToTexture.at(ent_l->_player))->render(window_p.getRenderer(), x_l, y_l, w_l, h_l, nullptr);
	}


	//Render texture to screen
	SDL_Rect rect_l {_x, _y, _w, _h};
	SDL_RenderCopy( window_p.getRenderer(), fog_l, NULL, &rect_l );

	SDL_DestroyTexture( fog_l );

	// camera rendering
	SDL_Rect cam_l;
	cam_l.x = _x + window_p.getWorldVector(0, 0).x*_w/state_p.getWorldSize();
	cam_l.y = _y + window_p.getWorldVector(0, 0).y*_h/state_p.getWorldSize();
	cam_l.w = window_p.getWindowSize().x*_w/state_p.getWorldSize();
	cam_l.h = window_p.getWindowSize().y*_h/state_p.getWorldSize();
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
	octopus::Vector ratio_l = minimap_p.getRatioInside(x, y);

	octopus::Vector pos_l = window_p.getPixelVector(ratio_l.x * worldSize_p - window_p.getWindowSize().x/2, ratio_l.y * worldSize_p - window_p.getWindowSize().y/2);

	return pos_l;
}

} // cuttlefish