
#include "MultiTextureDrawer.h"

#include <algorithm>

namespace godot
{
	void MultiTextureDrawer::_ready()
	{}

	void MultiTextureDrawer::add_instance(Vector2 const &pos_p, Vector2 const &offset_p, AtlasTexture *texture_p)
	{
		_instances.push_back({pos_p, offset_p, texture_p});
		std::sort(_instances.begin(), _instances.end(),
			[](MultiTextureInstance const & a, MultiTextureInstance const & b)
			{
				return a.position.y < b.position.y;
			}
		);
	}

	void MultiTextureDrawer::_draw()
	{
		for(MultiTextureInstance const &instance_l : _instances)
		{
			if(instance_l.texture && instance_l.texture->get_atlas().is_valid())
			{
				draw_texture_rect_region(instance_l.texture->get_atlas(),
					Rect2(instance_l.position+instance_l.offset, instance_l.texture->get_region().size),
					instance_l.texture->get_region());
			}
		}
	}

	void MultiTextureDrawer::_bind_methods()
	{
		ClassDB::bind_method(D_METHOD("add_instance", "position", "offset", "texture"), &MultiTextureDrawer::add_instance);

		ADD_GROUP("MultiTextureDrawer", "MultiTextureDrawer_");
	}

} // namespace godot

