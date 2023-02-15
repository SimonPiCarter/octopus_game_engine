#include "Sprite.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "texture/Texture.hh"
#include "window/Window.hh"

namespace cuttlefish
{

Sprite::Sprite(octopus::Handle const &ent_p, Texture const * texture_p, double scale_p, int logX_p, int logY_p,
	int width_p, int height_p, std::vector<int> const &nbFramesPerState_p, std::vector<double> const &timePerFramePerState_p, bool absolute_p)
	: _handle(ent_p)
	, _texture(texture_p)
	, _scale(scale_p)
	, _logicalX(logX_p * (64./width_p))
	, _logicalY(logY_p * (64./height_p))
	, _width(width_p)
	, _height(height_p)
	, _nbFramesPerState(nbFramesPerState_p)
	, _timePerFramePerState(timePerFramePerState_p)
	, _absolute(absolute_p)
{}

void Sprite::setState(int state_p)
{
	if(state_p != _state)
	{
		_timeIntoFrame = 0.;
		_frame = 0;
	}
	_state = state_p;
}

void Sprite::setStateNoReset(int state_p)
{
	_state = state_p;
}

void Sprite::setFrame(int frame_p)
{
	_frame = frame_p;
}

void Sprite::update(double elapsedTime_l)
{
	_timeIntoFrame += elapsedTime_l;

	while(_timeIntoFrame > _timePerFramePerState.at(_state))
	{
		_timeIntoFrame -= _timePerFramePerState.at(_state);
		_frame = (_frame + 1) % _nbFramesPerState.at(_state);
	}
}

void Sprite::render(Window &window_p)
{
	SDL_Rect final_l {
		int(_x - _logicalX * _scale),
		int(_y - _logicalY * _scale),
		int(64 * _scale),
		int(64 * _scale),
	};

	SDL_Rect clip_l {
		_frame * _width,
		_state * _height,
		_width,
		_height,
	};
	SDL_Point const &cam_l = window_p.getCamera();
	if(_absolute)
	{
		_texture->render(window_p.getRenderer(), final_l.x, final_l.y, final_l.h, final_l.w, &clip_l);
	}
	else
	{
		_texture->render(window_p.getRenderer(), final_l.x - cam_l.x, final_l.y - cam_l.y, final_l.h, final_l.w, &clip_l);
	}
}

bool Sprite::isInside(Window const &window_p, int x, int y) const
{
	// rectangle of drawing
	SDL_Rect final_l {
		int(_x - _logicalX * _scale),
		int(_y - _logicalY * _scale),
		int(64. * _scale),
		int(64. * _scale),
	};

	if(x >= final_l.x && x <= final_l.x + final_l.w
	&& y >= final_l.y && y <= final_l.y + final_l.h
	&& _absolute)
	{
		return true;
	}

	SDL_Point const &cam_l = window_p.getCamera();
	if(x >= final_l.x - cam_l.x && x <= final_l.x - cam_l.x + final_l.w
	&& y >= final_l.y - cam_l.y && y <= final_l.y - cam_l.y + final_l.h
	&& !_absolute)
	{
		return true;
	}
	return false;
}

bool Sprite::intersect(int lx, int ly, int ux, int uy) const
{
	// rectangle of drawing
	SDL_Rect final_l {
		int(_x - _logicalX * _scale),
		int(_y - _logicalY * _scale),
		int(64. * _scale),
		int(64. * _scale),
	};

	int lowerX_l = std::min(lx, ux);
	int upperX_l = std::max(lx, ux);
	int lowerY_l = std::min(ly, uy);
	int upperY_l = std::max(ly, uy);

	int lowerIntersectX_l = std::max(lowerX_l, final_l.x);
	int upperIntersectX_l = std::min(upperX_l, final_l.x+final_l.w);

	int lowerIntersectY_l = std::max(lowerY_l, final_l.y);
	int upperIntersectY_l = std::min(upperY_l, final_l.y+final_l.h);

	if(lowerIntersectX_l <= upperIntersectX_l
	&& lowerIntersectY_l <= upperIntersectY_l)
	{
		return true;
	}
	return false;
}

double Sprite::getWidth() const
{
	return 64. * _scale;
}

double Sprite::getHeight() const
{
	return 64. * _scale;
}

void Sprite::setPosition(double x, double y)
{
	_x = x;
	_y = y;
}

void Sprite::move(double dx, double dy)
{
	_x += dx;
	_y += dy;
}

double Sprite::getX() const
{
	return _x;
}

double Sprite::getY() const
{
	return _y;
}

octopus::Handle const & Sprite::getHandle() const
{
	return _handle;
}


} // cuttlefish
