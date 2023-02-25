#include "Menu.hh"

#include <SDL2/SDL.h>

#include "sprite/Picture.hh"
#include "text/Text.hh"


namespace cuttlefish
{

Menu::Menu(int x, int y, int spacing_p)
    : _spacing(spacing_p), _x(x), _y(y)
{}

Menu::~Menu()
{
    for(Button &button_l : _buttons)
    {
        delete button_l.picture;
        delete button_l.text;
    }
}

void Menu::addButton(Picture *picture_p, Text *text_p, std::function<void(void)> callback_p)
{
    // set position
    if(_buttons.empty())
    {
        picture_p->setPosition(_x, _y);
    }
    else
    {
        picture_p->setPosition(_x, _buttons.back().picture->getY() + _buttons.back().picture->getHeight() + double(_spacing));
    }
    text_p->setPosition(_x - text_p->getWidth()/2, picture_p->getY() - text_p->getHeight()/2);

    _buttons.push_back(Button {picture_p, text_p, callback_p});
}

void Menu::handleEvent(Window &window_p, SDL_Event const &e)
{
    if( e.type == SDL_MOUSEMOTION )
    {
        // if one button is clicked do not update on mouse motion
        if(_clicked)
        {
            return;
        }
        for(Button &button_l : _buttons)
        {
            if(button_l.picture->isInside(window_p, e.button.x, e.button.y))
            {
                button_l.picture->setState(1);
            }
            else
            {
                button_l.picture->setState(0);
            }
        }
    }
    else if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        for(Button &button_l : _buttons)
        {
            if(button_l.picture->isInside(window_p, e.button.x, e.button.y))
            {
                _clicked = button_l.picture;
                _clicked->setState(2);
            }
        }
    }
    else if (e.type == SDL_MOUSEBUTTONUP)
    {
        for(Button &button_l : _buttons)
        {
            if(button_l.picture->isInside(window_p, e.button.x, e.button.y)
            && _clicked == button_l.picture)
            {
                button_l.callback();
            }
            button_l.picture->setState(0);
        }
        _clicked = nullptr;
    }
}

void Menu::display(double elapsedTime_p, Window &window_p)
{
    for(Button &button_l : _buttons)
    {
        button_l.picture->update(elapsedTime_p);
        button_l.picture->render(window_p);
        button_l.text->display(window_p);
    }
}

} // cuttlefish