#include "Dialog.hh"

namespace cuttlefish
{

Dialog::Dialog(Window* window_p, int x, int y, Picture const & background_p,
    std::string const &name_p, std::string const &text_p, Picture const & icon_p)
    : _background(background_p)
    , _textName(window_p, {0,0,0}, x+10, y+10)
    , _textDescription(window_p, x+80, y+35, 610)
    , _icon(icon_p)
{
    _background.setDestination(x, y, 700, 180);
    _icon.setDestination(x+10, y+35, 64, 64);

    _textName.setText(name_p);
    _textDescription.setText(text_p, {0,0,0});
}

Dialog::~Dialog()
{}

void Dialog::display(Window &window_p)
{
    _background.display(window_p);
    _textName.display(window_p);
    _textDescription.display(window_p);
    _icon.display(window_p);
}
} // namespace cuttlefish
