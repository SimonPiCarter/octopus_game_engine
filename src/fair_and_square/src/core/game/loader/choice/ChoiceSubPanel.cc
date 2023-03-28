#include "ChoiceSubPanel.hh"

#include "text/WrappedText.hh"
#include "text/Text.hh"
#include "window/Window.hh"

namespace fas
{

ChoiceSubPanel::ChoiceSubPanel(cuttlefish::Window &window_p, int x, int y, int w, cuttlefish::Picture const &background_p, cuttlefish::Texture const *icons_p)
    : _window(window_p), _x(x), _y(y), _w(w)
    , _background(background_p)
    , _icons(icons_p)
{}

ChoiceSubPanel::~ChoiceSubPanel()
{
    delete _titleText;
    delete _descriptionText;
    delete _statIcons;
    delete _statText;
    for(cuttlefish::Picture * pic_l : _mainIcons)
    {
        delete pic_l;
    }
}


void ChoiceSubPanel::update(BuffOption const &option_p)
{
    delete _titleText;
    delete _descriptionText;
    delete _statIcons;
    delete _statText;
    for(cuttlefish::Picture * pic_l : _mainIcons)
    {
        delete pic_l;
    }

    if(option_p._model == "square")
    {
        _mainIcons.push_back(new cuttlefish::Picture(_icons, 64, 64, {1}, {1}));
        _mainIcons.back()->setState(1);
        _mainIcons.back()->setFrame(4);
    }
    else if(option_p._model == "circle")
    {
        _mainIcons.push_back(new cuttlefish::Picture(_icons, 64, 64, {1}, {1}));
        _mainIcons.back()->setState(1);
        _mainIcons.back()->setFrame(3);
    }
    else if(option_p._model == "triangle")
    {
        _mainIcons.push_back(new cuttlefish::Picture(_icons, 64, 64, {1}, {1}));
        _mainIcons.back()->setState(2);
        _mainIcons.back()->setFrame(3);
    }

    size_t count_l = 0;
    for(cuttlefish::Picture * pic_l : _mainIcons)
    {
        pic_l->setDestination(_x+ (count_l+1) * (double(_w)/(_mainIcons.size()+1))-32, _y+10, 64, 64);
        ++count_l;
    }

    int offset_l = 0;
    if(!_mainIcons.empty())
    {
        offset_l += 64;
    }

    _descriptionText = new cuttlefish::WrappedText(&_window, _x+10, _y+offset_l+10, _w-20);
    _descriptionText->setText("Option $$ type : $type_val $$ offset : $offset_val $$ coef : $coef_val $$ model : $model_val", {0,0,0});
    _descriptionText->updateText("type_val", octopus::to_string(option_p._buff._type));
    _descriptionText->updateText("offset_val", std::to_string(int(option_p._buff._offset)));
    _descriptionText->updateText("coef_val", std::to_string(int(option_p._buff._coef*100.))+"%");
    _descriptionText->updateText("model_val", option_p._model);
}

void ChoiceSubPanel::display(cuttlefish::Window &window_p)
{
    _background.display(window_p);
    for(cuttlefish::Picture * pic_l : _mainIcons)
    {
        pic_l->display(window_p);
    }
    // _titleText->display(window_p);
    _descriptionText->display(window_p);
    // _statIcons->display(window_p);
    // _statText->display(window_p);
}

cuttlefish::Picture const &ChoiceSubPanel::getBackground() const
{
    return _background;
}


} // namespace fas
