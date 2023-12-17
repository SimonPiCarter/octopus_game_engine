#pragma once

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>

namespace godot {

class Controller;

struct MultiTextureInstance
{
	/// @brief position in the world (this is used to sort the textures)
	Vector2 position;
	/// @brief offset to apply to the texture to display it
	Vector2 offset;
	AtlasTexture * texture;
};

class MultiTextureDrawer : public Node2D {
	GDCLASS(MultiTextureDrawer, Node2D)

public:
	void add_instance(Vector2 const &pos_p, Vector2 const &offset_p, AtlasTexture *texture_p);

	void _ready() override;
	void _draw() override;

	// Will be called by Godot when the class is registered
	// Use this to add properties to your class
	static void _bind_methods();

private:
	std::vector<MultiTextureInstance> _instances;
};

}
