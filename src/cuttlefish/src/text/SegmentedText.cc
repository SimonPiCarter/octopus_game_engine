#include "SegmentedText.hh"

#include <iostream>

#include <SDL2/SDL.h>

#include "window/Window.hh"
#include "logger/Logger.hh"

namespace cuttlefish
{

SegmentedText::SegmentedText(Window* window_p, int x, int y)
	: _window(window_p)
	, _x(x)
	, _y(y)
	, _cursorX(x)
	, _cursorY(y)
{}

SegmentedText::~SegmentedText()
{
	clean();
}

void SegmentedText::addText(std::string const &key_p, std::string const & text_p, SDL_Color const &color_p, bool newLine_p)
{
	Text * text_l = new Text(_window, color_p, _cursorX, _cursorY);
	text_l->setText(text_p);
	if(newLine_p)
	{
		_cursorY += text_l->getHeight();
		_cursorX = _x;
	}
	else
	{
		_cursorX += text_l->getWidth();
	}
	_segments.emplace_back(text_l, newLine_p);
	_mapText[key_p] = text_l;
}

void SegmentedText::updateText(std::string const &key_p, std::string const & text_p)
{
	_mapText[key_p]->setText(text_p);
	refresh();
}

void SegmentedText::refresh()
{
	_cursorX = _x;
	_cursorY = _y;
	for(TextSegment &segment_l : _segments)
	{
		segment_l._text->setPosition(_cursorX, _cursorY);
		if(segment_l._newline)
		{
			_cursorY += segment_l._text->getHeight();
			_cursorX = _x;
		}
		else
		{
			_cursorX += segment_l._text->getWidth();
		}
	}
}

void SegmentedText::display(Window& window_p) const
{
	for(TextSegment const &segment_l : _segments)
	{
		segment_l._text->display(window_p);
	}
}

void SegmentedText::clean()
{
	for(TextSegment &segment_l : _segments)
	{
		delete segment_l._text;
	}
	_segments.clear();
	_mapText.clear();
}


} // namespace cuttlefish

