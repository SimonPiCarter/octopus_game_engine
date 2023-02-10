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
		/// @brief add a text in the segmented text
		/// @param key_p the key for the text to add (used to update text)
		/// @param text_p the text to add
		/// @param color_p the color of the text
		/// @param newLine_p set to true to end the line after this text
		void addText(std::string const &key_p, std::string const & text_p, SDL_Color const &color_p, bool newLine_p);

		/// @brief update text and refresh segmented text positions
		void updateText(std::string const &key_p, std::string const & text_p);
		/// @brief update text color
		void updateColor(std::string const &key_p, SDL_Color const &color_p);

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
