#ifndef __Text__
#define __Text__

#include <string>
#include <sstream>

#include <SDL2/SDL.h>

namespace cuttlefish
{
	class Window;

	class Text
	{
	public:
		Text(Window* window_p, SDL_Color const &color_p, int x, int y);
		~Text();

		void setText(std::string const & text_p);

		void display();

	protected:
		void clean();

		int _x;
		int _y;
		SDL_Color const _color;
		Window * const _window;

		std::string _text;

		SDL_Surface *_surface {nullptr};
		SDL_Texture *_texture {nullptr};
	};

	template<typename T>
	std::string stringify(T const &t)
	{
		std::stringstream ss_l;
		ss_l<<t;
		return ss_l.str();
	}
} // namespace cuttlefish


#endif
