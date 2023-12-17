
#include "TextureDrawer.h"

#include <algorithm>

namespace godot
{
	void TextureDrawer::_ready()
	{}

    void TextureDrawer::set_texture(AtlasTexture *texture_p)
	{
		_texture = texture_p;
	}
    AtlasTexture * TextureDrawer::get_texture() const
	{
		return _texture;
	}

	void TextureDrawer::add_point(Vector2 const &pos_p)
	{
		_positions.push_back(pos_p);
		std::sort(_positions.begin(), _positions.end(),
			[](Vector2 const & a, Vector2 const & b)
			{
				return a.y < b.y;
			}
		);
	}

	void TextureDrawer::_draw()
	{
		if(_texture && _texture->get_atlas().is_valid())
		{
			for(Vector2 const &pos_l : _positions)
			{
				draw_texture_rect_region(_texture->get_atlas(), Rect2(pos_l, _texture->get_region().size) , _texture->get_region());
			}
		}
	}

	void TextureDrawer::_bind_methods()
	{
		ClassDB::bind_method(D_METHOD("set_texture", "texture"), &TextureDrawer::set_texture);
		ClassDB::bind_method(D_METHOD("get_texture"), &TextureDrawer::get_texture);
		ClassDB::bind_method(D_METHOD("add_point", "position"), &TextureDrawer::add_point);

		ADD_GROUP("TextureDrawer", "TextureDrawer_");
		ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "AtlasTexture"), "set_texture", "get_texture");
	}

} // namespace godot

