#include "Selection.hh"

namespace cuttlefish
{

void Selection::removeSprite(Sprite * sprite_p)
{
	_sprites.remove(sprite_p);
	if(_sprite == sprite_p)
	{
		if(_sprites.empty())
		{
			_sprite = *_sprites.begin();
		}
		else
		{
			_sprite = nullptr;
		}
	}
}

} // cuttlefish
