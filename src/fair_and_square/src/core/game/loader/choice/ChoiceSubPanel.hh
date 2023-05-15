#ifndef __ChoiceSubPanel__
#define __ChoiceSubPanel__

#include <list>

#include "sprite/Picture.hh"

#include "library/model/bonus/BuffGenerators.hh"

namespace cuttlefish
{
    class Window;
    class WrappedText;
    class Text;
    class Texture;
} // namespace cuttlefish


namespace fas
{

class OneOptionPanel
{
public:
    OneOptionPanel(cuttlefish::Window &window_p, int x, int y, int w, cuttlefish::Texture const *icons_p, cuttlefish::Texture const *statsIcons_p);
    ~OneOptionPanel();

    void display(cuttlefish::Window &window_p);

    /// @brief update the sub panel from the buff option
    void update(BuffOption const &option_p);
    /// @brief update the sub panel from the double buff option (not implemented yet)
    void update(DoubleBuffOption const &option_p);
    /// @brief update the sub panel from the modifier option
    void update(ModifierOption const &option_p);
    /// @brief update the sub panel from the div option (not implemented yet)
    void update(DivinityOption const &divOption_p);

    void updateFromModifier(octopus::NoModifier const &mod_p);
    void updateFromModifier(octopus::AoEModifier const &mod_p);
    void updateFromModifier(octopus::ChainingModifier const &mod_p);
    void updateFromModifier(octopus::DotModifier const &mod_p);
    void updateFromModifier(octopus::LifeStealModifier const &mod_p);
    void updateFromModifier(octopus::CompositeModifier const &mod_p);
    void updateFromModifier(octopus::SelfDamageModifier const &mod_p);

    void updateIconsPosition();
private:
	cuttlefish::Window &_window;
    int const _x;
    int const _y;
    int const _w;

    cuttlefish::WrappedText *_descriptionText {nullptr};
    std::list<cuttlefish::Picture *> _mainIcons;

    cuttlefish::Picture * _statIcon {nullptr};
    cuttlefish::Text *_statText {nullptr};

    cuttlefish::Texture const * const _icons;
    cuttlefish::Texture const * const _statsIcons;
};

/// @brief This class holds a sub panel for one
/// option with all the visuals
/// - icons (one or several) to know on what the buff is going to apply
/// - Title text
/// - Description text
/// - icon of the stat boosted (or new attack modifier)
/// - bonus text (if applicable)
class ChoiceSubPanel
{
public:
    ChoiceSubPanel(cuttlefish::Window &window_p, int x, int y, int w, cuttlefish::Picture const & background_p, cuttlefish::Texture const *icons_p, cuttlefish::Texture const *statsIcons_p);

    void update(Option const &opt_p);

    void display(cuttlefish::Window &window_p);

    cuttlefish::Picture const &getBackground() const;
    cuttlefish::Picture &getBackground();

    OneOptionPanel &getOptionPanelPlayer();
private:
	cuttlefish::Window &_window;
    int const _x;
    int const _y;
    int const _w;
    cuttlefish::Picture _background;
    OneOptionPanel _optionPanelPlayer;
    OneOptionPanel _optionPanelEnemy;
};

} // fas

#endif
