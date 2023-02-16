#ifndef __Tilemap__
#define __Tilemap__

#include <list>
#include <random>
#include <string>
#include <vector>

namespace cuttlefish
{
class Sprite;
class SpriteLibrary;
class Window;

struct DetailInfo
{
    int state;
    double x;
    double y;
};

/// @brief This class allow the definition of a tilemap
/// for easy generation and render
class Tilemap
{
public:
    Tilemap(unsigned long size_p, SpriteLibrary const &spriteLib_p, std::string const &idTiles_p, std::string const &idDetails_p);
    ~Tilemap();

    /// @brief generate internal state
    /// generate all sprites based on internal state
    void generate();

    void render(Window & window_p, double elapsed_p);

protected:
    /// @brief internal state for tiles (state per tile)
    std::vector<std::vector<int> > _tiles;

    /// @brief internal state for details
    std::vector<DetailInfo> _details;

    std::list<Sprite *> _sprites;

    unsigned long const _size;
    SpriteLibrary const &_lib;
    std::string const _idTiles;
    std::string const _idDetails;

    std::mt19937 _gen;
};

} // namespace cuttlefish


#endif