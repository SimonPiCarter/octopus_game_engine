#ifndef __minimap__
#define __minimap__

#include <unordered_map>
#include "texture/RenderTexture.hh"

namespace cuttlefish
{
class Tilemap;
class Window;
class World;

/// @brief This class allow to display
/// a minimap of the given size at the given
/// position
class Minimap
{
public:
	Minimap(Window &window_p, int x, int y, int w, int h, Tilemap &tilemap_p, std::unordered_map<std::string, std::string> const &map_p);

	void render(World const &world_p, Window &window_p);

private:
	int const _x;
	int const _y;
	int const _w;
	int const _h;

	/// @brief the texture of the background (rendered once)
	RenderTexture _background;

	/// @brief a correspondance map between models and texture (for dynamic info)
	std::unordered_map<std::string, std::string> const _mapModelToTexture;
};

} // namespace cuttlefish


#endif
