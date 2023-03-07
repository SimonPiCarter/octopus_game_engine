#include "Picture.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "texture/Texture.hh"
#include "window/Window.hh"

namespace cuttlefish
{

Picture::Picture(Texture const * texture_p, int width_p, int height_p, std::vector<int> const &nbFramesPerState_p, std::vector<double> const &timePerFramePerState_p)
	:  _texture(texture_p)
	, _width(width_p)
	, _height(height_p)
	, _nbFramesPerState(nbFramesPerState_p)
	, _timePerFramePerState(timePerFramePerState_p)
{
	_dest.w = _width;
	_dest.h = _height;
}

void Picture::setState(int state_p)
{
	if(state_p != _state)
	{
		_timeIntoFrame = 0.;
		_frame = 0;
	}
	_state = state_p;
}

void Picture::setStateNoReset(int state_p)
{
	_state = state_p;
}

void Picture::queueState(int state_p)
{
	_hasStateQueuedUp = true;
	_stateNext = state_p;
}

bool Picture::hasStateQueued() const
{
	return _hasStateQueuedUp;
}

void Picture::setFrame(int frame_p)
{
	_frame = frame_p;
}

void Picture::update(double elapsedTime_p)
{
	_timeIntoFrame += elapsedTime_p;
	_frame = _frame % _nbFramesPerState.at(_state);

	while(_timeIntoFrame > _timePerFramePerState.at(_state))
	{
		_timeIntoFrame -= _timePerFramePerState.at(_state);
		_frame = (_frame + 1) % _nbFramesPerState.at(_state);
		if(_frame == 0 && _hasStateQueuedUp)
		{
			_hasStateQueuedUp = false;
			_state = _stateNext;
			if(_endAfterLastFrame)
			{
				_ended = true;
			}
		}
	}
}

void Picture::display(Window &window_p)
{
	if(!_ended)
	{
		SDL_Rect clip_l(this->getClip());
		_texture->render(window_p.getRenderer(), _dest.x, _dest.y, _dest.h, _dest.w, &clip_l);
	}
}

bool Picture::isInside(Window const &window_p, int x, int y) const
{
	if(x >= _dest.x && x <= _dest.x + _dest.w
	&& y >= _dest.y && y <= _dest.y + _dest.h)
	{
		return true;
	}
	return false;
}

bool Picture::intersect(int lx, int ly, int ux, int uy) const
{
	int lowerX_l = std::min(lx, ux);
	int upperX_l = std::max(lx, ux);
	int lowerY_l = std::min(ly, uy);
	int upperY_l = std::max(ly, uy);

	int lowerIntersectX_l = std::max(lowerX_l, _dest.x);
	int upperIntersectX_l = std::min(upperX_l, _dest.x+_dest.w);

	int lowerIntersectY_l = std::max(lowerY_l, _dest.y);
	int upperIntersectY_l = std::min(upperY_l, _dest.y+_dest.h);

	if(lowerIntersectX_l <= upperIntersectX_l
	&& lowerIntersectY_l <= upperIntersectY_l)
	{
		return true;
	}
	return false;
}

void Picture::setDestination(int x, int y, int w, int h)
{
	_dest.x = x;
	_dest.y = y;
	_dest.w = w;
	_dest.h = h;
}

SDL_Rect const & Picture::getDestination() const
{
	return _dest;
}

SDL_Rect Picture::getClip() const
{
	SDL_Rect clip_l {
		_frame * _width,
		_state * _height,
		_width,
		_height,
	};

	return clip_l;
}

int Picture::getWidth() const
{
	return _dest.w;
}

int Picture::getHeight() const
{
	return _dest.h;
}
int Picture::getTextureAtomicWidth() const
{
	return _width;
}
int Picture::getTextureAtomicHeight() const
{
	return _height;
}
void Picture::setEndAfterLastFrame(bool b)
{
	_endAfterLastFrame = b;
}
bool Picture::isEndAfterLastFrame() const
{
	return _endAfterLastFrame;
}
bool Picture::isEnded() const
{
	return _ended;
}

} // cuttlefish
