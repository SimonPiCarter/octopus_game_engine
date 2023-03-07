#include "SpriteEntity.hh"

namespace cuttlefish
{

SpriteEntity::SpriteEntity(octopus::Handle const &ent_p, Texture const * texture_p, Picture *background_p, Picture *bar_p,
	double scale_p, int logX_p, int logY_p,
	int width_p, int height_p, std::vector<int> const &nbFramesPerState_p, std::vector<double> const &timePerFramePerState_p, bool absolute_p)
	: Sprite(texture_p, scale_p, logX_p, logY_p, width_p, height_p, nbFramesPerState_p, timePerFramePerState_p)
	, _handle(ent_p)
	, _lifeBar(nullptr)
{
	if(background_p && bar_p)
	{
		_lifeBar = new ProgressBar(background_p, bar_p, 64*scale_p-2, 6, 1);
		_lifeBar->setProgress(100);
	}
}

SpriteEntity::~SpriteEntity()
{
	delete _lifeBar;
}

octopus::Handle const & SpriteEntity::getHandle() const
{
	return _handle;
}

void SpriteEntity::renderLifeBar(Window &window_p, double elapsed_p)
{
	if(_lifeBar && _lifeBar->getProgress() != 100)
	{
		// center life bar
		_lifeBar->setPosition(_dest.x +_dest.w/2 - _lifeBar->getWidth()/2  , _dest.y-_lifeBar->getHeight()-1);
		_lifeBar->update(elapsed_p);
		_lifeBar->display(window_p);
	}
}

void SpriteEntity::setLifePercent(int percent_p)
{
	if(_lifeBar)
	{
		_lifeBar->setProgress(percent_p);
	}
}

bool SpriteEntity::hasDyingState() const
{
	return _hasDyingState;
}

void SpriteEntity::setDyingState(unsigned long state_p)
{
	_dyingState = state_p;
	_hasDyingState = true;
}

unsigned long SpriteEntity::getDyingState() const
{
	return _dyingState;
}


} // cuttlefish
