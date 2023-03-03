#include "Tilemap.hh"

#include "sprite/SpriteLibrary.hh"
#include "sprite/Sprite.hh"
#include "window/Window.hh"

using octopus::to_int;

namespace cuttlefish
{

Tilemap::Tilemap(unsigned long size_p, SpriteLibrary const &spriteLib_p, std::string const &idTiles_p, std::string const &idDetails_p)
    : _size(size_p), _lib(spriteLib_p), _idTiles(idTiles_p), _idDetails(idDetails_p), _gen(42)
{}

Tilemap::~Tilemap()
{
    for(Sprite *sprite_l :_sprites)
    {
        delete sprite_l;
    }
}

void Tilemap::generate()
{
    // clean up
    for(Sprite *sprite_l :_sprites)
    {
        delete sprite_l;
    }
    _sprites.clear();
    _tiles.clear();
    _mapSprites.clear();
    _details.clear();

    // create tiles map
    for(unsigned long i = 0 ; i < _size ; ++ i)
    {
        _tiles.emplace_back(_size, 0);
        _mapSprites.emplace_back(_size, nullptr);
    }

    /// fill tiles
    // get nb states from frames per state
    SpriteTemplate const &template_l = _lib.getSpriteTemplate(_idTiles);
    size_t nbState_l = template_l.nbFramesPerState.size();

    std::uniform_int_distribution<> dist_l(0, nbState_l-1);

    for(size_t i = 0 ; i < _size ; ++ i)
    {
        for(size_t j = 0 ; j < _size ; ++ j)
        {
            _tiles[i][j] = dist_l(_gen);

            _sprites.push_back(_lib.createSprite(0, _idTiles, false));
            _sprites.back()->setState(_tiles[i][j]);
            _sprites.back()->setPosition(template_l.width*template_l.scale*i, template_l.height*template_l.scale*j);

            _mapSprites[i][j] = _sprites.back();
        }
    }
}

void Tilemap::render(Window & window_p, double elapsed_p)
{
    long long minX_l = to_int(window_p.getWorldVector(0, 0).x);
    long long maxX_l = minX_l + to_int(window_p.getWindowSize().x) + 1;
    long long minY_l = to_int(window_p.getWorldVector(0, 0).y);
    long long maxY_l = minY_l + to_int(window_p.getWindowSize().y) + 1;

    for(long long x = minX_l ; x <= maxX_l && x < _mapSprites.size(); ++x )
    {
        for(long long y = minY_l ; y <= maxY_l && x < _mapSprites[x].size(); ++y )
        {
            _mapSprites[x][y]->update(elapsed_p);
            _mapSprites[x][y]->render(window_p);
        }
    }
}

void Tilemap::renderScaled(Window & window_p, double scaledX_p, double scaledY_p)
{
    for(Sprite *sprite_l :_sprites)
    {
        sprite_l->render(window_p, scaledX_p, scaledY_p, true);
    }
}

} // namespace cuttlefish
