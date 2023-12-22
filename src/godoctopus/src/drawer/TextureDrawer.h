#pragma once

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>

namespace godot {

class Controller;

class TextureDrawer : public Node2D {
	GDCLASS(TextureDrawer, Node2D)

public:
	void set_texture(AtlasTexture *texture_p);
	AtlasTexture * get_texture() const;

	void add_point(Vector2 const &pos_p);

	void _ready() override;
	void _draw() override;

	// Will be called by Godot when the class is registered
	// Use this to add properties to your class
	static void _bind_methods();

private:
	AtlasTexture * _texture = nullptr;

	std::vector<Vector2> _positions;
};

}
