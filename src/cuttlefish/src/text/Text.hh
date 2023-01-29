#ifndef __Text__
#define __Text__

#include <string>

class SDL_Color;

namespace cuttlefish
{
	class Window;
	void displayText(Window* window_p, std::string const &text_p, SDL_Color const &color_p, int x, int y);
} // namespace cuttlefish


#endif
