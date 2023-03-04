#include "SpriteEntity.hh"

namespace cuttlefish
{

SpriteEntity::SpriteEntity(octopus::Handle const &ent_p, Texture const * texture_p, double scale_p, int logX_p, int logY_p,
	int width_p, int height_p, std::vector<int> const &nbFramesPerState_p, std::vector<double> const &timePerFramePerState_p, bool absolute_p)
	: Sprite(texture_p, scale_p, logX_p, logY_p, width_p, height_p, nbFramesPerState_p, timePerFramePerState_p)
	, _handle(ent_p)
{}

octopus::Handle const & SpriteEntity::getHandle() const
{
	return _handle;
}

} // cuttlefish
