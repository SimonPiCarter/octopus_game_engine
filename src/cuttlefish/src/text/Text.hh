#ifndef __Text__
#define __Text__

#include <string>
#include <sstream>

class SDL_Color;

namespace cuttlefish
{
	class Window;

	void displayText(Window* window_p, std::string const &text_p, SDL_Color const &color_p, int x, int y);

	template<typename T>
	std::string stringify(T const &t)
	{
		std::stringstream ss_l;
		ss_l<<t;
		return ss_l.str();
	}
} // namespace cuttlefish


#endif
