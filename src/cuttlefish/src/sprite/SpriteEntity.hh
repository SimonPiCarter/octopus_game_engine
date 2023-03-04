#ifndef __SpriteEntity__
#define __SpriteEntity__

#include "state/Handle.hh"
#include "Sprite.hh"

namespace cuttlefish
{

/// @brief sprite used to render an entity in the world based on  Sprite
class SpriteEntity : public Sprite
{
	public:
		SpriteEntity(octopus::Handle const &ent_p, Texture const * texture_p, double scale_p, int logX_p, int logY_p, int width_p, int height_p,
			std::vector<int> const &nbFramesPerState_p, std::vector<double> const &timePerFramePerState_p, bool absolute_p=false);

		octopus::Handle const & getHandle() const;
	private:
		/// @brief the handle of the entity displayed
		octopus::Handle const _handle;
};

} // namespace cuttlefish

#endif
