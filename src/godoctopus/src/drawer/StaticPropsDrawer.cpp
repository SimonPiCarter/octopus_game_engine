
#include "StaticPropsDrawer.h"

#include <algorithm>
#include "Controller.h"
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot
{
	void StaticPropsDrawer::_ready()
	{}

	void StaticPropsDrawer::add_instance(Vector2 const &pos_p, Vector2 const &offset_p, AtlasTexture *texture_p)
	{
		_instances.push_back({pos_p, offset_p, texture_p, false, 1.});
		std::sort(_instances.begin(), _instances.end(),
			[](StaticPropsInstance const & a, StaticPropsInstance const & b)
			{
				return a.position.y < b.position.y;
			}
		);
	}

	void StaticPropsDrawer::set_controller(Controller * controller_p)
	{
		_controller = controller_p;
	}

	void StaticPropsDrawer::_process(double delta_p)
	{
		Node2D::_process(delta_p);

		_elapsed += delta_p;

		if(_elapsed > 1. && _controller)
		{
			_elapsed = 0.;

			for(StaticPropsInstance &instance_l : _instances)
			{
				Vector2 vec_l = instance_l.position/32.;

				instance_l.hidden = _controller->hasNonStaticBehindFromPos(instance_l.position/32., 4, 4);
			}
		}

		for(StaticPropsInstance &instance_l : _instances)
		{
			instance_l.alpha = instance_l.alpha + (instance_l.hidden?-1.:1.)*delta_p;
			instance_l.alpha = std::max(0.25, std::min(1., instance_l.alpha));
		}

		queue_redraw();
	}

	void StaticPropsDrawer::_draw()
	{
		for(StaticPropsInstance const &instance_l : _instances)
		{
			if(instance_l.texture && instance_l.texture->get_atlas().is_valid())
			{
				draw_texture_rect_region(instance_l.texture->get_atlas(),
					Rect2(instance_l.position+instance_l.offset, instance_l.texture->get_region().size),
					instance_l.texture->get_region(),
					Color(1,1,1,instance_l.alpha));
			}
		}
	}

	void StaticPropsDrawer::_bind_methods()
	{
		ClassDB::bind_method(D_METHOD("add_instance", "position", "offset", "texture"), &StaticPropsDrawer::add_instance);
		ClassDB::bind_method(D_METHOD("set_controller", "controller"), &StaticPropsDrawer::set_controller);

		ADD_GROUP("StaticPropsDrawer", "StaticPropsDrawer_");
	}

} // namespace godot

