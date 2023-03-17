#ifndef __GameLoop__
#define __GameLoop__

namespace octopus
{
class Controller;
} // namespace octopus


namespace cuttlefish
{
class Window;
class DescPanel;
class OptionPanel;
class Minimap;
class Panel;
class SpriteLibrary;
class StatsPanel;
class Tilemap;
class World;

class GameLoop
{
public:
    GameLoop(
        DescPanel &descPanel_p,
        OptionPanel &divinityPanel_p,
        Minimap &minimap_p,
        octopus::Controller &controller_p,
        Panel &panel_p,
        SpriteLibrary &spriteLibrary_p,
        StatsPanel &statsPanel_p,
        Tilemap &tilemap_p,
        World & world_p
    ) : _controller(controller_p),
        _descPanel(descPanel_p),
        _optionPanel(divinityPanel_p),
        _minimap(minimap_p),
        _panel(panel_p),
        _spriteLibrary(spriteLibrary_p),
        _statsPanel(statsPanel_p),
        _tilemap(tilemap_p),
        _world(world_p)
    {}

    void runLoop(Window &window_p);

private:
    DescPanel & _descPanel;
    OptionPanel & _optionPanel;
    Minimap & _minimap;
    octopus::Controller & _controller;
    Panel & _panel;
    SpriteLibrary & _spriteLibrary;
    StatsPanel & _statsPanel;
    Tilemap & _tilemap;
    World & _world;
};

} // namespace cuttlefish



#endif