#ifndef __WrappedText__
#define __WrappedText__

#include <list>
#include <unordered_map>

#include "SegmentedText.hh"

namespace cuttlefish
{

	/// @brief this class allows to draw segmented text
	/// allowing new line
	class WrappedText
	{
	public:
		WrappedText(Window* window_p, int x, int y, int w);
		~WrappedText();

		/// @brief set the text of this wrapped text
		/// @note every $ in the text trigger an accessible text portion which can be updated
		/// every updatable text should be updated with mono word content if possible
		void setText(std::string const & text_p, SDL_Color const &color_p);

		/// @brief update text and refresh segmented text positions
		void updateText(std::string const &key_p, std::string const & text_p);
		/// @brief update color
		void updateColor(std::string const &key_p, SDL_Color const &color_p);

		/// @brief refresh text positions based on updated text
		void refresh();

		void display(Window & window_p) const;

		void clean();
	protected:

		Window * const _window;
		int _x;
		int _y;
		int _w;

		int _cursorX;
		int _cursorY;

		/// @brief map to access texts
		std::unordered_map<std::string, Text *> _mapText;
		std::list<Text *> _segments;
	};

} // namespace cuttlefish


#endif
