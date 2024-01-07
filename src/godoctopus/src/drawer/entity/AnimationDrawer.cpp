
#include "AnimationDrawer.h"
#include <godot_cpp/variant/utility_functions.hpp>

#include <algorithm>

namespace godot
{
	void AnimationDrawer::_ready()
	{}

	void AnimationDrawer::add_instance(Vector2 const &pos_p, Vector2 const &offset_p, Ref<SpriteFrames> const & animation_p)
	{
		if(_freeIdx.empty())
		{
			_instances.push_back({pos_p, offset_p, animation_p, true, _elapsedTime, 0});
		}
		else
		{
			size_t idx_l = _freeIdx.front();
			_freeIdx.pop_front();
			_instances[idx_l].pos = pos_p;
			_instances[idx_l].offset = offset_p;
			_instances[idx_l].animation = animation_p;
			_instances[idx_l].enabled = true;
			_instances[idx_l].start = _elapsedTime;
			_instances[idx_l].idx = 0;
		}
	}

	void AnimationDrawer::_process(double delta_p)
	{
		_elapsedTime += delta_p;
		queue_redraw();
	}

	void AnimationDrawer::_draw()
	{
		//RenderingServer::canvas_item_clear(get_canvas_item());
		size_t i = 0;
		for(AnimationInstance &instance_l : _instances)
		{
			if(instance_l.enabled && instance_l.animation.is_valid() && instance_l.animation->get_frame_count("default") > 0)
			{
				double frameTime_l = instance_l.animation->get_frame_duration("default", instance_l.idx) / instance_l.animation->get_animation_speed("default") ;
				double nextFrameTime_l = instance_l.start + instance_l.idx * frameTime_l;
				if(_elapsedTime >= nextFrameTime_l)
				{
					++instance_l.idx;
				}
				if(instance_l.idx >= instance_l.animation->get_frame_count("default"))
				{
					// free animation
					instance_l.enabled = false;
					_freeIdx.push_back(i);
				}
				else
				{
					// draw animaton
					Ref<Texture2D> texture_l = instance_l.animation->get_frame_texture("default", instance_l.idx);
					texture_l->draw(get_canvas_item(), instance_l.pos+instance_l.offset);
				}
			}
			++i;
		}
	}

	void AnimationDrawer::_bind_methods()
	{
		ClassDB::bind_method(D_METHOD("add_instance", "position", "offset", "animation"), &AnimationDrawer::add_instance);

		ADD_GROUP("AnimationDrawer", "AnimationDrawer_");
	}

} // namespace godot

