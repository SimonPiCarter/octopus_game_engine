#include "Sprite.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "texture/Texture.hh"
#include "window/Window.hh"

namespace cuttlefish
{

Sprite::Sprite(Texture const * texture_p, double scale_p, int logX_p, int logY_p, int width_p, int height_p,
	std::vector<int> const &nbFramesPerState_p, std::vector<double> const &timePerFramePerState_p)
	: _texture(texture_p)
	, _scale(scale_p)
	, _logicalX(logX_p)
	, _logicalY(logY_p)
	, _width(width_p)
	, _height(height_p)
	, _nbFramesPerState(nbFramesPerState_p)
	, _timePerFramePerState(timePerFramePerState_p)
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

void Sprite::update(double elapsedTime_l)
{
	_timeIntoFrame += elapsedTime_l;

	while(_timeIntoFrame > _timePerFramePerState.at(_state))
	{
		_timeIntoFrame -= _timePerFramePerState.at(_state);
		_frame = (_frame + 1) % _nbFramesPerState.at(_state);
	}
}

void Sprite::render( Window &window_p )
{
	SDL_Rect final_l {
		int(_x - _logicalX * _scale),
		int(_y - _logicalY * _scale),
		int(_width * _scale),
		int(_height * _scale),
	};

	SDL_Rect clip_l {
		_frame * _width,
		_state * _height,
		_width,
		_height,
	};
	SDL_Point const &cam_l = window_p.getCamera();
	_texture->render(window_p.getRenderer(), final_l.x - cam_l.x, final_l.y - cam_l.y, final_l.h, final_l.w, &clip_l);
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



} // cuttlefish
