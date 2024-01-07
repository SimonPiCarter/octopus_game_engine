#pragma once

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>

namespace godot {

struct AnimationInstance
{
	/// @brief offset to apply to the texture to display it
	Vector2 pos;
	Vector2 offset;
	Ref<SpriteFrames> animation;
	bool enabled = true;
	double start = 0.;
	int idx = 0;
};

class AnimationDrawer : public Node2D {
	GDCLASS(AnimationDrawer, Node2D)

public:
	void add_instance(Vector2 const &pos_p, Vector2 const &offset_p, Ref<SpriteFrames> const & animation_p);

	void _ready() override;
	void _draw() override;
	void _process(double delta_p) override;

	// Will be called by Godot when the class is registered
	// Use this to add properties to your class
	static void _bind_methods();

private:
	std::vector<AnimationInstance> _instances;
	std::list<size_t> _freeIdx;

	/// @brief time sine last position update
	double _elapsedTime = 0.;
};

}
