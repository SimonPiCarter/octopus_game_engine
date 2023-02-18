#include "WrappedText.hh"

#include <algorithm>
#include <iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "window/Window.hh"
#include "logger/Logger.hh"

namespace cuttlefish
{

WrappedText::WrappedText(Window* window_p, int x, int y, int w)
	: _window(window_p)
	, _x(x)
	, _y(y)
	, _w(w)
	, _cursorX(x)
	, _cursorY(y)
{}

WrappedText::~WrappedText()
{
	clean();
}

template <typename Out>
void split(const std::string &s, char delim, Out result) {
	std::istringstream iss(s);
	std::string item;
	while (std::getline(iss, item, delim)) {
		*result++ = item;
	}
}

void WrappedText::setText(std::string const & text_p, SDL_Color const &color_p)
{
	if(text_p == _oldText)
	{
		return;
	}
	_oldText = text_p;
	clean();
	// split text
	std::vector<std::string> words_l;
	split(text_p, ' ', std::back_inserter(words_l));

	// for every word
	for(std::string word_l : words_l)
	{
		// if new line
		if(word_l.find("$$") != std::string::npos)
		{
			_segments.emplace_back(nullptr);
		}
		// if updatable text
		if(word_l.find("$") != std::string::npos)
		{
			word_l.erase(std::remove(word_l.begin(), word_l.end(), '$'), word_l.end());

			Text * text_l = new Text(_window, color_p, _x, _y);
			text_l->setText("");

			_mapText[word_l] = text_l;
			_segments.emplace_back(text_l);
		}
		// else just generate text
		else
		{
			Text * text_l = new Text(_window, color_p, _x, _y);
			text_l->setText(word_l);

			_segments.emplace_back(text_l);
		}
	}

	refresh();
}

void WrappedText::updateText(std::string const &key_p, std::string const & text_p)
{
	_mapText[key_p]->setText(text_p);
	refresh();
}

void WrappedText::updateColor(std::string const &key_p, SDL_Color const &color_p)
{
	_mapText[key_p]->setColor(color_p);
}

void WrappedText::refresh()
{
	_cursorX = _x;
	_cursorY = _y;

	bool first_l = true;
	// remember last height to enable new line with no text ($$)
	int lastHeight = 0;

	for(Text *text_l : _segments)
	{
		if(text_l)
		{
			text_l->setPosition(_cursorX, _cursorY);

			// new line if too wide
			// if first do not skip new line
			if(_cursorX + text_l->getWidth() - _x > _w && !first_l)
			{
				// update cursor to start of new line
				_cursorY += text_l->getHeight();
				_cursorX = _x;
				// update position after update
				text_l->setPosition(_cursorX, _cursorY);
			}
			_cursorX += text_l->getWidth() + 10;

			first_l = false;
			// update height
			lastHeight = text_l->getHeight();
		}
		// new line
		else
		{
			// update cursor to start of new line
			_cursorY += lastHeight;
			_cursorX = _x;
			first_l = true;
		}
	}
}

void WrappedText::display(Window& window_p) const
{
	for(Text const *segment_l : _segments)
	{
		// new line segment are null : need to test
		if(segment_l)
		{
			segment_l->display(window_p);
		}
	}
}

void WrappedText::clean()
{
	for(Text const *segment_l : _segments)
	{
		delete segment_l;
	}
	_segments.clear();
	_mapText.clear();
}


} // namespace cuttlefish

