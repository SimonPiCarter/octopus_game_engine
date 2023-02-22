#include "Minimap.hh"

#include "window/Window.hh"
#include "tilemap/Tilemap.hh"

namespace cuttlefish
{

Minimap::Minimap(Window &window_p, int x, int y, int w, int h, Tilemap &tilemap_p, std::unordered_map<std::string, std::string> const &map_p)
	: _x(x), _y(y), _w(w), _h(h), _mapModelToTexture(map_p)
{
	int resolution_l = 1024;
	_background.createBlank(resolution_l, resolution_l, window_p.getRenderer());

	_background.startRendering(window_p.getRenderer());

	tilemap_p.renderScaled(window_p, resolution_l/(50.*64.), resolution_l/(50.*64.));

	_background.stopRendering(window_p.getRenderer());
}

void Minimap::render(World const &world_p, Window &window_p)
{
	_background.render(window_p.getRenderer(), _x, _y, _w, _h, nullptr);
}

} // cuttlefish
