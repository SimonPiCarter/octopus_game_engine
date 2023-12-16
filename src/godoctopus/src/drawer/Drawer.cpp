
#include "Drawer.h"

#include <godot_cpp/classes/random_number_generator.hpp>

namespace godot
{
	void Drawer::_ready()
	{
		update();
	}

	void Drawer::update()
	{
		_bullets.resize(_number, bullet());
		RandomNumberGenerator rand_l;

		for(int i = 0 ; i < _number ; ++ i)
		{
			_bullets[i].pos.x = rand_l.randf_range(0, get_viewport_rect().size.x) + get_viewport_rect().size.x;
			_bullets[i].pos.y = rand_l.randf_range(0, get_viewport_rect().size.y);
			_bullets[i].speed = rand_l.randf_range(20, 80);
		}
	}

	void Drawer::move(double delta)
	{
		double offset_l = get_viewport_rect().size.x + 16.;
		for(int i = 0 ; i < _number ; ++ i)
		{
			_bullets[i].pos.x -= _bullets[i].speed * delta;
			if(_bullets[i].pos.x < -16.)
			{
				_bullets[i].pos.x = offset_l;
			}
		}
	}

	void Drawer::_draw()
	{
		if(_ref.is_valid())
		{
			for(int i = 0 ; i < _number ; ++ i)
			{
				draw_texture_rect_region(_ref, Rect2(_bullets[i].pos, Vector2(32,32)) , Rect2(0,0, 32,32));
			}
		}
	}

	void Drawer::_bind_methods()
	{
		ClassDB::bind_method(D_METHOD("set_texture", "texture"), &Drawer::set_texture);
		ClassDB::bind_method(D_METHOD("get_texture"), &Drawer::get_texture);
		ClassDB::bind_method(D_METHOD("set_number", "number"), &Drawer::set_number);
		ClassDB::bind_method(D_METHOD("get_number"), &Drawer::get_number);
		ClassDB::bind_method(D_METHOD("get_bullet_pos", "index"), &Drawer::get_bullet_pos);
		ClassDB::bind_method(D_METHOD("move", "delta"), &Drawer::move);
		ClassDB::bind_method(D_METHOD("update"), &Drawer::update);

		ADD_GROUP("Drawer", "Drawer_");
		ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture"), "set_texture", "get_texture");
		ADD_PROPERTY(PropertyInfo(Variant::INT, "number"), "set_number", "get_number");
	}

} // namespace godot

