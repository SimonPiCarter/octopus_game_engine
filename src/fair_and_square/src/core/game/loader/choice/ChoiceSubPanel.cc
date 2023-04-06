#include "ChoiceSubPanel.hh"

#include "text/WrappedText.hh"
#include "text/Text.hh"
#include "window/Window.hh"

#include "core/lang/LangEntries.hh"

namespace fas
{

OneOptionPanel::OneOptionPanel(cuttlefish::Window &window_p, int x, int y, int w, cuttlefish::Texture const *icons_p, cuttlefish::Texture const *statsIcons_p)
    : _window(window_p), _x(x), _y(y), _w(w)
    , _icons(icons_p)
    , _statsIcons(statsIcons_p)
{}

OneOptionPanel::~OneOptionPanel()
{
    delete _descriptionText;
    delete _statIcon;
    delete _statText;
    for(cuttlefish::Picture * pic_l : _mainIcons)
    {
        delete pic_l;
    }
}

void OneOptionPanel::display(cuttlefish::Window &window_p)
{
    for(cuttlefish::Picture * pic_l : _mainIcons)
    {
        pic_l->display(window_p);
    }
    if(_descriptionText)
    {
        _descriptionText->display(window_p);
    }
    if(_statIcon)
    {
        _statIcon->display(window_p);
    }
    if(_statText)
    {
        _statText->display(window_p);
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

void setUpStatIcon(cuttlefish::Picture * statIcon_p, octopus::TyppedBuff::Type type_p)
{
    switch (type_p)
    {
    case octopus::TyppedBuff::Type::Speed:
        statIcon_p->setState(2);
        statIcon_p->setFrame(1);
        break;
    case octopus::TyppedBuff::Type::FullReload:
        statIcon_p->setState(3);
        statIcon_p->setFrame(0);
        break;
    case octopus::TyppedBuff::Type::Damage:
        statIcon_p->setState(1);
        statIcon_p->setFrame(0);
        break;
    case octopus::TyppedBuff::Type::Armor:
        statIcon_p->setState(2);
        statIcon_p->setFrame(0);
        break;
    case octopus::TyppedBuff::Type::HpMax:
        statIcon_p->setState(0);
        statIcon_p->setFrame(0);
        break;
    default:
        break;
    }
}

void OneOptionPanel::update(BuffOption const &option_p)
{
    delete _descriptionText; _descriptionText = nullptr;
    delete _statIcon; _statIcon = nullptr;
    delete _statText; _statText = nullptr;
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
        offset_l += 74;
    }

    if(option_p._buff._type == octopus::TyppedBuff::Type::Speed
    || option_p._buff._type == octopus::TyppedBuff::Type::FullReload
    || option_p._buff._type == octopus::TyppedBuff::Type::Damage
    || option_p._buff._type == octopus::TyppedBuff::Type::Armor
    || option_p._buff._type == octopus::TyppedBuff::Type::HpMax)
    {
        _statIcon = new cuttlefish::Picture(_statsIcons, 32, 32, {1}, {1});
        setUpStatIcon(_statIcon, option_p._buff._type);
        _statIcon->setDestination(_x+_w/2-16, _y+offset_l, 32, 32);
        offset_l += 34;

        _statText = new cuttlefish::Text(&_window, {0,0,0}, 0, 0);
        if(!is_zero(option_p._buff._offset))
        {
            _statText->setText((option_p._buff._offset>0?std::string("+"):std::string(""))+std::to_string(int(option_p._buff._offset)));
            _statText->setPosition(_x+_w/2-_statText->getWidth()/2, _y+offset_l);
        }
        else if(!is_zero(option_p._buff._coef))
        {
            _statText->setText((option_p._buff._coef>0?std::string("+"):std::string(""))+std::to_string(int(option_p._buff._coef*100))+"%");
            _statText->setPosition(_x+_w/2-_statText->getWidth()/2, _y+offset_l);
        }
    }
    else
    {
        _descriptionText = new cuttlefish::WrappedText(&_window, _x+10, _y+offset_l, _w-20);
        _descriptionText->setText(LangEntries::GetInstance()->getEntry("choiceSubPanel"), {0,0,0});
        _descriptionText->updateText("type_val", octopus::to_string(option_p._buff._type));
        _descriptionText->updateText("offset_val", std::to_string(int(option_p._buff._offset)));
        _descriptionText->updateText("coef_val", std::to_string(int(option_p._buff._coef*100.))+"%");
        _descriptionText->updateText("model_val", option_p._model);
    }
}

void OneOptionPanel::update(ModifierOption const &option_p)
{
    delete _descriptionText; _descriptionText = nullptr;
    delete _statIcon; _statIcon = nullptr;
    delete _statText; _statText = nullptr;
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

void OneOptionPanel::updateFromModifier(octopus::NoModifier const &mod_p)
{
    _mainIcons.push_back(new cuttlefish::Picture(_icons, 64, 64, {1}, {1}));
    _mainIcons.back()->setState(3);
    _mainIcons.back()->setFrame(0);

    _descriptionText = new cuttlefish::WrappedText(&_window, _x+10, _y+74, _w-20);
    _descriptionText->setText(LangEntries::GetInstance()->getEntry("NoModifierChoice"), {0,0,0});
}

void OneOptionPanel::updateFromModifier(octopus::AoEModifier const &mod_p)
{
    _mainIcons.push_back(new cuttlefish::Picture(_icons, 64, 64, {1}, {1}));
    _mainIcons.back()->setState(3);
    _mainIcons.back()->setFrame(1);

    _descriptionText = new cuttlefish::WrappedText(&_window, _x+10, _y+74, _w-20);
    _descriptionText->setText(LangEntries::GetInstance()->getEntry("AoEModifierChoice"), {0,0,0});
    _descriptionText->updateText("ratio", std::to_string(mod_p._ratio));
    _descriptionText->updateText("range", std::to_string(octopus::to_double(mod_p._range)));
}

void OneOptionPanel::updateFromModifier(octopus::ChainingModifier const &mod_p)
{
    _mainIcons.push_back(new cuttlefish::Picture(_icons, 64, 64, {1}, {1}));
    _mainIcons.back()->setState(3);
    _mainIcons.back()->setFrame(2);
    _descriptionText = new cuttlefish::WrappedText(&_window, _x+10, _y+74, _w-20);
    _descriptionText->setText(LangEntries::GetInstance()->getEntry("ChainingModifierChoice"), {0,0,0});
    _descriptionText->updateText("nbOfTicks", std::to_string(mod_p._nbOfTicks));
    _descriptionText->updateText("ratio", std::to_string(mod_p._ratio));
    _descriptionText->updateText("range", std::to_string(mod_p._range));
}

void OneOptionPanel::updateFromModifier(octopus::DotModifier const &mod_p)
{
    _mainIcons.push_back(new cuttlefish::Picture(_icons, 64, 64, {1}, {1}));
    _mainIcons.back()->setState(3);
    _mainIcons.back()->setFrame(3);

    _descriptionText = new cuttlefish::WrappedText(&_window, _x+10, _y+74, _w-20);
    _descriptionText->setText(LangEntries::GetInstance()->getEntry("DotModifierChoice"), {0,0,0});
    _descriptionText->updateText("nbOfTicks", std::to_string(mod_p._nbOfTicks));
    _descriptionText->updateText("tickrate", std::to_string(1./(mod_p._tickRate/100.)));
    _descriptionText->updateText("damage", std::to_string(mod_p._dmg));
}

void OneOptionPanel::updateFromModifier(octopus::LifeStealModifier const &mod_p)
{
    _mainIcons.push_back(new cuttlefish::Picture(_icons, 64, 64, {1}, {1}));
    _mainIcons.back()->setState(3);
    _mainIcons.back()->setFrame(4);

    _descriptionText = new cuttlefish::WrappedText(&_window, _x+10, _y+74, _w-20);
    _descriptionText->setText(LangEntries::GetInstance()->getEntry("LifeStealModifierChoice"), {0,0,0});
    _descriptionText->updateText("ratio", std::to_string(mod_p._ratio*100.));
}

void OneOptionPanel::updateIconsPosition()
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

ChoiceSubPanel::ChoiceSubPanel(cuttlefish::Window &window_p, int x, int y, int w, cuttlefish::Picture const &background_p, cuttlefish::Texture const *icons_p, cuttlefish::Texture const *statsIcons_p)
    : _window(window_p), _x(x), _y(y), _w(w)
    , _background(background_p)
    , _optionPanelPlayer(window_p, x, y, w, icons_p, statsIcons_p)
    , _optionPanelEnemy(window_p, x, y+172, w, icons_p, statsIcons_p)
{}

void ChoiceSubPanel::update(Option const &opt_p)
{
    std::visit([&](auto && arg) { _optionPanelPlayer.update(arg); }, opt_p._playerOption);
    std::visit([&](auto && arg) { _optionPanelEnemy.update(arg); }, opt_p._enemyOption);
}

void ChoiceSubPanel::display(cuttlefish::Window &window_p)
{
    _background.display(window_p);
    _optionPanelPlayer.display(window_p);
    _optionPanelEnemy.display(window_p);
}

cuttlefish::Picture const &ChoiceSubPanel::getBackground() const
{
    return _background;
}

cuttlefish::Picture &ChoiceSubPanel::getBackground()
{
    return _background;
}

OneOptionPanel &ChoiceSubPanel::getOptionPanelPlayer()
{
    return _optionPanelPlayer;
}

} // namespace fas
