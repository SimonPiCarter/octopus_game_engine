#ifndef __minimap__
#define __minimap__

#include <vector>
#include "texture/Texture.hh"
#include "texture/RenderTexture.hh"

#include "utils/Vector.hh"

struct SDL_Surface;

namespace octopus
{
class State;
} // namespace octopus


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
	Minimap(Window &window_p, int x, int y, int w, int h, std::vector<std::string> const &vec_p);
	~Minimap();

	/// @brief generate texture base on tile map
	/// @note must be called after tilemap generation
	void generate(Window &window_p, Tilemap &tilemap_p, unsigned long worldSize_p);

	void render(octopus::State const &state_p, World const &world_p, Window &window_p);

	/// @brief check if the coordinate is inside the minimap
	bool isInside(int x, int y) const;

	/// @brief return the ratio position inside the minimap
	octopus::Vector getRatioInside(int x, int y) const;

private:
	int const _x;
	int const _y;
	int const _w;
	int const _h;

	/// @brief cadre around the minimap
	Texture const * _cadre;

	/// @brief the texture of the background (rendered once)
	RenderTexture _background;

	/// @brief a correspondance map between players and texture (for dynamic info)
	std::vector<std::string> const _vecPlayerToTexture;

	///
	/// Fog of war
	///
	SDL_Surface * _fogSurface {nullptr};
};

/// @brief get camera position based on the mouse position and the minimap
octopus::Vector getCameraPosition(int x, int y, Minimap const &minimap_p, Window const &window_p, unsigned long worldSize_p);

/// @brief get world position based on the mouse position and the minimap
octopus::Vector getPosition(int x, int y, Minimap const &minimap_p, Window const &window_p, unsigned long worldSize_p);

} // namespace cuttlefish


#endif
