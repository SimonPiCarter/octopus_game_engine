#pragma once

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>

namespace godot {

class Controller;

struct StaticPropsInstance
{
	/// @brief position in the world (this is used to sort the textures)
	Vector2 position;
	/// @brief offset to apply to the texture to display it
	Vector2 offset;
	AtlasTexture * texture;
	/// @brief set to true if the props should be hiding itself
	bool hidden;
	/// @brief current alpha
	double alpha;
};

class StaticPropsDrawer : public Node2D {
	GDCLASS(StaticPropsDrawer, Node2D)

public:
	void add_instance(Vector2 const &pos_p, Vector2 const &offset_p, AtlasTexture *texture_p);

	void set_controller(Controller * controller_p);

	void _ready() override;
	void _process(double delta_p) override;
	void _draw() override;

	// Will be called by Godot when the class is registered
	// Use this to add properties to your class
	static void _bind_methods();

private:
	std::vector<StaticPropsInstance> _instances;

	/// @brief controller pointer is used to check for non static entities behind every instance
	Controller const * _controller = nullptr;

	/// @brief time elapsed since last referesh for non static check
	double _elapsed = 0.;
};

}
