#ifndef __SegmentedText__
#define __SegmentedText__

#include <list>
#include <unordered_map>

#include "Text.hh"

namespace cuttlefish
{
	struct TextSegment
	{
		TextSegment() : _text(nullptr), _newline(false) {}
		TextSegment(Text * text_p, bool newline_p) : _text(text_p), _newline(newline_p) {}
		Text * _text;
		bool _newline;
	};

	/// @brief this class allows to draw segmented text
	/// allowing new line
	class SegmentedText
	{
	public:
		SegmentedText(Window* window_p, int x, int y);
		~SegmentedText();

		void addText(std::string const &key_p, std::string const & text_p, SDL_Color const &color_p, bool newLine_p);

		/// @brief update text and refresh segmented text positions
		void updateText(std::string const &key_p, std::string const & text_p);

		/// @brief refresh text positions based on updated text
		void refresh();

		void display(Window & window_p) const;

		void clean();
	protected:

		Window * const _window;
		int _x;
		int _y;

		int _cursorX;
		int _cursorY;

		/// @brief map to access texts
		std::unordered_map<std::string, Text *> _mapText;
		std::list<TextSegment> _segments;
	};

} // namespace cuttlefish


#endif
