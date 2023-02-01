#include "SpriteLibrary.hh"

#include "sprite/Sprite.hh"

namespace cuttlefish
{
void SpriteLibrary::registerSpriteTemplate(std::string const &id_p, Texture const * texture_p, double scale_p, int logX_p, int logY_p, int width_p, int height_p,
	std::vector<int> const &nbFramesPerState_p, std::vector<double> const &timePerFramePerState_p, int blueprintState_p)
{
	_mapTemplates[id_p].texture = texture_p;
	_mapTemplates[id_p].scale = scale_p;
	_mapTemplates[id_p].logX = logX_p;
	_mapTemplates[id_p].logY = logY_p;
	_mapTemplates[id_p].width = width_p;
	_mapTemplates[id_p].height = height_p;
	_mapTemplates[id_p].nbFramesPerState = nbFramesPerState_p;
	_mapTemplates[id_p].timePerFramePerState = timePerFramePerState_p;
	_mapTemplates[id_p].blueprint = blueprintState_p;
}

Sprite * SpriteLibrary::createSprite(octopus::Handle const &handle_p, std::string const &id_p, bool absolute_p) const
{
	return new Sprite(handle_p,
			_mapTemplates.at(id_p).texture,
			_mapTemplates.at(id_p).scale,
			_mapTemplates.at(id_p).logX,
			_mapTemplates.at(id_p).logY,
			_mapTemplates.at(id_p).width,
			_mapTemplates.at(id_p).height,
			_mapTemplates.at(id_p).nbFramesPerState,
			_mapTemplates.at(id_p).timePerFramePerState,
			absolute_p
		);
}

int SpriteLibrary::getBlueprintState(std::string const &id_p) const
{
	return _mapTemplates.at(id_p).blueprint;
}


} // namespace cuttlefish
