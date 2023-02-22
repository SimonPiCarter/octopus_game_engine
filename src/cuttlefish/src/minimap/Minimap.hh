#ifndef __minimap__
#define __minimap__

#include <vector>
#include "texture/RenderTexture.hh"

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
	Minimap(Window &window_p, int x, int y, int w, int h, Tilemap &tilemap_p, unsigned long worldSize_p, std::vector<std::string> const &vec_p);

	void render(octopus::State const &state_p, World const &world_p, Window &window_p);

private:
	int const _x;
	int const _y;
	int const _w;
	int const _h;

	/// @brief the texture of the background (rendered once)
	RenderTexture _background;

	/// @brief a correspondance map between teams and texture (for dynamic info)
	std::vector<std::string> const _vecTeamToTexture;
};

} // namespace cuttlefish


#endif
