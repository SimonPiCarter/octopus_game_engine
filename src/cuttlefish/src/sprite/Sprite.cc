#include "Sprite.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "texture/Texture.hh"
#include "window/Window.hh"

namespace cuttlefish
{

Sprite::Sprite(octopus::Handle const &ent_p, Texture const * texture_p, double scale_p, int logX_p, int logY_p,
	int width_p, int height_p, std::vector<int> const &nbFramesPerState_p, std::vector<double> const &timePerFramePerState_p, bool absolute_p)
	: Picture(texture_p, width_p, height_p, nbFramesPerState_p, timePerFramePerState_p)
	, _scale(scale_p)
	, _logicalX(logX_p * (64./width_p))
	, _logicalY(logY_p * (64./height_p))
	, _absolute(absolute_p)
	, _handle(ent_p)
{
	_dest.w = 64 * _scale;
	double ratio_l = double(_height)/double(_width);
	_dest.h = 64 * ratio_l * _scale;
}

octopus::Handle const & Sprite::getHandle() const
{
	return _handle;
}

void Sprite::setPosition(double x, double y)
{
	_x = x;
	_y = y;
}

double Sprite::getX() const
{
	return _x;
}

double Sprite::getY() const
{
	return _y;
}

void Sprite::render(Window &window_p)
{
	double ratio_l = double(getHeight())/double(getHeight());

	_dest.x = _x - _logicalX * _scale;
	_dest.y = _y - _logicalY * ratio_l * _scale;

	SDL_Rect clip_l {
		_frame * _width,
		_state * _height,
		_width,
		_height,
	};

	SDL_Point const &cam_l = window_p.getCamera();
	if(!_absolute)
	{
		_dest.x -= cam_l.x;
		_dest.y -= cam_l.y;
	}

	display(window_p);
}

void Sprite::renderScaled(Window &window_p, double scaleX_p, double scaleY_p)
{
	double ratio_l = double(_height)/double(_width);
	SDL_Rect final_l {
		int(std::floor((_x - _logicalX * _scale) * scaleX_p)),
		int(std::floor((_y - _logicalY * ratio_l * _scale) * scaleY_p)),
		int(std::ceil((64. * _scale) * scaleX_p)),
		int(std::ceil((64. * ratio_l * _scale) * scaleY_p)),
	};

	SDL_Rect clip_l {
		_frame * _width,
		_state * _height,
		_width,
		_height,
	};
	_texture->render(window_p.getRenderer(), final_l.x, final_l.y, final_l.h, final_l.w, &clip_l);
}

} // cuttlefish
