
#include "ProgressBar.hh"
#include "sprite/Picture.hh"

namespace cuttlefish
{

ProgressBar::ProgressBar(Picture *background_p, Picture *bar_p, int w, int h, int margin_p)
    : _background(background_p), _bar(bar_p), _w(w), _h(h), _margin(margin_p), _maxW(w-2*margin_p)
{
    setPosition(0, 0);
}

ProgressBar::~ProgressBar()
{
    delete _background;
    delete _bar;
}

void ProgressBar::update(double elapsedTime_p)
{
    _background->update(elapsedTime_p);
    _bar->update(elapsedTime_p);
}

void ProgressBar::display( Window &window_p)
{
    _background->display(window_p);
    _bar->display(window_p);
}

void ProgressBar::setProgress(int progress_p)
{
    _progress = std::max(0, std::min(100, progress_p));
    _bar->setDestination(_bar->getDestination().x, _bar->getDestination().y, _maxW*_progress/100, _h-2*_margin);
}

void ProgressBar::setPosition(int x, int y)
{
    _background->setDestination(x, y, _w, _h);
    _bar->setDestination(x+_margin, y+_margin, _maxW*_progress/100, _h-2*_margin);
}


} // cuttlefish