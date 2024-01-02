
#include "EntityDrawer.h"
#include <godot_cpp/variant/utility_functions.hpp>

#include <algorithm>

namespace godot
{
	void EntityDrawer::_ready()
	{}

	void EntityDrawer::add_instance(Vector2 const &pos_p, Vector2 const &offset_p, Ref<AtlasTexture> const & texture_p)
	{
		_instances.push_back({offset_p, texture_p});

		_newPos.push_back(pos_p);
		_oldPos.push_back(pos_p);

		// std::sort(_instances.begin(), _instances.end(),
		// 	[](EntityInstance const & a, EntityInstance const & b)
		// 	{
		// 		return a.position.y < b.position.y;
		// 	}
		// );
	}

	void EntityDrawer::update_pos()
	{
		_elapsedTime = 0.;
		// swap positions
		std::swap(_oldPos, _newPos);
	}

	std::vector<Vector2> & EntityDrawer::getNewPos()
	{
		return _newPos;
	}

	void EntityDrawer::_process(double delta_p)
	{
		_elapsedTime += delta_p;
		queue_redraw();
	}

	void EntityDrawer::_draw()
	{
		size_t i = 0;
		for(EntityInstance const &instance_l : _instances)
		{
			if(instance_l.texture.is_valid() && instance_l.texture->get_atlas().is_valid() && instance_l.enabled)
			{
				Vector2 diff_l = _newPos[i] - _oldPos[i];
				Vector2 pos_l = instance_l.offset + _oldPos[i] + diff_l * std::min<double>(1., _elapsedTime/_timeStep);
				draw_texture_rect_region(instance_l.texture->get_atlas(),
					Rect2(pos_l, instance_l.texture->get_region().size),
					instance_l.texture->get_region());
			}
			++i;
		}
	}

	void EntityDrawer::_bind_methods()
	{
		ClassDB::bind_method(D_METHOD("add_instance", "position", "offset", "texture"), &EntityDrawer::add_instance);
		ClassDB::bind_method(D_METHOD("update_pos"), &EntityDrawer::update_pos);

		ADD_GROUP("EntityDrawer", "EntityDrawer_");
	}

} // namespace godot

