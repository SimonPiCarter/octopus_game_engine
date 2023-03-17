#include "OptionPanel.hh"


namespace cuttlefish
{

OptionPanel::OptionPanel(int x, int y, Texture const * background_p)
    : _x(x), _y(y), _background(new Picture(background_p, 400, 400, {1}, {1}))
{
	_background->setDestination(x, y, 200., 200.);
}

OptionPanel::~OptionPanel()
{
	delete _background;
}

Picture const * OptionPanel::getBackground() const
{
    return _background;
}


}