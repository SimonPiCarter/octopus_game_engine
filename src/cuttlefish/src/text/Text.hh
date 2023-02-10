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
		std::string getText() const { return _text; }

		void setColor(SDL_Color const &color_p);

		void setPosition(int x, int y);

		void display(Window & window_p) const;

		int getWidth() const { return _surface?_surface->w:0; }
		int getHeight() const { return _surface?_surface->h:0; }

	protected:
		void clean();
		void generate();

		int _x;
		int _y;
		SDL_Color _color;
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
