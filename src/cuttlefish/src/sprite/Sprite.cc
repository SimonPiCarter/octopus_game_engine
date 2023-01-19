#include "Sprite.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "texture/Texture.hh"

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
	_state = state_p;
	_timeIntoFrame = 0.;
	_frame = 0;
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

void Sprite::render( SDL_Renderer* renderer_p, int x, int y )
{
	SDL_Rect final_l {
		int(x - _logicalX * _scale),
		int(y - _logicalY * _scale),
		int(_width * _scale),
		int(_height * _scale),
	};

	SDL_Rect clip_l {
		_frame * _width,
		_state * _height,
		_width,
		_height,
	};

	_texture->render(renderer_p, final_l.x, final_l.y, final_l.h, final_l.w, &clip_l);
}

} // cuttlefish
