#include "SpriteLibrary.hh"

#include "sprite/Sprite.hh"
#include "sprite/SpriteEntity.hh"
#include "texture/Texture.hh"

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

void SpriteLibrary::registerAltTextureForTemplate(std::string const &id_p, unsigned long player_p, Texture const * texture_p)
{
	_mapTemplates[id_p].mapTexturePerPlayer[player_p] = texture_p;
}

void SpriteLibrary::addDyingState(std::string const &id_p, int state_p)
{
	_mapTemplates[id_p].dyingState = state_p;
}

Sprite * SpriteLibrary::createSprite(std::string const &id_p, bool absolute_p) const
{
	return new Sprite(
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

SpriteEntity * SpriteLibrary::createSpriteEntity(octopus::Handle const &handle_p, std::string const &id_p, bool hpBar_p, unsigned long player_p) const
{
	Texture const *texture_l = _mapTemplates.at(id_p).texture;
	if(_mapTemplates.at(id_p).mapTexturePerPlayer.find(player_p) != _mapTemplates.at(id_p).mapTexturePerPlayer.end()
	&& _mapTemplates.at(id_p).mapTexturePerPlayer.at(player_p))
	{
		texture_l = _mapTemplates.at(id_p).mapTexturePerPlayer.at(player_p);
	}
	SpriteEntity * sprite_l = new SpriteEntity(
			handle_p,
			texture_l,
			_hpBarBackground && hpBar_p?new Picture(_hpBarBackground, _hpBarFilling->getWidth(), _hpBarFilling->getHeight(), {1}, {1}):nullptr,			// optional pictures
			_hpBarFilling && hpBar_p?new Picture(_hpBarFilling, _hpBarFilling->getWidth(), _hpBarFilling->getHeight(), {1}, {1}):nullptr,				// optional pictures
			_mapTemplates.at(id_p).scale,
			_mapTemplates.at(id_p).logX,
			_mapTemplates.at(id_p).logY,
			_mapTemplates.at(id_p).width,
			_mapTemplates.at(id_p).height,
			_mapTemplates.at(id_p).nbFramesPerState,
			_mapTemplates.at(id_p).timePerFramePerState,
			false
		);

	if(_mapTemplates.at(id_p).dyingState >= 0)
	{
		sprite_l->setDyingState(_mapTemplates.at(id_p).dyingState);
	}
	return sprite_l;
}

SpriteTemplate const & SpriteLibrary::getSpriteTemplate(std::string const &id_p) const
{
	return _mapTemplates.at(id_p);
}

int SpriteLibrary::getBlueprintState(std::string const &id_p) const
{
	return _mapTemplates.at(id_p).blueprint;
}

void SpriteLibrary::setHpBarBackground(Texture const * texture_p)
{
	_hpBarBackground = texture_p;
}

void SpriteLibrary::setHpBarFilling(Texture const * texture_p)
{
	_hpBarFilling = texture_p;
}

} // namespace cuttlefish
