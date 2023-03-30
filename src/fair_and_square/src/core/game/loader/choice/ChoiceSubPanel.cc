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

void updateFromModel(cuttlefish::Picture *pic_p, std::string const &model_p)
{
    if(model_p == "square")
    {
        pic_p->setState(1);
        pic_p->setFrame(4);
    }
    else if(model_p == "circle")
    {
        pic_p->setState(1);
        pic_p->setFrame(3);
    }
    else if(model_p == "triangle")
    {
        pic_p->setState(2);
        pic_p->setFrame(3);
    }
    else if(model_p == "barrack")
    {
        pic_p->setState(0);
        pic_p->setFrame(0);

    }
    else if(model_p == "deposit")
    {
        pic_p->setState(0);
        pic_p->setFrame(1);
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
    _mainIcons.clear();

    _mainIcons.push_back(new cuttlefish::Picture(_icons, 64, 64, {1}, {1}));
    updateFromModel(_mainIcons.back(), option_p._model);

    updateIconsPosition();

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

void ChoiceSubPanel::update(ModifierOption const &option_p)
{
    delete _titleText;
    delete _descriptionText;
    delete _statIcons;
    delete _statText;
    for(cuttlefish::Picture * pic_l : _mainIcons)
    {
        delete pic_l;
    }
    _mainIcons.clear();

    _mainIcons.push_back(new cuttlefish::Picture(_icons, 64, 64, {1}, {1}));
    updateFromModel(_mainIcons.back(), option_p._model);

    std::visit([&](auto &&arg) { this->updateFromModifier(arg); }, option_p._mod);

    updateIconsPosition();
}

void ChoiceSubPanel::updateFromModifier(octopus::NoModifier const &mod_p)
{
    _mainIcons.push_back(new cuttlefish::Picture(_icons, 64, 64, {1}, {1}));
    _mainIcons.back()->setState(3);
    _mainIcons.back()->setFrame(0);

    _descriptionText = new cuttlefish::WrappedText(&_window, _x+10, _y+74, _w-20);
    _descriptionText->setText("NoModifier", {0,0,0});
}

void ChoiceSubPanel::updateFromModifier(octopus::AoEModifier const &mod_p)
{
    _mainIcons.push_back(new cuttlefish::Picture(_icons, 64, 64, {1}, {1}));
    _mainIcons.back()->setState(3);
    _mainIcons.back()->setFrame(1);

    _descriptionText = new cuttlefish::WrappedText(&_window, _x+10, _y+74, _w-20);
    _descriptionText->setText("AoEModifier $$ ratio : $ratio $$ range : $range", {0,0,0});
    _descriptionText->updateText("ratio", std::to_string(mod_p._ratio));
    _descriptionText->updateText("range", std::to_string(octopus::to_double(mod_p._range)));
}

void ChoiceSubPanel::updateFromModifier(octopus::ChainingModifier const &mod_p)
{
    _mainIcons.push_back(new cuttlefish::Picture(_icons, 64, 64, {1}, {1}));
    _mainIcons.back()->setState(3);
    _mainIcons.back()->setFrame(2);
    _descriptionText = new cuttlefish::WrappedText(&_window, _x+10, _y+74, _w-20);
    _descriptionText->setText("ChainingModifier $$ nb of chains : $nbOfTicks $$ ratio : $ratio $$ range : $range", {0,0,0});
    _descriptionText->updateText("nbOfTicks", std::to_string(mod_p._nbOfTicks));
    _descriptionText->updateText("ratio", std::to_string(mod_p._ratio));
    _descriptionText->updateText("range", std::to_string(mod_p._range));
}

void ChoiceSubPanel::updateFromModifier(octopus::DotModifier const &mod_p)
{
    _mainIcons.push_back(new cuttlefish::Picture(_icons, 64, 64, {1}, {1}));
    _mainIcons.back()->setState(3);
    _mainIcons.back()->setFrame(3);

    _descriptionText = new cuttlefish::WrappedText(&_window, _x+10, _y+74, _w-20);
    _descriptionText->setText("DotModifier $$ nb of ticks : $nbOfTicks $$ seconds between ticks : $tickrate $$ damage : $damage", {0,0,0});
    _descriptionText->updateText("nbOfTicks", std::to_string(mod_p._nbOfTicks));
    _descriptionText->updateText("tickrate", std::to_string(mod_p._tickRate/100.));
    _descriptionText->updateText("damage", std::to_string(mod_p._dmg));
}

void ChoiceSubPanel::updateFromModifier(octopus::LifeStealModifier const &mod_p)
{
    _mainIcons.push_back(new cuttlefish::Picture(_icons, 64, 64, {1}, {1}));
    _mainIcons.back()->setState(3);
    _mainIcons.back()->setFrame(4);

    _descriptionText = new cuttlefish::WrappedText(&_window, _x+10, _y+74, _w-20);
    _descriptionText->setText("LifeStealModifier $$ ratio : $ratio %", {0,0,0});
    _descriptionText->updateText("ratio", std::to_string(mod_p._ratio*100.));
}

void ChoiceSubPanel::updateIconsPosition()
{
    if(_mainIcons.size() == 2)
    {
        _mainIcons.front()->setDestination(_x + double(_w)/3 - 40, _y+10, 64, 64);
        _mainIcons.back()->setDestination(_x + 2*double(_w)/3 - 24, _y+10, 64, 64);
    }
    else
    {
        size_t count_l = 0;
        for(cuttlefish::Picture * pic_l : _mainIcons)
        {
            pic_l->setDestination(_x+ (count_l+1) * (double(_w)/(_mainIcons.size()+1))-32, _y+10, 64, 64);
            ++count_l;
        }
    }
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
